//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

Processor::Processor(std::unordered_map<std::string, std::vector<std::string>> *authors) {
    this->authors = authors;
}

void Processor::generateIndex(std::string folderName) {
    std::cout << "Generating index..." << std::endl;
    std::cout << this->filesProcessed.load() << " files processed" << std::endl;
    this->fillQueue(folderName);
    while (!this->fileQueue.empty()) {
        std::cout << fileQueue.front() << std::endl;
        fileQueue.pop();
    }
}

void Processor::fillQueue(std::string folderName) {

    // Only files not folders
    for (const fs::directory_entry &dir_entry:
            fs::recursive_directory_iterator(folderName)) {
        if (fs::is_regular_file(dir_entry)) {
            this->fileQueue.push(dir_entry.path().string());
        }
    }

}
