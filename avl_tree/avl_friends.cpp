//
// Created by misc1 on 4/30/2022.
//

#include <iostream>
#include "./avl_tree.h"
#include "../include/termcolor/termcolor.hpp"

void print_top_25(avl_tree<std::string, std::vector<std::pair<std::string, double>>> &tree) {
    std::vector<std::pair<std::string, int>> words;

    std::function<void(binary_node<std::string, std::vector<std::pair<std::string, double>>> *&)> find;
    find = [&words, &find](binary_node<std::string, std::vector<std::pair<std::string, double>>> *&node) {
        if (node != nullptr) {
            find(node->left);

            words.emplace_back(node->key, node->data.size());

            find(node->right);
        }
    };

    find(tree.root);

    std::sort(words.begin(), words.end(),
              [](const std::pair<std::string, int> &p1, const std::pair<std::string, int> &p2) {
                  return (p1.second > p2.second);
              });

    std::cout << termcolor::bright_blue << "Top 25 most popular words:" << termcolor::white << std::endl;
    for (int i = 0; i < 25; ++i) {
        std::cout << words[i].first << "\t\t" << words[i].second << " articles" << std::endl;
    }
}