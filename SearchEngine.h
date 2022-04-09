//
// Created by drewharris on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_SEARCHENGINE_H
#define INC_22S_FINAL_PROJ_SEARCHENGINE_H


#include <string>
#include <unordered_map>
#include <vector>
#include "Processor.h"
#include "hash_ordered_map/hash_ordered_map.h"

class SearchEngine {
private:
    std::string data_folder;

    // Key: author name,  value: vector of article ids
    hash_ordered_map<std::string, std::vector<std::string>> *authors = nullptr;
    Processor *processor = nullptr;

public:
    explicit SearchEngine(std::string data_folder);

    void generateIndex();

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
