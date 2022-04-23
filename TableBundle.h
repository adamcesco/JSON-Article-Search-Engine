//
// Created by drewharris on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_TABLEBUNDLE_H
#define INC_22S_FINAL_PROJ_TABLEBUNDLE_H

#include "./hash_table/hash_table.h"
#include <mutex>
#include <vector>
#include <tbb/concurrent_unordered_map.h>

struct Article {
    std::string uuid;
    std::string filename;
    std::vector<std::string> orgList;
    std::string author;

    // Overloaded stream insertion operator
    friend std::ostream &operator<<(std::ostream &os, const Article &article) {
        os << "Article: " << article.uuid << " " << article.filename << " " << article.author << " ";
        for (auto &org: article.orgList) {
            os << org << " ";
        }
        return os;
    }
};

struct TableBundle {
    // Authors to uuids
    tbb::concurrent_unordered_map<std::string, std::vector<std::string>> *authors;
    // Orgs to uuids
    tbb::concurrent_unordered_map<std::string, std::vector<std::string>> *orgs;
    tbb::concurrent_unordered_map<std::string, Article> *articles;

    ~TableBundle() {
        delete authors;
        delete orgs;
        delete articles;
    }
};


#endif //INC_22S_FINAL_PROJ_TABLEBUNDLE_H
