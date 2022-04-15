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

void Processor::process() {
    while (true) {
        this->fileQueueMutex->lock();
        if (this->fileQueue.empty()) {
            // Prevents a deadlock
            this->fileQueueMutex->unlock();
            break;
        }
        std::string filename = this->fileQueue.front();
        this->fileQueue.pop();
        this->fileQueueMutex->unlock();

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

        if (!document.IsObject() || !document.HasMember("uuid")) {
            continue;
        }


        std::string *uuid = new std::string(document["uuid"].GetString());
//        std::string author = document["author"].GetString();

//        std::vector<std::string> orgs = {};
//        // Check if array empty
//        for (auto &org: document["organizations"].GetArray()) {
//            orgs.emplace_back(org.GetString());
//        }

//        Article art = {
//                .uuid = *uuid,
//                .filename = filename,
//                .orgList = orgs,
//                .author = author,
//        };

//        std::thread tableFillAuthorThread(&Processor::fillAuthors, this, uuid, author);
//        std::thread tableFillOrgsThread(&Processor::fillOrganization, this, orgs, uuid);
//        std::thread tableFillArticlesThread(&Processor::fillArticle, this, art);

//        this->fillAuthors(*uuid,
//                          author);    ////Saying that this has values that are incorrectly passed, double check this DREW
//        this->fillOrganization(orgs, *uuid);
//        this->fillArticle(art);

        std::istringstream iss(document["text"].GetString());

        // Iterate the istringstream
        // using do-while loop
        tbbAccessor accessor;
        do {
            std::string subs;
            iss >> subs;
            unsigned int hash = 1;
            for (const char &cc: subs) {
                if (std::isalpha(cc)) {
                    hash *= 16777619;
                    hash = hash ^ (cc & 31);
                }
            }
            if (stopWords.hashedLexicon.find(hash) == stopWords.hashedLexicon.end()) {
                if (subs.length() > 6) {
                    Porter2Stemmer::stem(subs);
                    hash = 1;
                    for (const char &cc: subs) {
                        if (std::isalpha(cc)) {
                            hash *= 16777619;
                            hash = hash ^ (cc & 31);
                        }
                    }
                }
                this->tbbMap->operator[](hash).push_back(uuid);
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

bool Processor::safeIsEmpty() {
    this->fileQueueMutex->lock();
    bool empty = this->fileQueue.empty();
    this->fileQueueMutex->unlock();
    return empty;
}

Processor::~Processor() {
//    delete this->fileQueueMutex;
//    delete this->tbbMap;
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


Processor::Processor(TableBundle *tableBundle, avl_tree<unsigned int, tbb::concurrent_vector<std::string *> *> *tree,
                     std::mutex *treeMut) {
    this->tableBundle = tableBundle;
    this->stopWords = StopWords();
    this->wordTree = tree;
    this->wordTreeMutex = treeMut;
    this->totalFiles = 0;
    this->fileQueueMutex = new std::mutex();
    this->filesProcessed = 0;
    this->wordsConverted = 0;
    this->tbbMap = new tbb::concurrent_unordered_map<unsigned int, tbb::concurrent_vector<std::string *>>();
}

std::string Processor::convertToTree() {
    this->wordTreeMutex->lock();
    std::cout << this->tbbMap->size() << std::endl;
    for (auto &word: *this->tbbMap) {
        this->wordTree->insert_overwriting(word.first, &word.second);
    }
    wordsConverted = wordTree->size();
    this->wordTreeMutex->unlock();
    return "Conversion complete";
}

double Processor::getConversionProgress() {
    return (double) this->wordsConverted.load() / (double) this->totalWords;
}

