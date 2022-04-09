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
#include "./TableBundle.h"
#include "./include/porter2_stemmer/porter2_stemmer.h"
#include "./StopWords.h"

#include "./utils.h"

namespace fs = std::experimental::filesystem;


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

        std::string uuid = document["uuid"].GetString();
        std::string author = document["author"].GetString();

        std::vector<std::string> orgs = {};
        // Check if array empty
        for (auto &org: document["organizations"].GetArray()) {
            if (org.IsObject()) {
                if (org["name"].IsString()) {
                    orgs.push_back(org["name"].GetString());
                }
            }
        }

        Article art = {
                .uuid = uuid,
                .filename = filename,
                .orgList = orgs,
                .author = author,
        };

        std::thread tableFillAuthorThread(&Processor::fillAuthors, this, uuid, author);
        std::thread tableFillOrgsThread(&Processor::fillOrganization, this, orgs, uuid);
        std::thread tableFillArticlesThread(&Processor::fillArticle, this, art);


        std::string text = document["text"].GetString();
        std::istringstream iss(text);

        // Iterate the istringstream
        // using do-while loop
        do {
            std::string subs;
            // Get the word from the istringstream
            iss >> subs;
            toLower(subs);
            removePunctuation(subs);
            if (stopWords.stopWords.find(subs) == stopWords.stopWords.end()) {
                Porter2Stemmer::stem(subs);
                if (subs.length() > 0) {
                    // Add ato avl tree
                    std::vector<std::string> dummyVector = {subs};
                    this->wordMapMutex->lock();
                    auto ref = this->wordMap->operator[](subs);
                    if (std::find(ref.begin(), ref.end(), subs) == ref.end()) {
                        ref.push_back(uuid);
                    }
                    this->wordMapMutex->unlock();
                }
            }
        } while (iss);
        tableFillAuthorThread.join();
        tableFillOrgsThread.join();
        tableFillArticlesThread.join();

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
    t1.join();
    t2.join();
    t3.join();

    this->totalWords = this->wordMap->size();

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
    delete this->fileQueueMutex;
}

void Processor::fillArticle(Article article) {
    this->tableBundle->articlesMutex.lock();
    this->tableBundle->articles->operator[](article.uuid) = article;
    this->tableBundle->articlesMutex.unlock();
}

void Processor::fillOrganization(std::vector<std::string> organizations, std::string uuid) {
    this->tableBundle->orgsMutex.lock();
    for (auto &org: organizations) {
        this->tableBundle->orgs->operator[](org).push_back(uuid);
    }
    this->tableBundle->orgsMutex.unlock();
}

void Processor::fillAuthors(std::string authors, std::string uuid) {
    this->tableBundle->authorsMutex.lock();
    this->tableBundle->authors->operator[](authors).push_back(uuid);
    this->tableBundle->authorsMutex.unlock();
}


Processor::Processor(TableBundle *tableBundle, avl_tree<std::string, std::vector<std::string>> *tree,
                     std::mutex *treeMut) {
    this->tableBundle = tableBundle;
    this->stopWords = StopWords();
    this->wordTree = tree;
    this->wordTreeMutex = treeMut;
    this->totalFiles = 0;
    this->fileQueueMutex = new std::mutex();
    this->wordMap = new std::unordered_map<std::string, std::vector<std::string>>();
    this->wordMapMutex = new std::mutex();
    this->filesProcessed = 0;
    this->wordsConverted = 0;
}

void dummyFunction(std::vector<std::string> &s1, const std::vector<std::string> &s2) {
}

std::string Processor::convertToTree() {
    this->wordTreeMutex->lock();
    this->wordMapMutex->lock();
    for (auto &word: *this->wordMap) {
        // pass first second and empty function
        this->wordTree->insert(word.first, word.second, dummyFunction);
        this->wordsConverted++;
    }
    this->wordTreeMutex->unlock();
    this->wordMapMutex->unlock();
    return "Conversion complete";
}

double Processor::getConversionProgress() {
    return (double) this->wordsConverted.load() / (double) this->totalWords;
}

