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

#endif //INC_22S_FINAL_PROJ_UTILS_H