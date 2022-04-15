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

/**
 * Adds every article filename to a queue so that the multithreading can process them easily
 * @param folderName The folder name to search
 */
void Processor::fillQueue(const std::string &folderName) {
    // Only files not folders
    for (const fs::directory_entry &dir_entry:
            fs::recursive_directory_iterator(folderName)) {
        if (fs::is_regular_file(dir_entry)) {
            this->fileVector.push_back(dir_entry.path().string());
            this->totalFiles++;
        }
    }
}

void Processor::process() {
    std::hash<std::string> hashObj;
    for (auto &filename: fileVector) {
        std::ifstream file(filename);
        rapidjson::Document document;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        document.Parse(content.c_str());
        file.close();

        auto *uuid = new std::string(document["uuid"].GetString());
        std::istringstream textCorpus(document["text"].GetString());

        std::string subs;
        do {
            textCorpus >> subs;
            unsigned int hashed = 1;
            for (char &cc: subs)
                if (std::isalpha(cc)) {
                    hashed *= 16777619;
                    hashed = hashed ^ (cc & 31);
                }

            if (stopWords.hashedLexicon.find(hashed) == stopWords.hashedLexicon.end())
                this->wordMap->operator[](hashed).push_back(uuid);
        } while (textCorpus);

        filesProcessed++;
    }

    totalWords = wordMap->size();

    for (auto &word: *this->wordMap) {
        this->wordTree->insert_overwriting(word.first, &word.second);
    }

    wordsConverted = wordTree->size();
}


/**
 * Generates the index from a specific folder
 * @param folderName the folder to recursively process through
 * @return A dummy return value so that the function can be called with std::async
 */
void Processor::generateIndex(const std::string &folderName) {
    this->fillQueue(folderName);
    process();
}

double Processor::getProgress() {
    return (double) filesProcessed / (double) totalFiles;
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

double Processor::getConversionProgress() {
    return (double) wordsConverted / (double) totalWords;
}

