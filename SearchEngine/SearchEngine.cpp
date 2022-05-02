//
// Created by drewharris on 4/7/2022.
//

#include <iostream>
#include <thread>
#include "SearchEngine.h"
#include "../external/termcolor/termcolor.hpp"
#include "../utilities/ProgressBar.h"
#include "../utilities/Pipelines.h"


SearchEngine::SearchEngine(std::string data_folder) {
    this->data_folder = data_folder;

    this->articles = new tbb::concurrent_unordered_map<std::string, Article>();

    this->wordTree = new avl_tree<std::string, std::vector<std::pair<std::string, double>>>();
    this->wordTreeMutex = new std::mutex();

    this->processor = new Processor(this->articles, this->wordTree, this->wordTreeMutex);
    this->query_builder = new QueryBuilder(this->articles, this->wordTree);
}

SearchEngine::~SearchEngine() {
    delete this->processor;
    delete this->articles;
    delete this->wordTree;
    delete this->wordTreeMutex;
    delete this->query_builder;
}

/**
 * Loads the data from the data folder into the articles and AVL tree.
 */
void SearchEngine::generateDataFromFiles() {
    // Start clock
    system("clear");
    std::cout << std::endl << termcolor::red << pipeline::getCenteredText("Generating Index and Articles...", 80)
              << termcolor::white << std::endl << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    ProgressBar<Processor> progressBar = {
            .invoker = this->processor,
            .getProgress = [](Processor *p) -> double {
                return p->fileParseProgress();
            },
    };
    progressBar.initiate<>(&Processor::generateIndex, this->processor,
                           this->data_folder);

    std::cout << std::endl << termcolor::red << pipeline::getCenteredText("Converting to AVL Tree...", 80)
              << termcolor::white << std::endl << std::endl;

    progressBar.getProgress = [](Processor *p) -> double {
        return p->buildingTreeProgress();
    };
    progressBar.initiate<>(&Processor::convertMapToTree, this->processor);

    // End clock
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    system("clear");
    std::cout << std::endl << termcolor::bright_blue << "Index generated successfully!" << termcolor::yellow
              << std::endl << std::endl;
    std::cout << "Time Taken: " << diff.count() << " Seconds." << termcolor::white << std::endl;
}

void SearchEngine::InitiateConsoleInterface() {
    auto GetInput = [this](int maxChoice) -> int {
        bool invalid;
        int intInput;
        do {
            std::cout << termcolor::bright_blue
                      << "22s-final-project-fair-game / console-interface / " << termcolor::bright_green
                      << "search-engine > " << termcolor::white;
            std::string input;
            std::cin >> input;

            char *p;
            intInput = std::strtol(input.c_str(), &p, 10);
            invalid = (*p != '\0' || intInput > maxChoice || intInput < 1 ||
                       (intInput == 6 && this->isIncomplete()) || (intInput == 4 && this->wordTree->is_empty()));
            if (invalid) {
                std::cout << "incorrect input" << std::endl;
            }
        } while (invalid);
        return intInput;
    };

    system("clear");
    while (true) {
        std::cout << termcolor::bright_green << std::endl;
        std::cout << "enter a number: " << std::endl;
        std::cout << "1. populate engine data by parsing JSON documents" << std::endl;
        std::cout << "2. populate avl tree from cache" << std::endl;
        std::cout << "3. populate articles from cache" << std::endl;
        std::cout << "4. delete a word from the avl tree" << std::endl;
        std::cout << "5. manage avl tree cache" << std::endl;
        std::cout << "6. manage article cache" << std::endl;
        std::cout << "7. enter boolean search query" << std::endl;
        std::cout << "8. print search engine statistics" << std::endl;
        std::cout << "9. clear engine data" << std::endl;
        std::cout << "10. end program" << std::endl;
        std::cout << termcolor::white << std::endl;

        int intInput = GetInput(10);

        switch (intInput) {
            case 1 :
                system("clear");
                this->generateDataFromFiles();
                break;

            case 2 :
                system("clear");
                this->generateAVLFromCache();
                break;

            case 3 :
                system("clear");
                this->generateArticlesFromCache();
                break;

            case 4 : {
                system("clear");
                std::cout << std::endl << termcolor::bright_green << "Enter a word: " << termcolor::white;
                std::string word;
                std::cin >> word;
                std::string oriWord = word;
                system("clear");
                pipeline::cleanStr(word);
                Porter2Stemmer::stem(word);
                try { this->wordTree->delete_node(word); }
                catch (const std::invalid_argument &e) {
                    std::cout << std::endl << termcolor::bright_blue << oriWord << " (" << word
                              << ") was not found in avl tree" << termcolor::white << std::endl << std::endl;
                    break;
                }
                std::cout << std::endl << termcolor::bright_blue << oriWord << " (" << word
                          << ") has been successfully deleted!" << termcolor::white << std::endl << std::endl;
                break;
            }

            case 5 :
                system("clear");
                this->AvlCacheConsoleManager();
                system("clear");
                break;

            case 6 :
                system("clear");
                this->ArticleCacheConsoleManager();
                system("clear");
                break;

            case 7 :
                QueryInterface();
                break;

            case 8 :
                system("clear");
                this->ConsolePrintEngineStats();
                break;

            case 9 : {
                system("clear");
                std::cout << std::endl << termcolor::red << pipeline::getCenteredText("clearing all engine data...", 80)
                          << termcolor::white
                          << std::endl;

                if (this->wordTree != nullptr)
                    this->wordTree->clear();

                if (this->articles != nullptr)
                    this->articles->clear();

                delete this->processor;
                this->processor = new Processor(this->articles, this->wordTree, this->wordTreeMutex);

                system("clear");
                std::cout << std::endl << termcolor::bright_blue << "all engine data is cleared" << termcolor::white
                          << std::endl << std::endl;
                break;
            }

            case 10 :
                return;
                break;
        }
    }
}


int SearchEngine::ConsolePrintEngineStats() {
    int avlSize = 0;
    std::cout << std::endl;
    std::cout << termcolor::bright_blue << "Search Engine Statistics:" << termcolor::white << std::endl;

    if (this->processor != nullptr) {
        this->processor->printProcessorStats();
    } else {
        std::cout << termcolor::bright_blue << "articles compiled\t" << termcolor::white << "0" << std::endl
                  << std::endl;

        std::cout << termcolor::bright_blue << "organizations compiled\t" << termcolor::white << "0" << std::endl
                  << std::endl;

        std::cout << termcolor::bright_blue << "authors compiled\t\t" << termcolor::white << "0" << std::endl
                  << std::endl;
    }

    if (this->wordTree != nullptr) {
        std::cout << termcolor::bright_blue << "avl tree size\t\t" << termcolor::white << this->wordTree->size()
                  << std::endl;
        std::cout << std::endl;
        avlSize = this->wordTree->size();

        if (!this->wordTree->is_empty()) {
            print_top_25(*this->wordTree);
        }
    } else {
        std::cout << termcolor::bright_blue << "avl tree size\t\t" << termcolor::white << "0" << std::endl;
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
                this->processor->cacheAvlTree();
                system("clear");
                std::cout << std::endl << this->wordTree->size() << " nodes placed into cache" << std::endl
                          << std::endl;
                break;
            }

            case 2 : {
                system("clear");
                this->generateAVLFromCache();
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
                this->processor->cacheArticles();
                system("clear");
                std::cout << std::endl << this->articles->size() << " articles placed into cache" << std::endl
                          << std::endl;
                break;

            case 2 :
                system("clear");
                this->generateArticlesFromCache();
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
            std::cout << std::endl << termcolor::bright_green << "Top-15 Results: " << termcolor::white << std::endl;

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
                break;
            }
            case 2: {
                // Prompt for a number to view
                std::cout << std::endl << termcolor::bright_green << "Enter a number to view an article: " << std::endl
                          << std::endl;
                int num = GetInput(result.size());
                system("clear");
                Processor::printArticleTextFromFilePath(result[num - 1].filename);
                break;
            }
            case 3: {
                system("clear");
                return;
            }
        }
    } while (true);
}

bool SearchEngine::isIncomplete() {
    return ((this->wordTree == nullptr || this->wordTree->is_empty()) ||
            (this->articles == nullptr || this->articles->empty()));
}

bool SearchEngine::isEmpty() {
    return ((this->wordTree == nullptr || this->wordTree->is_empty()) &&
            (this->articles == nullptr || this->articles->empty()));
}

void SearchEngine::generateAVLFromCache() {
    std::cout << std::endl << termcolor::red << pipeline::getCenteredText("Building AVL Tree...", 80)
              << termcolor::white << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    this->wordTree->clear();

    ProgressBar<Processor> progressBar = {
            .invoker = this->processor,
            .getProgress = [](Processor *p) -> double {
                return p->buildingTreeProgress();
            },
    };
    progressBar.initiate<>(&Processor::buildAvlFromCache, this->processor);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    system("clear");
    std::cout << std::endl << termcolor::bright_blue << "AVL Tree Built successfully!" << termcolor::yellow
              << std::endl << std::endl;
    std::cout << "Time Taken: " << diff.count() << " Seconds." << termcolor::white << std::endl;
}

void SearchEngine::generateArticlesFromCache() {
    std::cout << std::endl << termcolor::red << pipeline::getCenteredText("Building Articles...", 80)
              << termcolor::white << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    this->articles->clear();

    ProgressBar<Processor> progressBar = {
            .invoker = this->processor,
            .getProgress = [](Processor *p) -> double {
                return p->buildingArticlesProgress();
            },
    };
    progressBar.initiate<>(&Processor::buildArticlesFromCache, this->processor);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    system("clear");
    std::cout << std::endl << termcolor::bright_blue << "Articles Built successfully!" << termcolor::yellow
              << std::endl << std::endl;
    std::cout << "Time Taken: " << diff.count() << " Seconds." << termcolor::white << std::endl;
}
