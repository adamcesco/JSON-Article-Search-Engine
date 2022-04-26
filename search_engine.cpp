//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include "SearchEngine.h"
#include <future>
#include "./include/termcolor/termcolor.hpp"
#include "./TableBundle.h"
#include <iomanip>      // std::setprecision
#include "./include/porter2_stemmer/porter2_stemmer.h"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->tables = new TableBundle();
    this->tables->orgs = new tbb::concurrent_unordered_map<std::string, std::vector<std::string>>();
    this->tables->authors = new tbb::concurrent_unordered_map<std::string, std::vector<std::string>>();
    this->tables->articles = new tbb::concurrent_unordered_map<std::string, Article>();

    this->wordTree = new avl_tree<std::string, std::vector<std::pair<std::string, double>>>();
    this->wordTreeMutex = new std::mutex();

    this->processor = new Processor(this->tables, this->wordTree,
                                    this->wordTreeMutex);
}

SearchEngine::~SearchEngine() {
    delete this->processor;
    delete this->tables;
    delete this->wordTree;
    delete this->wordTreeMutex;
}

void printProgressBar(double progress) {
    int barWidth = 70;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setprecision(3) << std::fixed << double(progress * 100.0) << " %\r";
    std::cout.flush();
}

/**
 * Loads the data from the data folder into the tables and AVL tree.
 */
void SearchEngine::generateIndex() {
    // Start clock
    auto start = std::chrono::high_resolution_clock::now();
    // Call generateIndex on the processor asynchronously so that we can show a progress bar
    std::future<std::string> fut = std::async(std::launch::async, &Processor::generateIndex, this->processor,
                                              this->data_folder);

    // Poll the progress of the processor's filename stack every 400 milliseconds
    while (fut.wait_for(std::chrono::milliseconds(40)) != std::future_status::ready) {
        double progress = this->processor->getProgress();
        if (progress > 0) {
            printProgressBar(progress);
        }
    }
    printProgressBar(1);
    std::cout << std::endl;


    this->processor->convertToTree();
    // End clock
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << termcolor::green << "Index generated successfully!" << termcolor::reset << std::endl;
    std::cout << "Time Taken: " << diff.count() << " Seconds." << std::endl;

}

void SearchEngine::testFindWord(std::string word) {
    Porter2Stemmer::stem(word);
    std::vector<std::pair<std::string, double>> result = this->wordTree->operator[](word);
    std::cout << "Found " << result.size() << " articles containing the word " << word << ":" << std::endl;
    for (auto &article: result) {
        std::cout << article.first << " | " << article.second << " | ";
        Article doc = this->tables->articles->operator[](article.first);
        std::cout << doc.filename << std::endl;

    }

}

#include "include/cereal/archives/json.hpp"
#include "include/cereal/archives/binary.hpp"
#include "include/cereal/types/vector.hpp"
#include "include/cereal/types/string.hpp"
#include "include/cereal/types/utility.hpp"

void SearchEngine::cache() {
    if (this->wordTree != nullptr)
        this->wordTree->archive_tree("../tree-cache.txt");

    //cache tables
    if (this->tables == nullptr)
        return;

    if (this->tables->authors != nullptr) {
        std::ofstream authorFile("../author-cache.txt", std::ios::binary);
        if (!authorFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::cache()\" | Could not open file ../author-cache.txt");

        cereal::JSONOutputArchive authorArchive(authorFile);
        authorFile << this->tables->authors->size() << std::endl;
        for (auto &it: *this->tables->authors) {
            authorArchive(it.first, it.second);
        }
    }

    if (this->tables->orgs != nullptr) {
        std::ofstream orgFile("../org-cache.txt", std::ios::binary);
        if (!orgFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::cache()\" | Could not open file ../org-cache.txt");

        cereal::JSONOutputArchive orgArchive(orgFile);
        orgFile << this->tables->orgs->size() << std::endl;
        for (auto &it: *this->tables->orgs) {
            orgArchive(it.first, it.second);
        }
    }

    if (this->tables->articles != nullptr) {
        std::ofstream artFile("../art-cache.txt", std::ios::binary);
        if (!artFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::cache()\" | Could not open file ../art-cache.txt");

        cereal::JSONOutputArchive artArchive(artFile);
        artFile << this->tables->articles->size() << std::endl;
        for (auto &it: *this->tables->articles) {
            artArchive(it.first, it.second);
        }
    }
}

void SearchEngine::buildFromCache() {
    if (this->wordTree != nullptr)
        this->wordTree->load_from_archive("../tree-cache.txt");

    //load cache of tables
    if (this->tables == nullptr)
        return;

    if (this->tables->authors != nullptr) {
        std::ifstream authorFile("../author-cache.txt", std::ios::binary);
        if (!authorFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::buildFromCache()\" | Could not open file ../author-cache.txt");

        int size;
        authorFile >> size;
        cereal::JSONInputArchive authorArchive(authorFile);
        if (size > 0) {
            for (int i = 0; i < size; ++i) {
                std::string str;
                std::vector<std::string> vect;
                authorArchive(str, vect);
                this->tables->authors->operator[](str) = vect;
            }
        }
    }

    if (this->tables->orgs != nullptr) {
        std::ifstream orgFile("../org-cache.txt", std::ios::binary);
        if (!orgFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::buildFromCache()\" | Could not open file ../org-cache.txt");

        int size;
        orgFile >> size;
        if (size > 0) {
            cereal::JSONInputArchive orgArchive(orgFile);
            for (int i = 0; i < size; ++i) {
                std::string str;
                std::vector<std::string> vect;
                orgArchive(str, vect);
                this->tables->orgs->operator[](str) = vect;
            }
        }
    }

    if (this->tables->articles != nullptr) {
        std::ifstream artFile("../art-cache.txt", std::ios::binary);
        if (!artFile.is_open())
            throw std::invalid_argument(
                    "Error in \"void SearchEngine::buildFromCache()\" | Could not open file ../art-cache.txt");

        int size;
        artFile >> size;
        if (size > 0) {
            cereal::JSONInputArchive artArchive(artFile);
            for (int i = 0; i < size; ++i) {
                std::string str;
                Article arti;
                artArchive(str, arti);
                this->tables->articles->operator[](str) = arti;
            }
        }
    }
}
