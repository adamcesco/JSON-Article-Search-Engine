//
// Created by drewharris on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_SEARCHENGINE_H
#define INC_22S_FINAL_PROJ_SEARCHENGINE_H


#include <string>
#include <unordered_map>
#include <vector>
#include "Processor.h"
#include "hash_table/hash_table.h"
#include "./TableBundle.h"

class SearchEngine {
private:
    std::string data_folder;

    Processor *processor = nullptr;
    TableBundle *tables;

public:
    /**
     * @param data_folder The folder containing the data files
     */
    explicit SearchEngine(std::string data_folder);

    /**
     * Populates the tables and the inverse index with the articles
     */
    void generateIndex();

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
