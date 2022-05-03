//
// Created by Adam Escobedo on 4/28/2022.
//

#include "Pipelines.h"

std::string pipeline::getCenteredText(const std::string &text, int width) {
    int textLength = text.length();
    int spaces = (width - textLength) / 2;
    std::string spacesStr = "";
    for (int i = 0; i < spaces; i++) {
        spacesStr += " ";
    }
    return spacesStr + text;
}

void pipeline::toLower(std::string &str) {
    for (char &i: str) {
        i = tolower(i);
    }
}

void pipeline::removePunctuation(std::string &str) {
    std::string clean;
    for (auto &it: str) {
        if (std::isalpha(it)) {
            clean += it;
        }
    }
    str = clean;
}

void pipeline::cleanStr(std::string &str) {
    std::string clean;
    for (char &it: str) {
        if (std::isalpha(it)) {
            clean += tolower(it);
        }
    }
    str = clean;
}

std::string pipeline::cleanPropnoun(const std::string &str) {
    std::string clean;
    for (const char &it: str) {
        if (std::isalpha(it)) {
            clean += tolower(it);
        } else if (it == '-' || it == ' ' || it == '_') {
            clean += '-';
        }
    }
    return clean;
}

unsigned int pipeline::custom_string_hash(const std::string &str) {
    unsigned int hashed = 1;
    for (const char &cc: str)
        if (std::isalpha(cc)) {
            hashed *= 16777619;
            hashed = hashed ^ (cc & 31);
        }
    return hashed;
}