//
// Created by dshar on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_PROCESSOR_H
#define INC_22S_FINAL_PROJ_PROCESSOR_H

#include <atomic>
#include <queue>
#include "../utilities/Article.h"
#include "../utilities/StopWords.h"
#include "../avl_tree/avl_tree.h"
#include "../hash_table/hash_table.h"
#include "tbb/concurrent_hash_map.h"

class Processor {

private:
    // Passed from SearchEngine
    std::atomic<int> wordsProcessed;
    unsigned int totalWords;
    std::atomic<int> filesProcessed;
    unsigned int totalFiles;
    unsigned int totalOrgs;     //ADAM-> delete later | todo
    unsigned int totalPeople;   //ADAM-> delete later | todo
    StopWords stopWords;
    std::mutex *fileQueueMutex = nullptr;
    std::queue<std::string> fileQueue;
    tbb::concurrent_unordered_map<std::string, tbb::concurrent_unordered_map<std::string, int>> *tbbMap = nullptr;
    tbb::concurrent_unordered_map<std::string, Article> *articles = nullptr;
    avl_tree<std::string, std::vector<std::pair<std::string, double>>> *wordTree = nullptr;
    std::mutex *wordTreeMutex;
    hash_table<std::string, int> organizations;
    hash_table<std::string, int> people;

    void fillQueue(std::string folderName);

    void process();

public:
    explicit Processor(tbb::concurrent_unordered_map<std::string, Article> *pArticles,
                       avl_tree<std::string, std::vector<std::pair<std::string, double>>> *tree,
                       std::mutex *treeMut);

    ~Processor();

    void convertMapToTree();

    void generateIndex(std::string folderName);

    double fileParseProgress();

    void printProcessorStats() const;

    void cacheAvlTree();

    void cacheArticles();

    void buildArticlesFromCache();

    void buildAvlFromCache();

    static void printArticleTextFromFilePath(const std::string &filename);

    double buildingTreeProgress();

    double buildingArticlesProgress();
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
