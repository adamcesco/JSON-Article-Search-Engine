//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include "SearchEngine.h"
#include <future>
#include "./include/termcolor/termcolor.hpp"
#include "./Article.h"
#include <iomanip>      // std::setprecision
#include "./include/porter2_stemmer/porter2_stemmer.h"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->articles = new tbb::concurrent_unordered_map<std::string, Article>();

    this->wordTree = new avl_tree<std::string, std::vector<std::pair<std::string, double>>>();
    this->wordTreeMutex = new std::mutex();

    this->processor = new Processor(this->articles, this->wordTree,
                                    this->wordTreeMutex);
}

SearchEngine::~SearchEngine() {
    delete this->processor;
    delete this->articles;
    delete this->wordTree;
    delete this->wordTreeMutex;
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
 * Loads the data from the data folder into the articles and AVL tree.
 */
void SearchEngine::generateIndex() {
    // Start clock
    auto start = std::chrono::high_resolution_clock::now();
    // Call generateIndex on the processor asynchronously so that we can show a progress bar
    std::future<std::string> fut = std::async(std::launch::async, &Processor::generateIndex, this->processor,
                                              this->data_folder);

    // Poll the progress of the processor's filename stack every 400 milliseconds
    while (fut.wait_for(std::chrono::milliseconds(40)) != std::future_status::ready) {
        double progress = this->processor->getProgress();
        if (progress > 0) {
            printProgressBar(progress);
        }
    }
    printProgressBar(1);
    std::cout << std::endl;


    this->processor->convertToTree();
    // End clock
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << termcolor::green << "Index generated successfully!" << termcolor::reset << std::endl;
    std::cout << "Time Taken: " << diff.count() << " Seconds." << std::endl;

}

void SearchEngine::testFindWord(std::string word) {
    Porter2Stemmer::stem(word);
    std::vector<std::pair<std::string, double>> result = this->wordTree->operator[](word);
    std::cout << "Found " << result.size() << " articles containing the word " << word << ":" << std::endl;
    for (auto &article: result) {
        std::cout << article.first << " | " << article.second << " | ";
        Article doc = this->articles->operator[](article.first);
        std::cout << doc.filename << std::endl;

    }

}

#include "include/cereal/archives/json.hpp"
#include "include/cereal/types/vector.hpp"
#include "include/cereal/types/string.hpp"
#include "include/cereal/types/utility.hpp"

void SearchEngine::cacheAvlTree() {
    if (this->wordTree != nullptr)
        this->wordTree->archive_tree("../tree-cache.txt");

    //cache articles
    if (this->articles == nullptr)
        return;

    std::ofstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::cache()\" | Could not open file ../article-cache.txt");

    cereal::JSONOutputArchive artArchive(artFile);
    artFile << this->articles->size() << std::endl;
    for (auto &it: *this->articles) {
        artArchive(it.first, it.second);
    }

}

void SearchEngine::buildFromCache() {
    if (this->wordTree != nullptr)
        this->wordTree->load_from_archive("../tree-cache.txt");

    //load cache of articles
    if (this->articles == nullptr)
        return;

    std::ifstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::buildFromCache()\" | Could not open file ../article-cache.txt");

    int size;
    artFile >> size;
    if (size > 0) {
        cereal::JSONInputArchive artArchive(artFile);
        for (int i = 0; i < size; ++i) {
            std::string str;
            Article arti;
            artArchive(str, arti);
            this->articles->operator[](str) = arti;
        }
    }

}

void SearchEngine::InitiateConsoleInterface() {
    while (true) {
        std::cout << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate avl tree by parsing JSON documents" << std::endl;
        std::cout << "2. populate avl tree from cache" << std::endl;
        std::cout << "3. manage avl tree cache" << std::endl;
        std::cout << "4. manage article cache" << std::endl;
        std::cout << "5. enter boolean search query" << std::endl;
        std::cout << "6. print search engine statistics" << std::endl;
        std::cout << "7. end program" << std::endl << std::endl;

        bool invalid;
        int intInput;
        do {
            std::cout << "22s-final-project-fair-game / console-interface / search-engine > ";
            std::string input;
            std::cin >> input;

            intInput = input[0] & 15;
            invalid = (input.length() != 1 || intInput > 7 || intInput < 1 || (intInput == 5 && this->is_empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);

        switch (intInput) {
            case 1 :
                this->generateIndex();
                break;

            case 2 :
                this->buildFromCache();
                break;

            case 3 :
                this->AvlCacheConsoleManager();
                break;

            case 4 :

                break;

            case 5 :
                //query here
                break;

            case 6 :
                this->ConsolePrintEngineState();
                break;

            case 7 :
                return;
                break;
        }
    }
}

int SearchEngine::ConsolePrintEngineState() {
    int avlSize = 0;
    std::cout << std::endl;
    std::cout << "Search Engine Statistics:" << std::endl;
    if (this->wordTree != nullptr) {
        std::cout << "avl tree size\t\t" << this->wordTree->size() << std::endl;
        avlSize = this->wordTree->size();
    } else {
        std::cout << "avl tree size\t\t0" << std::endl;
    }

    if (this->articles != nullptr) {
        std::cout << "articles compiled\t" << this->articles->size() << std::endl;
    } else {
        std::cout << "articles compiled\t0" << std::endl;
    }
    std::cout << std::endl;

    return avlSize;
}

void SearchEngine::AvlCacheConsoleManager() {
    while (true) {
        std::cout << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate avl-cache with current avl tree data" << std::endl;
        std::cout << "2. populate avl tree from cache" << std::endl;
        std::cout << "3. clear avl-cache" << std::endl;
        std::cout << "4. view cavl-ache statistics" << std::endl;
        std::cout << "5. exit to main menu" << std::endl << std::endl;

        bool invalid;
        int intInput;
        do {
            std::cout
                    << "22s-final-project-fair-game / console-interface / search-engine / avl-cache-manager > ";
            std::string input;
            std::cin >> input;

            intInput = input[0] & 15;
            invalid = (input.length() != 1 || intInput > 5 || intInput < 1 || (intInput == 1 && this->is_empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);

        switch (intInput) {
            case 1 :
                this->cacheAvlTree();
                break;

            case 2 :
                this->buildFromCache();
                break;

            case 3 :

                break;

            case 4 :

                break;

            case 5 :
                return;
                break;
        }
    }
}
