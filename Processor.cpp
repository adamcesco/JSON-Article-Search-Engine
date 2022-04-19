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
#include <chrono>
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
            this->fileVect.push_back(dir_entry.path().string());
        }
    }
}

void Processor::process() {
//    std::ofstream stemFile("../data/hashed-inverse-stemmed.txt");
//    std::unordered_map<unsigned int, std::unordered_set<unsigned int>> inverseStemming;
    for (std::string &file_dir: fileVect) {
        std::ifstream file(file_dir);
        rapidjson::Document document;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        document.Parse(content.c_str());
        file.close();

        auto *uuid = &file_dir;
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

            if (stopWords.hashedLexicon.find(hashed) == stopWords.hashedLexicon.end()) {
//                cleanStr(subs);
//                unsigned int first = custom_string_hash(subs);
//                Porter2Stemmer::stem(subs);
//                unsigned int second = custom_string_hash(subs);
//                if (first != second)
//                    inverseStemming[second].emplace(first);
                this->wordMap->operator[](hashed).push_back(uuid);
            }
        } while (textCorpus);
    }

//    for (auto &it: inverseStemming) {
//        stemFile << it.first << " ";
//        for (auto &jt: it.second) {
//            stemFile << jt << " ";
//        }
//        stemFile << std::endl;
//    }
//    stemFile.close();

    for (auto &word: *this->wordMap) {
        this->wordTree->insert_overwriting(word.first, &word.second);
    }
}


/**
 * Generates the index from a specific folder
 * @param folderName the folder to recursively process through
 * @return A dummy return value so that the function can be called with std::async
 */
void Processor::generateIndex(const std::string &folderName) {
    fillQueue(folderName);
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

