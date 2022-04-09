//
// Created by drewharris on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_TABLEBUNDLE_H
#define INC_22S_FINAL_PROJ_TABLEBUNDLE_H

#include "./hash_table/hash_table.h"
#include <mutex>
#include <vector>

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
    hash_table<std::string, std::vector<std::string>> *authors;
    // Orgs to uuids
    hash_table<std::string, std::vector<std::string>> *orgs;
    hash_table<std::string, Article> *articles;
    std::mutex authorsMutex;
    std::mutex orgsMutex;
    std::mutex articlesMutex;

    ~TableBundle() {
        delete authors;
        delete orgs;
        delete articles;
    }
};


#endif //INC_22S_FINAL_PROJ_TABLEBUNDLE_H
