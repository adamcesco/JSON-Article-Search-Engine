//
// Created by dshar on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_UTILS_H
#define INC_22S_FINAL_PROJ_UTILS_H

#include <string>
#include "./include/porter2_stemmer/porter2_stemmer.h"

std::string getCenteredText(const std::string &text, int width);

void toLower(std::string &str);

void removePunctuation(std::string &str);

void cleanStr(std::string &str);

std::string cleanPropnoun(const std::string &str);

unsigned int custom_string_hash(const std::string &str);

#endif //INC_22S_FINAL_PROJ_UTILS_H
