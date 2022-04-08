//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "SearchEngine.h"
#include "Processor.h"

SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;
    this->authors = new std::unordered_map<std::string, std::vector<std::string>>();
    this->processor = new Processor(this->authors);
}

SearchEngine::~SearchEngine() {
    delete this->processor;
    delete this->authors;
}

void SearchEngine::generateIndex() {
    this->processor->generateIndex(this->data_folder);
}
