//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"
#include <experimental/filesystem>
#include "./include/cereal/external/rapidjson/document.h"
#include "./include/termcolor/termcolor.hpp"
#include <thread>
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
        if (fs::is_regular_file(dir_entry) &&
            dir_entry.path().string().substr(dir_entry.path().string().size() - 5, 5) == ".json") {
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

        std::string author = cleanPronoun(document["author"].GetString());

        const auto arr = document["entities"]["organizations"].GetArray();
        std::vector<std::string> orgs;
        for (const auto &org: arr) {
            orgs.emplace_back(cleanPronoun(org["name"].GetString()));
        }

        this->articles->operator[](uuid) = {
                .uuid = uuid,
                .filename = filename,
                .author = author,
                .orgList = orgs,
        };

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
                if (subs.substr(0, 3) != "www") {
                    this->tbbMap->operator[](subs)[uuid]++;
                }
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
    delete this->fileQueueMutex;
    delete this->tbbMap;
}

Processor::Processor(tbb::concurrent_unordered_map<std::string, Article> *pArticles,
                     avl_tree<std::string, std::vector<std::pair<std::string, double>>> *tree,
                     std::mutex *treeMut) {
    this->articles = pArticles;
    this->stopWords = StopWords();
    this->wordTree = tree;
    this->wordTreeMutex = treeMut;
    this->totalFiles = 0;
    this->fileQueueMutex = new std::mutex();
    this->filesProcessed = 0;
    this->wordsConverted = 0;
    this->tbbMap = new tbb::concurrent_unordered_map<std::string, tbb::concurrent_unordered_map<std::string, int>>();
}


#include <math.h>

std::string Processor::convertToTree() {
    this->wordTreeMutex->lock();
    for (auto &wordData: *this->tbbMap) {
        std::string currentWord = wordData.first;
        std::vector<std::pair<std::string, double>> toPush;
        int numDocsThatContainWord = wordData.second.size();
        double inverseDocumentFrequency = log10(double(this->totalFiles) / numDocsThatContainWord);

        for (const auto &uuidData: wordData.second) {
            toPush.emplace_back(
                    std::pair<std::string, double>(uuidData.first, inverseDocumentFrequency * uuidData.second));
        }

        std::sort(toPush.begin(), toPush.end(),
                  [](const std::pair<std::string, double> &a, const std::pair<std::string, double> &b) {
                      return a.second > b.second;
                  });

        this->wordTree->insert_overwriting(wordData.first, toPush);

        this->wordsConverted++;
    }
    this->wordTreeMutex->unlock();
    return "Conversion complete";
}

double Processor::getConversionProgress() {
    return (double) this->wordsConverted.load() / (double) this->totalWords;
}

