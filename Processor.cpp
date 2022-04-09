//
// Created by dshar on 4/7/2022.
//

#include <iostream>
#include "Processor.h"
#include <experimental/filesystem>
#include "./include/rapidjson/document.h"
#include "./include/termcolor/termcolor.hpp"
#include <thread>
#include <fstream>
#include "./TableBundle.h"

#include "./utils.h"

namespace fs = std::experimental::filesystem;

// TODO: ADD DESTRUCTOR

Processor::Processor(TableBundle *tb) {
    this->tableBundle = tb;
    this->fileQueueMutex = new std::mutex();
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

void Processor::process() {
    while (true) {
        this->fileQueueMutex->lock();
        // double check to make sure its not empty
        // TODO: DO THIS BETTER
        if (this->fileQueue.empty()) {
            this->fileQueueMutex->unlock();
            break;
        }
        std::string filename = this->fileQueue.front();
        this->fileQueue.pop();
        this->fileQueueMutex->unlock();
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Could not open file: " << filename << std::endl;
            continue;
        }
        rapidjson::Document document;
        try {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            document.Parse(content.c_str());
        } catch (std::exception &e) {
            std::cout << "Could not read file: " << filename << std::endl;
            continue;
        }
        file.close();

        assert(document.IsObject());
        assert(document.HasMember("title"));
        assert(document["title"].IsString());
        assert(document["text"].IsString());

        // loop through every word in text
        std::string text = document["text"].GetString();
        std::istringstream iss(text);

        // Iterate the istringstream
        // using do-while loop
        do {
            std::string subs;

            // Get the word from the istringstream
            iss >> subs;

            // Print the word fetched
            // from the istringstream
            if (subs[0] > 96 && subs[0] < 123) {
            }

        } while (iss);
        filesProcessed++;
    }
}


std::string Processor::generateIndex(std::string folderName) {


    std::cout << termcolor::red << std::endl << getCenteredText("Generating index...", 80) << std::endl;
    this->fillQueue(folderName);
    std::string fileDisplay = "Total files: " + std::to_string(this->totalFiles);

    std::cout << termcolor::green << getCenteredText(fileDisplay, 80) << std::endl;
    std::cout << termcolor::reset << std::endl;

    // Actually process the files
    std::thread t1(&Processor::process, this);
    std::thread t2(&Processor::process, this);
    std::thread t3(&Processor::process, this);
    std::thread t4(&Processor::process, this);
    std::thread t5(&Processor::process, this);
    std::thread t6(&Processor::process, this);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    return "Indexing complete";
}

double Processor::getProgress() {
    return (double) this->filesProcessed.load() / (double) this->totalFiles;
}

bool Processor::safeIsEmpty() {
    this->fileQueueMutex->lock();
    bool empty = this->fileQueue.empty();
    this->fileQueueMutex->unlock();
    return empty;
}

Processor::~Processor() {
    delete this->fileQueueMutex;
}
