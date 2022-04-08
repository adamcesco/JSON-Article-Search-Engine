//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"
#include <experimental/filesystem>
#include <thread>

namespace fs = std::experimental::filesystem;

Processor::Processor(std::unordered_map<std::string, std::vector<std::string>> *authors) {
    this->authors = authors;
}

void Processor::fillQueue(std::string folderName) {
    // Only files not folders
    for (const fs::directory_entry &dir_entry:
            fs::recursive_directory_iterator(folderName)) {
        if (fs::is_regular_file(dir_entry)) {
            this->fileQueue.push(dir_entry.path().string());
            this->totalFiles++;
        }
    }

}

std::string Processor::generateIndex(std::string folderName) {
    std::cout << "Generating index..." << std::endl;
    this->fillQueue(folderName);
    while (!this->fileQueue.empty()) {
        std::cout << fileQueue.front() << std::endl;
        fileQueue.pop();
    }
    this->totalFiles = 300;
    std::cout << "Total files: " << this->totalFiles << std::endl;

    // pretend to process stuff
    for (int i = 0; i < this->totalFiles; i++) {
        this->filesProcessed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(13));
    }


    return "Indexing complete";
}

double Processor::getProgress() {
    return (double) this->filesProcessed / (double) this->totalFiles;
}
