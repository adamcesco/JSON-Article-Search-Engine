//
// Created by drewharris on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_SEARCHENGINE_H
#define INC_22S_FINAL_PROJ_SEARCHENGINE_H


#include <string>
#include <unordered_map>
#include <vector>
#include "Processor.h"

class SearchEngine {
private:
    std::string data_folder;

    // Key: author name,  value: vector of article ids
    std::unordered_map<std::string, std::vector<std::string>> *authors = nullptr;
    Processor *processor = nullptr;

public:
    explicit SearchEngine(std::string data_folder);

    ~SearchEngine();


};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
