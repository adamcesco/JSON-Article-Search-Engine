//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"
#include <experimental/filesystem>
#include "./include/rapidjson/document.h"
#include "./include/termcolor/termcolor.hpp"
#include <thread>
#include <fstream>
#include "./include/porter2_stemmer/porter2_stemmer.h"
#include "./utils.h"

namespace fs = std::experimental::filesystem;

typedef tbb::concurrent_hash_map<std::string, std::vector<std::string>>::accessor tbbAccessor;

/**
 * Adds every article filename to a queue so that the multithreading can process them easily
 * @param folderName The folder name to search
 */
void Processor::fillQueue(std::string folderName) {
    // Only files not folders
    for (const fs::directory_entry &dir_entry:
            fs::recursive_directory_iterator(folderName)) {
        if (fs::is_regular_file(dir_entry)) {
            this->fileQueue.push(dir_entry.path().string());
            this->totalFiles++;
        }
    }
}


void aliasPushBack(std::vector<std::string> &existing, const std::vector<std::string> &newVal) {
    existing.push_back(newVal[0]);
}


void Processor::process() {
    while (true) {
        if (this->fileQueue.empty()) {
            // Prevents a deadlock
            break;
        }

        std::string filename;
        if (!this->fileQueue.try_pop(filename)) {
            break;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Could not open file: " << filename << std::endl;
            continue;
        }
        rapidjson::Document document;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        try {
            document.Parse(content.c_str());
        } catch (std::exception &e) {
            std::cout << termcolor::red << "Could not read file: " << filename << termcolor::reset << std::endl;
            continue;
        }
        file.close();

        std::string uuid = document["uuid"].GetString();
        std::string author = document["author"].GetString();

        std::vector<std::string> orgs = {};
        // Check if array empty
        for (auto &org: document["organizations"].GetArray()) {
            orgs.emplace_back(org.GetString());
        }

        Article art = {
                .uuid = uuid,
                .filename = filename,
                .orgList = orgs,
                .author = author,
        };

//        std::thread tableFillAuthorThread(&Processor::fillAuthors, this, uuid, author);
//        std::thread tableFillOrgsThread(&Processor::fillOrganization, this, orgs, uuid);
//        std::thread tableFillArticlesThread(&Processor::fillArticle, this, art);

        this->fillAuthors(uuid,
                          author);    ////Saying that this has values that are incorrectly passed, double check this DREW
        this->fillOrganization(orgs, uuid);
        this->fillArticle(art);


        std::string text = document["text"].GetString();
        std::istringstream iss(text);

        // Iterate the istringstream
        // using do-while loop
        tbbAccessor accessor;
        do {
            std::string subs;
            // Get the word from the istringstream
            iss >> subs;
            cleanStr(subs);
            if (stopWords.stopWords.find(subs) == stopWords.stopWords.end()) {
                Porter2Stemmer::stem(subs);
                if (subs.length() > 0 && subs.substr(0, 3) != "www") {
                    // Used :https://stackoverflow.com/questions/60586122/tbbconcurrent-hash-mapk-v-sample-code-for-intel-threading-building-blocks-t
                    this->tbbMap->operator[](subs).push_back(uuid);
                    // End quoted code
                }
            }
        } while (iss);
//        tableFillAuthorThread.join();
//        tableFillOrgsThread.join();
//        tableFillArticlesThread.join();

        filesProcessed++;
    }
}


/**
 * Generates the index from a specific folder
 * @param folderName the folder to recursively process through
 * @return A dummy return value so that the function can be called with std::async
 */
std::string Processor::generateIndex(std::string folderName) {
    std::cout << termcolor::red << std::endl << getCenteredText("Generating index...", 80) << std::endl;
    this->fillQueue(folderName);
    std::string fileDisplay = "Total files: " + std::to_string(this->totalFiles);

    std::cout << termcolor::green << getCenteredText(fileDisplay, 80) << std::endl;
    std::cout << termcolor::reset << std::endl;

    // Actually process the files
    std::thread t1(&Processor::process, this);
    std::thread t2(&Processor::process, this);
    std::thread t3(&Processor::process, this);
    std::thread t4(&Processor::process, this);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    this->totalWords = this->tbbMap->size();

    return "Indexing complete";
}

double Processor::getProgress() {
    return (double) this->filesProcessed.load() / (double) this->totalFiles;
}


Processor::~Processor() {
    delete this->tbbMap;
}

void Processor::fillArticle(const Article &article) {
    this->tableBundle->articles->operator[](article.uuid) = article;
}

void Processor::fillOrganization(const std::vector<std::string> &organizations, const std::string &uuid) {
    for (auto &org: organizations) {
        this->tableBundle->orgs->operator[](org).push_back(uuid);
    }
}

void Processor::fillAuthors(const std::string &authors, const std::string &uuid) {
    this->tableBundle->authors->operator[](authors).push_back(uuid);
}


Processor::Processor(TableBundle *tableBundle, avl_tree<std::string, tbb::concurrent_vector<std::string> *> *tree,
                     std::mutex *treeMut) {
    this->tableBundle = tableBundle;
    this->stopWords = StopWords();
    this->wordTree = tree;
    this->wordTreeMutex = treeMut;
    this->totalFiles = 0;
    this->filesProcessed = 0;
    this->wordsConverted = 0;
    this->tbbMap = new tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<std::string>>();
}

void dummyFunction(tbb::concurrent_vector<std::string> &s1, const tbb::concurrent_vector<std::string> &s2) {
}

std::string Processor::convertToTree() {
    this->wordTreeMutex->lock();
    for (auto &word: *this->tbbMap) {
        // pass first second and empty function
        this->wordTree->insert_overwriting(word.first, &word.second);

        this->wordsConverted++;
    }
    this->wordTreeMutex->unlock();
    return "Conversion complete";
}

double Processor::getConversionProgress() {
    return (double) this->wordsConverted.load() / (double) this->totalWords;
}

