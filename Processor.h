//
// Created by dshar on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_PROCESSOR_H
#define INC_22S_FINAL_PROJ_PROCESSOR_H

#include <atomic>
#include <queue>
#include "./Article.h"
#include "StopWords.h"
#include "./avl_tree/avl_tree.h"
#include <tbb/concurrent_hash_map.h>

class Processor {

private:
    // Passed from SearchEngine
    unsigned int totalFiles = 0;
    unsigned int totalOrgs = 0;
    unsigned int totalPeople = 0;
    std::atomic<int> filesProcessed;
    StopWords stopWords;
    std::mutex *fileQueueMutex;
    std::queue<std::string> fileQueue;

    tbb::concurrent_unordered_map<std::string, tbb::concurrent_unordered_map<std::string, int>> *tbbMap = nullptr;

    tbb::concurrent_unordered_map<std::string, Article> *articles;

    avl_tree<std::string, std::vector<std::pair<std::string, double>>> *wordTree = nullptr;
    std::mutex *wordTreeMutex;
    std::atomic<int> wordsConverted;
    int totalWords = 0;

    void fillQueue(std::string folderName);

    void process();

    bool safeIsEmpty();

    void avlCacheBuildingBackbone();

public:
    explicit Processor(tbb::concurrent_unordered_map<std::string, Article> *pArticles,
                       avl_tree<std::string, std::vector<std::pair<std::string, double>>> *tree,
                       std::mutex *treeMut);

    ~Processor();


    std::string convertToTree();

    void generateIndex(std::string folderName);

    double getProgress();

    double getConversionProgress();

    void printProcessorStats() const;

    void cacheAvlTree();

    void cacheArticles();

    void buildArticlesFromCache();

    void initiateAvlCacheBuilding();
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
