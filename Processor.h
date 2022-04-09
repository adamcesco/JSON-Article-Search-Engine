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
#include "hash_ordered_map/hash_ordered_map.h"
#include "./TableBundle.h"

class Processor {

private:
    // Passed from SearchEngine
    int totalFiles = 0;
    std::atomic<int> filesProcessed;

    std::mutex *fileQueueMutex;
    std::queue<std::string> fileQueue;

    TableBundle *tableBundle;

    void fillQueue(std::string folderName);

    void process();

    bool safeIsEmpty();

public:
    explicit Processor(TableBundle *tableBundle);

    ~Processor();


    std::string generateIndex(std::string folderName);

    double getProgress();
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
