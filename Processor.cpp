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

Processor::Processor(TableBundle *tb) {
    this->tableBundle = tb;
    this->fileQueueMutex = new std::mutex();
}

/**
 * Adds every article filename to a queue so that the multithreading can process them easily
 * @param folderName The folder name to search
 */
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
        if (this->fileQueue.empty()) {
            // Prevents a deadlock
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
            std::cout << termcolor::red << "Could not read file: " << filename << termcolor::reset << std::endl;
            continue;
        }
        file.close();

        assert(document.IsObject());
        assert(document.HasMember("title"));
        assert(document["title"].IsString());
        assert(document["text"].IsString());
        assert(document["uuid"].IsString());
        assert(document["author"].IsString());
        assert(document["entities"].IsObject());
        assert(document["entities"]["organizations"].IsArray());

        std::string uuid = document["uuid"].GetString();

        std::vector<std::string> orgs = {};
        // Check if array empty
        for (auto &org: document["entities"]["organizations"].GetArray()) {
            if (org.IsObject()) {
                if (org["name"].IsString()) {
                    orgs.push_back(org["name"].GetString());
                }
            }
        }

        Article art = {
                .uuid = document["uuid"].GetString(),
                .filename = filename,
                .orgList = orgs,
                .author = document["author"].GetString(),
        };

        this->tableBundle->articlesMutex.lock();
        this->tableBundle->articles->operator[](uuid) = art;
        this->tableBundle->articlesMutex.unlock();

        std::thread tableFillAuthorThread(&Processor::fillAuthors, this, uuid, document["author"].GetString());
        std::thread tableFillOrgsThread(&Processor::fillOrganization, this, orgs, uuid);
        std::thread tableFillArticlesThread(&Processor::fillArticle, this, art);

        tableFillAuthorThread.join();
        tableFillOrgsThread.join();
        tableFillArticlesThread.join();

        filesProcessed++;
    }
}


/**
 * Generates the index from a specific folder
 * @param folderName the folder to recursively process through
 * @return A dummy return value so that the function can be called with std::async
 */
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

void Processor::fillArticle(Article article) {
    this->tableBundle->articlesMutex.lock();
    this->tableBundle->articles->operator[](article.uuid) = article;
    this->tableBundle->articlesMutex.unlock();
}

void Processor::fillOrganization(std::vector<std::string> organizations, std::string uuid) {
    this->tableBundle->orgsMutex.lock();
    for (auto &org: organizations) {
        this->tableBundle->orgs->operator[](org).push_back(uuid);
    }
    this->tableBundle->orgsMutex.unlock();
}

void Processor::fillAuthors(std::string authors, std::string uuid) {
    this->tableBundle->authorsMutex.lock();
    this->tableBundle->authors->operator[](authors).push_back(uuid);
    this->tableBundle->authorsMutex.unlock();
}


