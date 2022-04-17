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

class Processor {

private:
    // Passed from SearchEngine
    int totalFiles = 0;
    int filesProcessed;
    StopWords stopWords;
    std::vector<std::string> fileVect;

    /** @attention Do not delete/destruct this instance ("wordMap") until you are done with the avl_tree instance that contains pointers to the values of "wordMap" */
    std::unordered_map<unsigned int, std::vector<std::string *>> *wordMap = nullptr;

    TableBundle *tableBundle;

    /** @attention Note that the values of this tree are pointers, but these pointers do not need to be deleted, because their memory is not allocated on the heap. There memory is handled by "Processor::wordMap" */
    avl_tree<unsigned int, std::vector<std::string *> *> *wordTree = nullptr;
    int wordsConverted;
    int totalWords = 0;

    void fillArticle(const Article &article);

    void fillOrganization(const std::vector<std::string> &organizations, const std::string &uuid);

    void fillAuthors(const std::string &authors, const std::string &uuid);

    void fillQueue(const std::string &folderName);

    void process();

public:
    explicit Processor(avl_tree<unsigned int, std::vector<std::string *> *> *tree);

    ~Processor();

    void convertToTree();

    void generateIndex(const std::string &folderName);

    double getProgress();

    double getConversionProgress();
};


#endif //INC_22S_FINAL_PROJ_PROCESSOR_H
