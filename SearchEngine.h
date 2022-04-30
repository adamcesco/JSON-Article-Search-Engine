//
// Created by drewharris on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_SEARCHENGINE_H
#define INC_22S_FINAL_PROJ_SEARCHENGINE_H

#include "Processor.h"
#include "QueryBuilder.h"

class SearchEngine {
private:
    unsigned int totalFiles = 0;
    std::string data_folder;

    Processor *processor = nullptr;
    QueryBuilder *query_builder = nullptr;
    ArticleTable *articles = nullptr;
    WordTree *wordTree = nullptr;
    std::mutex *wordTreeMutex = nullptr;

    void AvlCacheConsoleManager();

    void ArticleCacheConsoleManager();

    void QueryInterface();

    void printArticleTextFromFilename(std::string filename);


public:
    /**
     * @param data_folder The folder containing the data files
     */
    explicit SearchEngine(std::string data_folder);

    void testQuery(std::string query);

    /**
     * Populates the articles and the inverse index with the articles
     */
    void generateIndex();

    void cacheAvlTree();

    void cacheArticles();

    double avlCacheBuildingProgress();

    int buildAvlTreeFromCache();

    void buildArticlesFromCache();

    int ConsolePrintEngineStats();

    void InitiateConsoleInterface();

    bool is_empty() {
        return (this->wordTree == nullptr || this->wordTree->is_empty());
    }

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
