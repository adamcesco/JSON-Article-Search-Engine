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


void aliasPushBack(std::vector<std::string *> &existing, const std::vector<std::string *> &newVal) {
    existing.push_back(newVal[0]);
}


void Processor::process() {
    std::hash<std::string> hashObj;
    while (!this->fileQueue.empty()) {
        std::string filename = this->fileQueue.front();
        this->fileQueue.pop();

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


        auto *uuid = new std::string(document["uuid"].GetString());


        std::string text = document["text"].GetString();
        std::istringstream iss(text);

        // Iterate the istringstream
        // using do-while loop
        tbbAccessor accessor;
        do {
            std::string subs;
            iss >> subs;
            cleanStr(subs);
            unsigned int hashed = hashObj(subs);
            if (stopWords.hashedLexicon.find(hashed) == stopWords.hashedLexicon.end()) {
//            Porter2Stemmer::stem(subs);
                this->wordMap->operator[](hashed).push_back(uuid);
            }
        } while (iss);

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
    process();

    this->totalWords = this->wordMap->size();

    return "Indexing complete";
}

double Processor::getProgress() {
    return (double) this->filesProcessed.load() / (double) this->totalFiles;
}

Processor::~Processor() {
//    delete this->fileQueueMutex;
//    delete this->wordMap;
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


Processor::Processor(avl_tree<unsigned int, std::vector<std::string *> *> *tree) {
//    this->tableBundle = tableBundle;
    this->stopWords = StopWords();
    this->wordTree = tree;
    this->totalFiles = 0;
    this->filesProcessed = 0;
    this->wordsConverted = 0;
    this->wordMap = new std::unordered_map<unsigned int, std::vector<std::string *>>();
}

std::string Processor::convertToTree() {
    std::cout << this->wordMap->size() << std::endl;
    for (auto &word: *this->wordMap) {
        this->wordTree->insert_overwriting(word.first, &word.second);
        this->wordsConverted++;
    }
    return "Conversion complete";
}

double Processor::getConversionProgress() {
    return (double) this->wordsConverted.load() / (double) this->totalWords;
}

