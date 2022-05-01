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
#include "tbb/concurrent_hash_map.h"

class Processor {

private:
    // Passed from SearchEngine
    unsigned int totalFiles;
    unsigned int totalOrgs;
    unsigned int totalPeople;
    std::atomic<int> filesProcessed;
    StopWords stopWords;
    std::mutex *fileQueueMutex = nullptr;
    std::queue<std::string> fileQueue;
    tbb::concurrent_unordered_map<std::string, tbb::concurrent_unordered_map<std::string, int>> *tbbMap = nullptr;
    tbb::concurrent_unordered_map<std::string, Article> *articles = nullptr;
    avl_tree<std::string, std::vector<std::pair<std::string, double>>> *wordTree = nullptr;
    std::mutex *wordTreeMutex;
    int totalWords = 0;

    void fillQueue(std::string folderName);

    void process();

    void avlCacheBuildingBackbone();

public:
    explicit Processor(tbb::concurrent_unordered_map<std::string, Article> *pArticles,
                       avl_tree<std::string, std::vector<std::pair<std::string, double>>> *tree,
                       std::mutex *treeMut);

    ~Processor();

    std::string convertToTree();

    void generateIndex(std::string folderName);

    double getProgress();

    void printProcessorStats() const;

    void cacheAvlTree();

    void cacheArticles();

    void buildArticlesFromCache();

    void initiateAvlFromCache();

    static void printArticleTextFromFilename(const std::string &filename);
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
