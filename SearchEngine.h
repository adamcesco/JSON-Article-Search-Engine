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
#include "avl_tree/avl_tree.h"

class SearchEngine {
private:
    std::string data_folder;

    Processor *processor = nullptr;
    TableBundle *tables = nullptr;
    avl_tree<std::string, std::vector<std::string>> *wordTree = nullptr;
    std::mutex *wordTreeMutex = nullptr;

public:
    /**
     * @param data_folder The folder containing the data files
     */
    explicit SearchEngine(std::string data_folder);

    void testFindWord(std::string word);

    /**
     * Populates the tables and the inverse index with the articles
     */
    void generateIndex();

    std::vector<std::string> speedSearchFor(const std::string &term);

    ~SearchEngine();
};


#endif //INC_22S_FINAL_PROJ_SEARCHENGINE_H
