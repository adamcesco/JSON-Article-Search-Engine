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
#include "./TableBundle.h"

class SearchEngine {
private:
    std::string data_folder;

    Processor *processor = nullptr;
    TableBundle *tables;

public:
    explicit SearchEngine(std::string data_folder);

    void generateIndex();

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
