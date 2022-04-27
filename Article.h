//
// Created by drewharris on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_ARTICLE_H
#define INC_22S_FINAL_PROJ_ARTICLE_H

#include "./hash_table/hash_table.h"
#include <mutex>
#include <vector>
#include <tbb/concurrent_unordered_map.h>
#include <fstream>
#include "include/cereal/archives/json.hpp"
#include "include/cereal/types/vector.hpp"
#include "include/cereal/types/string.hpp"
#include "include/cereal/types/utility.hpp"
#include "include/cereal/types/memory.hpp"

struct Article {
    std::string uuid;
    std::string filename;
    std::string author;
    std::vector<std::string> orgList;

    // Overloaded stream insertion operator
    friend std::ostream &operator<<(std::ostream &os, const Article &article) {
        os << "Article: " << article.uuid << " " << article.filename << " " << article.author << " ";
        for (auto &org: article.orgList) {
            os << org << " ";
        }
        return os;
    }

    template<class Archive>
    void serialize(Archive &ar) {
        ar(uuid, filename, author, orgList);
    }
};

#endif //INC_22S_FINAL_PROJ_ARTICLE_H
