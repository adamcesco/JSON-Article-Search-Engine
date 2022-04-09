//
// Created by drewharris on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_TABLEBUNDLE_H
#define INC_22S_FINAL_PROJ_TABLEBUNDLE_H

#include "./hash_ordered_map/hash_ordered_map.h"
#include <mutex>
#include <vector>

struct Article {
    std::string uuid;
    std::vector<std::string> orgList;
    std::string author;
    std::string filename;
};

struct TableBundle {
    // Authors to uuids
    hash_ordered_map<std::string, std::vector<std::string>> *authors;
    // Orgs to uuids
    hash_ordered_map<std::string, std::vector<std::string>> *orgs;
    hash_ordered_map<std::string, Article> *articles;
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
