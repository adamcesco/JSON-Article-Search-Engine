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

class Processor {

private:
    // Passed from SearchEngine
    std::unordered_map<std::string, std::vector<std::string>> *authors = nullptr;

    int totalFiles = 0;
    std::atomic<int> filesProcessed;

    std::mutex fileQueueMutex;
    std::queue<std::string> fileQueue;

    void fillQueue(std::string folderName);

public:
    explicit Processor(std::unordered_map<std::string, std::vector<std::string>> *authors);

    void generateIndex(std::string folderName);
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
