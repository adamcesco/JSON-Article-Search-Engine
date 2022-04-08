//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include "SearchEngine.h"
#include <future>

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
    std::future<std::string> fut = std::async(std::launch::async, &Processor::generateIndex, this->processor,
                                              this->data_folder);
    while (fut.wait_for(std::chrono::milliseconds(41)) != std::future_status::ready) {
        double progress = this->processor->getProgress();
        if (progress > 0) {
            printf("\rAny text done\r%7.2f%%", processor->getProgress() * 100);
            fflush(stdout); // <- add this call
        }
    }
    printf("\rAny text done\r%7.2f%%", processor->getProgress() * 100);
    std::cout << std::endl;
}
