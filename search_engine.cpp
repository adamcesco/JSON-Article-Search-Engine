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
    this->wordTree = new avl_tree<unsigned int, std::vector<std::string *> *>();
    this->processor = new Processor(this->wordTree);
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
    processor->generateIndex(data_folder);
}

unsigned int hasher(const std::string &str) {
    unsigned int hashed = 1;
    for (const char &cc: str) {
        if (std::isalpha(cc)) {
            hashed *= 16777619;
            hashed = hashed ^ (cc & 31);
        }
    }
    return hashed;
}

void SearchEngine::testFindWord(std::string word) {
    std::vector<std::vector<std::string *> *> results;
    Porter2Stemmer::stem(word);
    unsigned int hash = hasher(word);
    std::ifstream inFile("../data/hashed-inverse-stemmed.txt");
    while (inFile.good()) {
        unsigned int cell;
        inFile >> cell;
        std::string row;
        getline(inFile, row);

        if (cell != hash)
            continue;
        else {
            results.push_back(this->wordTree->get_at(cell));
            std::istringstream rowStream(row);
            while (rowStream) {
                rowStream >> cell;
                std::cout << word << ',' << hash << ',' << cell << std::endl;
                results.push_back(this->wordTree->get_at(cell));
            }
            break;
        }
    }

    std::cout << "Top-five articles containing the word " << word << ':' << std::endl;

    auto it = results.begin();
    do {
        std::cout << (*it)->size() << std::endl;
        ++it;
    } while (it != results.end());

//    std::string *prev;
//    int count = 0;
//    for (std::string *article: it) {
//        if (count == 5)
//            break;
//        if (prev != article) {
//            std::cout << *article << std::endl;
//            count++;
//        }
//        prev = article;
//    }
}