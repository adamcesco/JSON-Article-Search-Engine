//
// Created by drewharris on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_SEARCHENGINE_H
#define INC_22S_FINAL_PROJ_SEARCHENGINE_H


#include <string>
#include <unordered_map>
#include <vector>
#include "Processor.h"
#include "hash_table/hash_table.h"
#include "./Article.h"
#include "avl_tree/avl_tree.h"

class SearchEngine {
private:
    std::string data_folder;

    Processor *processor = nullptr;
    tbb::concurrent_unordered_map<std::string, Article> *articles = nullptr;
    avl_tree<std::string, std::vector<std::pair<std::string, double>>> *wordTree = nullptr;
    std::mutex *wordTreeMutex = nullptr;

    void AvlCacheConsoleManager();

    void ArticleCacheConsoleManager();

public:
    /**
     * @param data_folder The folder containing the data files
     */
    explicit SearchEngine(std::string data_folder);

    void testFindWord(std::string word);

    /**
     * Populates the articles and the inverse index with the articles
     */
    void generateIndex();

    void cacheAvlTree();

    void cacheArticles();

    void buildAvlTreeFromCache();

    void buildArticlesFromCache();

    int ConsolePrintEngineState();

    void InitiateConsoleInterface();

    bool is_empty() {
        return (this->wordTree == nullptr || this->wordTree->is_empty());
    }

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
