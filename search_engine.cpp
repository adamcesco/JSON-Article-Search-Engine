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
//    this->wordTree->print_tree_inorder();
    Porter2Stemmer::stem(word);
    std::vector<std::string *> *result = this->wordTree->get_at(hasher(word));
    std::cout << "Top-five articles containing the word " << word << ':' << std::endl;

    std::string *prev;
    int count = 0;
    for (std::string *article: *result) {
        if (count == 5)
            break;
        if (prev != article)
            std::cout << *article << std::endl;
        prev = article;
        count++;
    }
}