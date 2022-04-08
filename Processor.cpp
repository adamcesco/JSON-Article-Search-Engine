//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"

Processor::Processor(std::unordered_map<std::string, std::vector<std::string>> *authors) {
    this->authors = authors;
}

void Processor::generateIndex(std::string folderName) {
    std::cout << "Generating index..." << std::endl;
    std::cout << this->filesProcessed.load() << " files processed" << std::endl;
}
