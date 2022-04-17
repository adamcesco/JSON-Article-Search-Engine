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
#include <fstream>
#include "./include/porter2_stemmer/porter2_stemmer.h"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->tables = new TableBundle();
    this->tables->orgs = new tbb::concurrent_unordered_map<std::string, std::vector<std::string>>();
    this->tables->authors = new tbb::concurrent_unordered_map<std::string, std::vector<std::string>>();
    this->tables->articles = new tbb::concurrent_unordered_map<std::string, Article>();

    this->wordTree = new avl_tree<unsigned int, tbb::concurrent_vector<std::string *> *>();
    this->wordTreeMutex = new std::mutex();

    this->processor = new Processor(this->tables, this->wordTree,
                                    this->wordTreeMutex);
}

SearchEngine::~SearchEngine() {
//    delete this->processor;
//    delete this->tables;
//    delete this->wordTree;
//    delete this->wordTreeMutex;
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
    std::vector<tbb::concurrent_vector<std::string *> *> results;

    Porter2Stemmer::stem(word);
    unsigned int hash = hash_table<bool, bool>::custom_string_hash(word);
    std::ifstream inverseStemFile("../data/hashed-inverse-stemmed.txt");
    while (inverseStemFile.good()) {
        unsigned int cell;
        inverseStemFile >> cell;
        std::string row;
        getline(inverseStemFile, row);

        if (cell != hash)
            continue;
        else {
            results.push_back(this->wordTree->get_at(cell));
            std::istringstream rowStream(row);
            while (rowStream) {
                rowStream >> cell;
                try {
                    results.push_back(this->wordTree->get_at(cell));
                } catch (const std::invalid_argument &e) {
                    continue;
                }
            }
            break;
        }
    }
    inverseStemFile.close();

    std::cout << "Top-five articles containing the word " << word << ':' << std::endl;

    std::string *prev = nullptr;
    for (int i = 0; i < results.size() - 1; ++i) {
        std::cout << results[i]->size() << std::endl;
//        prev = nullptr;
//        for (std::string *article: *results[i]) {
//            if (prev != article) {
//                std::cout << *article << std::endl;
//            }
//            prev = article;
//        }
    }
}

std::vector<std::string> SearchEngine::speedSearchFor(const std::string &term) {
    // TODO
    return std::vector<std::string>();
}
