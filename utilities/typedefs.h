//
// Created by Drew Harris on 4/27/2022.
//
#ifndef INC_22S_FINAL_PROJ_TYPEDEFS_H
#define INC_22S_FINAL_PROJ_TYPEDEFS_H

#include <string>
#include <vector>
#include "../avl_tree/avl_tree.h"
#include "tbb/concurrent_unordered_map.h"
#include "Article.h"

typedef avl_tree<std::string, std::vector<std::pair<std::string, double>>> WordTree;
typedef tbb::concurrent_unordered_map<std::string, Article> ArticleTable;
typedef std::pair<std::string, double> ScoredId;

#endif //INC_22S_FINAL_PROJ_TYPEDEFS_H
