//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include "SearchEngine.h"
#include <future>
#include "./include/termcolor/termcolor.hpp"
#include "./TableBundle.h"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->tables = new TableBundle();
    this->tables->orgs = new hash_table<std::string, std::vector<std::string>>();
    this->tables->authors = new hash_table<std::string, std::vector<std::string>>();
    this->tables->articles = new hash_table<std::string, Article>();

    this->processor = new Processor(this->tables);
}

SearchEngine::~SearchEngine() {
    delete this->processor;
    delete this->tables;
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
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

/**
 * Loads the data from the data folder into the tables and AVL tree.
 */
void SearchEngine::generateIndex() {
    // Call generateIndex on the processor asynchronously so that we can show a progress bar
    std::future<std::string> fut = std::async(std::launch::async, &Processor::generateIndex, this->processor,
                                              this->data_folder);

    // Poll the progress of the processor's filename stack every 41 milliseconds
    while (fut.wait_for(std::chrono::milliseconds(41)) != std::future_status::ready) {
        double progress = this->processor->getProgress();
        if (progress > 0) {
            printProgressBar(progress);
        }
    }
    printProgressBar(1);
    std::cout << std::endl;
}
