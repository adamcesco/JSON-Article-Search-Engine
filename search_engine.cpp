//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include <future>
#include <cassert>
#include <iomanip>
#include "SearchEngine.h"
#include "./include/termcolor/termcolor.hpp"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->articles = new tbb::concurrent_unordered_map<std::string, Article>();

    this->wordTree = new avl_tree<std::string, std::vector<std::pair<std::string, double>>>();
    this->wordTreeMutex = new std::mutex();

    this->processor = new Processor(this->articles, this->wordTree,
                                    this->wordTreeMutex);
    this->query_builder = new QueryBuilder(this->articles, this->wordTree);
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
    system("clear");
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
    this->totalWords = this->wordTree->size();
}

#include "include/cereal/archives/json.hpp"
#include "include/cereal/types/vector.hpp"
#include "include/cereal/types/string.hpp"
#include "include/cereal/types/utility.hpp"

void SearchEngine::cacheAvlTree() {
    if (this->wordTree != nullptr)
        this->wordTree->archive_tree("../tree-cache.txt");
}

int SearchEngine::buildAvlTreeFromCache() {
    if (this->wordTree != nullptr) {
        this->wordTree->clear();
        this->wordTree->load_from_archive("../tree-cache.txt");
    }
    return 0;
}

void SearchEngine::InitiateConsoleInterface() {
    system("clear");
    while (true) {
        std::cout << termcolor::bright_green << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate engine data by parsing JSON documents" << std::endl;
        std::cout << "2. populate avl tree from cache" << std::endl;
        std::cout << "3. populate articles from cache" << std::endl;
        std::cout << "4. manage avl tree cache" << std::endl;
        std::cout << "5. manage article cache" << std::endl;
        std::cout << "6. enter boolean search query" << std::endl;
        std::cout << "7. print search engine statistics" << std::endl;
        std::cout << "8. clear engine data" << std::endl;
        std::cout << "9. end program" << std::endl;
        std::cout << termcolor::white << std::endl;

        bool invalid;
        int intInput;
        do {
            std::cout << termcolor::bright_blue
                      << "22s-final-project-fair-game / console-interface / " << termcolor::bright_green
                      << "search-engine > " << termcolor::white;
            std::string input;
            std::cin >> input;

            intInput = input[0] & 15;
            invalid = (input.length() != 1 || !std::isdigit(input[0]) || intInput > 9 || intInput < 1 ||
                       (intInput == 6 && this->is_empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);

        switch (intInput) {
            case 1 :
                system("clear");
                this->generateIndex();
                system("clear");
                break;

            case 2 : {
                system("clear");

                std::ifstream treeFile("../tree-cache.txt");
                if (!treeFile.is_open()) {
                    throw std::invalid_argument("tree-cache file could not be opened");
                }
                treeFile >> totalWords;
                treeFile.close();
                this->wordTree->clear();

                std::future<int> fut = std::async(std::launch::async, &SearchEngine::buildAvlTreeFromCache, this);

                // Poll the progress of the processor's filename stack every 400 milliseconds
                while (fut.wait_for(std::chrono::milliseconds(40)) != std::future_status::ready) {
                    double progress = this->avlCacheBuildingProgress();
                    if (progress > 0) {
                        printProgressBar(progress);
                    }
                }
                printProgressBar(1);
                std::cout << std::endl;
                system("clear");
                break;
            }

            case 3 :
                system("clear");
                this->buildArticlesFromCache();
                system("clear");
                break;

            case 4 :
                system("clear");
                this->AvlCacheConsoleManager();
                system("clear");
                break;

            case 5 :
                system("clear");
                this->ArticleCacheConsoleManager();
                system("clear");
                break;

            case 6 :
                if (this->articles == nullptr || this->articles->empty()) {
                    std::cout << termcolor::red << std::endl;
                    std::cout << "WARNING: article data is empty | do you want to continue?" << std::endl;
                    std::cout << "1. Yes" << std::endl;
                    std::cout << "2. No" << std::endl;
                    std::cout << termcolor::white << std::endl;
                    do {
                        std::cout << termcolor::bright_blue
                                  << "22s-final-project-fair-game / console-interface / " << termcolor::bright_green
                                  << "search-engine > " << termcolor::white;
                        std::string input;
                        std::cin >> input;

                        intInput = input[0] & 15;
                        invalid = (input.length() != 1 || !std::isdigit(input[0]) || intInput > 2 || intInput < 1);
                        if (invalid) {
                            std::cout << "incorrect input" << std::endl;
                        }
                    } while (invalid);
                    if (intInput == 2)
                        break;
                }

                //query here
                QueryInterface();
                break;

            case 7 :
                system("clear");
                this->ConsolePrintEngineStats();
                break;

            case 8 : {
                if (this->wordTree != nullptr)
                    this->wordTree->clear();

                if (this->articles != nullptr)
                    this->articles->clear();

                delete this->processor;
                this->processor = new Processor(this->articles, this->wordTree,
                                                this->wordTreeMutex);

                system("clear");
                std::cout << std::endl << "all engine data is cleared" << std::endl << std::endl;
                break;
            }

            case 9 :
                return;
                break;
        }
    }
}


int SearchEngine::ConsolePrintEngineStats() {
    int avlSize = 0;
    std::cout << std::endl;
    std::cout << "Search Engine Statistics:" << std::endl;

    if (this->processor != nullptr) {
        this->processor->printProcessorStats();
    } else {
        std::cout << "articles compiled\t0" << std::endl << std::endl;
        std::cout << "organizations compiled\t0" << std::endl << std::endl;
        std::cout << "people compiled\t\t0" << std::endl << std::endl;
    }

    if (this->wordTree != nullptr) {
        std::cout << "avl tree size\t\t" << this->wordTree->size() << std::endl;
        std::cout << std::endl;
        avlSize = this->wordTree->size();

        if (!this->wordTree->is_empty()) {
            std::cout << "Top 25 most popular words:" << std::endl;
            avl_tree<std::string, std::vector<std::pair<std::string, double>>>::print_top_25(*this->wordTree);
        }
    } else {
        std::cout << "avl tree size\t\t0" << std::endl;
    }

    return avlSize;
}

void SearchEngine::AvlCacheConsoleManager() {   //completed
    while (true) {
        std::cout << termcolor::bright_green << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate avl-cache with current avl tree data" << std::endl;
        std::cout << "2. populate avl tree from cache" << std::endl;
        std::cout << "3. clear avl-cache" << std::endl;
        std::cout << "4. view avl-cache statistics" << std::endl;
        std::cout << "5. exit to main menu" << std::endl;
        std::cout << termcolor::white << std::endl;

        bool invalid;
        int intInput;
        do {
            std::cout
                    << termcolor::bright_blue
                    << "22s-final-project-fair-game / console-interface / search-engine / " << termcolor::bright_green
                    << "avl-cache-manager > " << termcolor::white;
            std::string input;
            std::cin >> input;

            intInput = input[0] & 15;
            invalid = (input.length() != 1 || !std::isdigit(input[0]) || intInput > 5 || intInput < 1 ||
                       (intInput == 1 && this->wordTree->is_empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);

        switch (intInput) {
            case 1 : {
                this->cacheAvlTree();
                system("clear");
                std::cout << std::endl << this->wordTree->size() << " nodes placed into cache" << std::endl
                          << std::endl;
                break;
            }

            case 2 : {
                system("clear");

                std::ifstream treeFile("../tree-cache.txt");
                if (!treeFile.is_open()) {
                    throw std::invalid_argument("tree-cache file could not be opened");
                }
                treeFile >> totalWords;
                treeFile.close();
                this->wordTree->clear();

                std::future<int> fut = std::async(std::launch::async, &SearchEngine::buildAvlTreeFromCache, this);

                // Poll the progress of the processor's filename stack every 400 milliseconds
                while (fut.wait_for(std::chrono::milliseconds(40)) != std::future_status::ready) {
                    double progress = this->avlCacheBuildingProgress();
                    if (progress > 0) {
                        printProgressBar(progress);
                    }
                }
                printProgressBar(1);
                std::cout << std::endl;
                system("clear");
                break;
            }

            case 3 : {
                std::ofstream treeFile("../tree-cache.txt", std::ios::trunc);
                if (!treeFile.is_open()) {
                    std::cout << "tree-cache file could not be opened" << std::endl;
                    break;
                }
                treeFile << "0" << std::endl;
                treeFile.close();
                system("clear");
                std::cout << std::endl << "AVL-Tree Cache is cleared" << std::endl << std::endl;

                break;
            }

            case 4: {
                system("clear");
                std::cout << std::endl << "AVL-Tree Cache Statistics:" << std::endl;

                std::ifstream cacheFile("../tree-cache.txt");
                if (!cacheFile.is_open()) {
                    std::cout << "tree-cache file could not be opened" << std::endl;
                    break;
                }
                std::cout << "state\t\tfile is present" << std::endl;

                int cacheSize = 0;
                cacheFile >> cacheSize;
                std::cout << "cache size\t" << cacheSize << " nodes in archive" << std::endl;
                std::cout << std::endl;
                break;
            }

            case 5:
                return;
                break;
        }
    }
}

void SearchEngine::ArticleCacheConsoleManager() {   //completed
    while (true) {
        std::cout << termcolor::bright_green << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate article-cache with current article data" << std::endl;
        std::cout << "2. populate articles from cache" << std::endl;
        std::cout << "3. clear article-cache" << std::endl;
        std::cout << "4. view article-cache statistics" << std::endl;
        std::cout << "5. exit to main menu" << std::endl;
        std::cout << termcolor::white << std::endl;

        bool invalid;
        int intInput;
        do {
            std::cout
                    << termcolor::bright_blue
                    << "22s-final-project-fair-game / search-engine / " << termcolor::bright_green
                    << "article-cache-manager > " << termcolor::white;
            std::string input;
            std::cin >> input;

            intInput = input[0] & 15;
            invalid = (input.length() != 1 || !std::isdigit(input[0]) || intInput > 5 || intInput < 1 ||
                       (intInput == 1 && this->articles->empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);

        switch (intInput) {
            case 1 :
                this->cacheArticles();
                system("clear");
                std::cout << std::endl << this->articles->size() << " articles placed into cache" << std::endl
                          << std::endl;
                break;

            case 2 :
                this->buildArticlesFromCache();
                system("clear");
                break;

            case 3 : {
                std::ofstream artFile("../article-cache.txt", std::ios::trunc);
                if (!artFile.is_open()) {
                    std::cout << "article-cache file could not be opened" << std::endl;
                    break;
                }
                artFile << "0" << std::endl;
                artFile.close();
                system("clear");
                std::cout << std::endl << "Article Cache is cleared" << std::endl << std::endl;
                break;
            }

            case 4: {
                system("clear");
                std::cout << std::endl;
                std::cout << "Article Cache Statistics:" << std::endl;

                std::ifstream cacheFile("../article-cache.txt");
                if (!cacheFile.is_open()) {
                    std::cout << "state\t\tfile could not be opened" << std::endl;
                    break;
                }
                std::cout << "state\t\tfile is present" << std::endl;

                int cacheSize = 0;
                cacheFile >> cacheSize;
                std::cout << "cache size\t" << cacheSize << " articles in archive" << std::endl;
                std::cout << std::endl;
                break;
            }

            case 5:
                return;
                break;
        }
    }
}

void SearchEngine::cacheArticles() {
    if (this->articles == nullptr)
        return;

    std::ofstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::cacheArticles()\" | Could not open file ../article-cache.txt");

    cereal::JSONOutputArchive artArchive(artFile);
    artFile << this->articles->size() << std::endl;
    for (auto &it: *this->articles) {
        artArchive(it.first, it.second);
    }
}

double SearchEngine::avlCacheBuildingProgress() {
    return ((double) this->wordTree->size() / (double) this->totalWords);
}

void SearchEngine::buildArticlesFromCache() {
    if (this->articles == nullptr)
        return;

    std::ifstream artFile("../article-cache.txt", std::ios::binary);
    if (!artFile.is_open())
        throw std::invalid_argument(
                "Error in \"void SearchEngine::buildArticlesFromCache()\" | Could not open file ../article-cache.txt");

    this->articles->clear();

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

void SearchEngine::QueryInterface() {
    std::vector<Article> result;
    auto GetInput = [&result](int maxChoice) -> int {
        bool invalid;
        int intInput;
        do {
            std::cout << termcolor::bright_blue
                      << "22s-final-project-fair-game / search-engine / " << termcolor::bright_green
                      << "query-interface > " << termcolor::white;
            std::string input;
            std::cin >> input;

            char *p;
            intInput = std::strtol(input.c_str(), &p, 10);
            invalid = (*p != '\0' || intInput > maxChoice || intInput < 1 || (intInput == 2 && result.empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);
        return intInput;
    };

    //------------------------------------------------------

    system("clear");
    std::cout << termcolor::bright_green << std::endl;
    std::cout << "Enter query: " << termcolor::white;
    std::string query;
    std::cin.ignore();
    std::getline(std::cin, query);

    auto start = std::chrono::high_resolution_clock::now();
    this->query_builder->buildQuery(query);
    result = this->query_builder->executeQuery();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << std::endl << termcolor::yellow << "Found " << result.size() << " articles in " << diff.count()
              << " seconds" << std::endl << termcolor::white;

    do {
        if (!result.empty())
            std::cout << std::endl << termcolor::bright_blue << "Results: " << termcolor::white << std::endl;

        // show first 15 results with a number in front of each
        int i = 1;
        for (auto &it: result) {
            std::cout << i << ": " << it.title << std::endl;
            i++;
            if (i > 15)
                break;
        }

        std::cout << termcolor::bright_green << std::endl;
        std::cout << "enter a number" << std::endl;
        std::cout << "1. enter another query" << std::endl;
        std::cout << "2. view an article" << std::endl;
        std::cout << "3. exit" << std::endl << std::endl << termcolor::white;

        int choice = GetInput(3);

        switch (choice) {
            case 1: {
                system("clear");
                std::cout << termcolor::bright_green << std::endl;
                std::cout << "Enter query: " << termcolor::white;
                std::cin.ignore();
                std::getline(std::cin, query);
                start = std::chrono::high_resolution_clock::now();
                this->query_builder->buildQuery(query);
                result = this->query_builder->executeQuery();
                end = std::chrono::high_resolution_clock::now();
                diff = end - start;
                std::cout << std::endl << termcolor::yellow << "Found " << result.size() << " articles in "
                          << diff.count()
                          << " seconds" << std::endl << termcolor::white;


                if (!result.empty())
                    std::cout << std::endl << termcolor::bright_blue << "Results: " << termcolor::white << std::endl;

                // show first 15 results with a number in front of each
                int i = 1;
                for (auto &it: result) {
                    std::cout << i << ": " << it.title << std::endl;
                    i++;
                    if (i > 15)
                        break;
                }
                std::cout << termcolor::white;

                break;
            }
            case 2: {
                // Prompt for a number to view
                std::cout << std::endl << termcolor::bright_green << "Enter a number to view an article: " << std::endl
                          << std::endl;
                int num = GetInput(result.size());
                SearchEngine::printArticleTextFromFilename(result[num - 1].filename);
                break;
            }
            case 3: {
                system("clear");
                return;
            }
        }
    } while (true);
}

void SearchEngine::printArticleTextFromFilename(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
    }
    rapidjson::Document document;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try {
        document.Parse(content.c_str());
    } catch (std::exception &e) {
        std::cout << termcolor::red << "Could not read file: " << filename << termcolor::reset << std::endl;
    }
    file.close();

    assert(document.HasMember("text"));
    // print out text field
    std::cout << document["text"].GetString() << std::endl;
}
