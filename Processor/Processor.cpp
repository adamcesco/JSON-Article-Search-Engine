//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include <experimental/filesystem>
#include <thread>
#include "Processor.h"
#include "../external/termcolor/termcolor.hpp"
#include "../utilities/Pipelines.h"
#include "../ProgressBar/ProgressBar.h"

namespace fs = std::experimental::filesystem;

typedef tbb::concurrent_hash_map<std::string, std::vector<std::string>>::accessor tbbAccessor;

/**
 * Adds every article filename to a queue so that the multithreading can process them easily
 * @param folderName The folder name to search
 */
void Processor::fillQueue(std::string folderName) {
    // Only files not folders
    this->totalFiles = 0;
    for (const fs::directory_entry &dir_entry:
            fs::recursive_directory_iterator(folderName)) {
        if (fs::is_regular_file(dir_entry) &&
            dir_entry.path().string().substr(dir_entry.path().string().size() - 5, 5) == ".json") {
            this->fileQueue.push(dir_entry.path().string());
            this->totalFiles++;
        }
    }
}

void Processor::process() {
    this->filesProcessed = 0;
    this->totalOrgs = 0;
    this->totalPeople = 0;
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

        std::string author = pipeline::cleanPropnoun(document["author"].GetString());

        const auto arr = document["entities"]["organizations"].GetArray();
        std::vector<std::string> orgs;
        for (const auto &it: arr) {
            orgs.emplace_back(pipeline::cleanPropnoun(it["name"].GetString()));
        }

        this->totalOrgs += orgs.size();

        if (!author.empty())
            this->totalPeople++;

        this->articles->operator[](uuid) = {
                .uuid = uuid,
                .filename = filename,
                .author = author,
                .orgList = orgs,
                .title = document["title"].GetString(),
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
            pipeline::cleanStr(subs);
            if (this->stopWords.stopWords.find(subs) == this->stopWords.stopWords.end()) {
                Porter2Stemmer::stem(subs);
                if (subs.substr(0, 3) != "www") {
                    this->tbbMap->operator[](subs)[uuid]++;
                }
            }
        } while (iss);

        this->filesProcessed++;
    }
}


/**
 * Generates the index from a specific folder
 * @param folderName the folder to recursively process through
 * @return A dummy return value so that the function can be called with std::async
 */
void Processor::generateIndex(std::string folderName) {
    this->filesProcessed = 0;
    std::cout << termcolor::red << std::endl << pipeline::getCenteredText("Generating index...", 80) << std::endl;
    this->fillQueue(folderName);
    std::string fileDisplay = "Total files: " + std::to_string(this->totalFiles);

    std::cout << termcolor::green << pipeline::getCenteredText(fileDisplay, 80) << std::endl;
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
}

double Processor::getProgress() {
    return (double) this->filesProcessed.load() / (double) this->totalFiles;
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
    this->fileQueueMutex = new std::mutex();
    this->tbbMap = new tbb::concurrent_unordered_map<std::string, tbb::concurrent_unordered_map<std::string, int>>();

    this->totalFiles = 0;
    this->filesProcessed = 0;
    this->totalOrgs = 0;
    this->totalPeople = 0;
}


#include <cmath>
#include <future>

std::string Processor::convertToTree() {
    this->wordTreeMutex->lock();
    this->wordTree->clear();
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
    }
    this->wordTreeMutex->unlock();
    return "Conversion complete";
}

void Processor::printProcessorStats() const {
    std::cout << std::endl;

    std::cout << termcolor::bright_blue << "articles compiled\t" << termcolor::white << this->articles->size()
              << std::endl << std::endl;

    std::cout << termcolor::bright_blue << "organizations compiled\t" << termcolor::white << totalOrgs << std::endl
              << std::endl;

    std::cout << termcolor::bright_blue << "people compiled\t\t" << termcolor::white << totalPeople << std::endl
              << std::endl;
}

void Processor::cacheAvlTree() {
    if (this->wordTree != nullptr)
        this->wordTree->archive_tree("../tree-cache.txt");
}

void Processor::avlCacheBuildingBackbone() {
    if (this->wordTree != nullptr) {
        this->wordTree->clear();
        this->wordTree->load_from_archive("../tree-cache.txt");
    }
}

void Processor::cacheArticles() {
    if (this->articles == nullptr)
        return;

    std::ofstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::cacheArticles()\" | Could not open file ../article-cache.txt");

    cereal::JSONOutputArchive artArchive(artFile);
    artFile << this->articles->size() << std::endl;
    for (auto &it: *this->articles) {
        artArchive(it.first, it.second);
    }
}

void Processor::buildArticlesFromCache() {
    if (this->articles == nullptr)
        return;

    std::ifstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::buildArticlesFromCache()\" | Could not open file ../article-cache.txt");

    this->articles->clear();
    this->totalOrgs = 0;
    this->totalPeople = 0;

    int size;
    artFile >> size;
    if (size > 0) {
        cereal::JSONInputArchive artArchive(artFile);
        for (int i = 0; i < size; ++i) {
            std::string str;
            Article arti;
            artArchive(str, arti);
            this->articles->operator[](str) = arti;

            this->totalOrgs += arti.orgList.size();
            if (!arti.author.empty())
                this->totalPeople++;
        }
    }
}

void Processor::initiateAvlFromCache() {
    std::ifstream treeFile("../tree-cache.txt");
    if (!treeFile.is_open()) {
        throw std::invalid_argument("tree-cache file could not be opened");
    }
    treeFile >> this->totalWords;
    treeFile.close();
    this->wordTree->clear();

    ProgressBar<Processor> progressBar = {
            .invoker = this,
            .getProgress = [](Processor *p) -> double {
                return (double) p->wordTree->size() / (double) p->totalWords;
            },
    };
    progressBar.initiate<>(&Processor::avlCacheBuildingBackbone, this);
}

void Processor::printArticleTextFromFilename(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
    }
    rapidjson::Document document;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try {
        document.Parse(content.c_str());
    } catch (std::exception &e) {
        std::cout << termcolor::red << "Could not read file: " << filename << termcolor::reset << std::endl;
    }
    file.close();

    assert(document.HasMember("text"));
    // print out text field

    std::cout << std::endl << termcolor::bright_blue << "UUID: " << termcolor::white << document["uuid"].GetString()
              << std::endl << std::endl;

    std::cout << termcolor::bright_blue << "File Path: " << termcolor::white << filename << std::endl << std::endl;

    std::string author = document["author"].GetString();
    if (!author.empty()) {
        std::cout << termcolor::bright_blue << "Author: " << termcolor::white << author << std::endl << std::endl;
    } else {
        std::cout << termcolor::bright_blue << "No Documented Author" << termcolor::white << std::endl << std::endl;
    }

    const auto arr = document["entities"]["organizations"].GetArray();
    if (!arr.Empty()) {
        std::cout << termcolor::bright_blue << "Organizations: " << termcolor::white << std::endl;
        std::vector<std::string> orgs;
        for (const auto &org: arr) {
            std::cout << '\t' << org["name"].GetString() << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << termcolor::bright_blue << "Content: " << termcolor::white << std::endl;
    std::cout << document["text"].GetString() << std::endl << std::endl;
}