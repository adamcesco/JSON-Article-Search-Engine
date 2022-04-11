//
// Created by dshar on 4/8/2022.
//

#ifndef INC_22S_FINAL_PROJ_UTILS_H
#define INC_22S_FINAL_PROJ_UTILS_H

#include <string>

std::string getCenteredText(std::string text, int width) {
    int textLength = text.length();
    int spaces = (width - textLength) / 2;
    std::string spacesStr = "";
    for (int i = 0; i < spaces; i++) {
        spacesStr += " ";
    }
    return spacesStr + text;
}

void toLower(std::string &str) {
    for (char &i: str) {
        i = tolower(i);
    }
}

void removePunctuation(std::string &str) {
    std::string clean;
    for (auto &it: str) {
        if (std::isalpha(it)) {
            clean += it;
        }
    }
    str = clean;
}

void cleanStr(std::string &str) {
    std::string clean;
    for (char &it: str) {
        if (std::isalpha(it)) {
            clean += tolower(it);
        }
    }
    str = clean;
}

#endif //INC_22S_FINAL_PROJ_UTILS_H
