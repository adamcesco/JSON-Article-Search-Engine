//
// Created by dshar on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_PROCESSOR_H
#define INC_22S_FINAL_PROJ_PROCESSOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <mutex>
#include <queue>
#include "hash_table/hash_table.h"
#include "./TableBundle.h"
#include "StopWords.h"
#include "./avl_tree/avl_tree.h"
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>

class Processor {

private:
    // Passed from SearchEngine
    int totalFiles = 0;
    std::atomic<int> filesProcessed;
    StopWords stopWords;
    tbb::concurrent_queue<std::string> fileQueue;

    tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<std::string>> *tbbMap = nullptr;

    TableBundle *tableBundle;

    avl_tree<std::string, tbb::concurrent_vector<std::string> *> *wordTree = nullptr;
    std::mutex *wordTreeMutex;
    std::atomic<int> wordsConverted;
    int totalWords = 0;

    void fillArticle(const Article &article);

    void fillOrganization(const std::vector<std::string> &organizations, const std::string &uuid);

    void fillAuthors(const std::string &authors, const std::string &uuid);

    void fillQueue(std::string folderName);

    void process();


    bool safeIsEmpty();

public:
    explicit Processor(TableBundle *tableBundle, avl_tree<std::string, tbb::concurrent_vector<std::string> *> *tree,
                       std::mutex *treeMut);

    ~Processor();


    std::string convertToTree();

    std::string generateIndex(std::string folderName);

    double getProgress();

    double getConversionProgress();
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
