//
// Created by misc1 on 4/19/2022.
//

#ifndef INC_22S_FINAL_PROJ_AVL_TREE_IO_H
#define INC_22S_FINAL_PROJ_AVL_TREE_IO_H

#include "avl_tree.h"
#include <sstream>
#include <iostream>

//T = key data type, U = value data type, D = data type of elements in list/vector U
template<class T, class U, class D> //use this class if you want an avl tree that can read from or print to fstreams
class avl_tree_io_list : public avl_tree<T, U> {
public:
    void build_tree_from_txt(const std::string &filePath, void (*push_to)(U &, const D &));

    void print_inOrder() { print_nodes_inOrder(avl_tree<T, U>::root); }

    void print_levelOrder();

    void print_levelOrder(const std::string &filePath);

private:
    void print_nodes_inOrder(binary_node<T, U> *&node);

    void print_console_current_level(binary_node<T, U> *&node, int level);

    void print_stream_current_level(std::ofstream &outStr, binary_node<T, U> *&node, int level);
};

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::build_tree_from_txt(const std::string &filePath, void (*push_to)(U &, const D &)) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
        throw std::invalid_argument(
                "Error in \"void avl_tree_io_list<T, U, D>::build_tree_from_txt(const std::string &filePath, void (*push_to)(U &, const D &))\" | Could not open " +
                filePath);

    avl_tree<T, U>::clear();

    while (inFile.good()) {
        T inKey;
        U inData;
        std::string row;
        inFile >> inKey;
        getline(inFile, row);
        std::stringstream rowStream(row);
        D element;
        while (rowStream.good()) {
            rowStream >> element;
            push_to(inData, element);
        }
        avl_tree<T, U>::insert_overwriting(inKey, inData);
    }
}

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::print_nodes_inOrder(binary_node<T, U> *&node) {
    if (node != nullptr) {
        print_nodes_inOrder(node->left);
        std::cout << node->key;
        for (const auto &it: node->data) {
            std::cout << "," << it;
        }
        std::cout << std::endl;
        print_nodes_inOrder(node->right);
    }
}

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::print_console_current_level(binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        std::cout << node->key;
        for (const auto &it: node->data) {
            std::cout << "," << it;
        }
        std::cout << std::endl;
    } else if (level > 1) {
        print_console_current_level(node->left, level - 1);
        print_console_current_level(node->right, level - 1);
    }
}

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::print_stream_current_level(std::ofstream &outStr, binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        outStr << node->key;
        for (const auto &it: node->data) {
            outStr << " " << it;
        }
        outStr << std::endl;
    } else if (level > 1) {
        print_stream_current_level(outStr, node->left, level - 1);
        print_stream_current_level(outStr, node->right, level - 1);
    }
}

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::print_levelOrder() {
    int height = avl_tree<T, U>::nodeCount;
    for (int i = 0; i < height; ++i) {
        print_console_current_level(avl_tree<T, U>::root, i);
    }
}

template<class T, class U, class D>
void avl_tree_io_list<T, U, D>::print_levelOrder(const std::string &filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open())
        throw std::invalid_argument(
                "Error in \"void avl_tree_io_list<T, U, D>::print_levelOrder(const std::string &filePath)\" | Could not open " +
                filePath);

    int height = avl_tree<T, U>::nodeCount;
    for (int i = 0; i < height; ++i) {
        print_stream_current_level(outFile, avl_tree<T, U>::root, i);
    }
    outFile.close();
}

template<class T, class U> //use this class if you want an avl tree that can read from or print to fstreams
class avl_tree_io : public avl_tree<T, U> {
public:
    void build_tree_from_txt(const std::string &);

    void print_inOrder() { print_nodes_inOrder(avl_tree<T, U>::root); }

    void print_levelOrder();

    void print_levelOrder(const std::string &filePath);

private:
    void print_nodes_inOrder(binary_node<T, U> *&node);

    void print_console_current_level(binary_node<T, U> *&node, int level);

    void print_stream_current_level(std::ofstream &outStr, binary_node<T, U> *&node, int level);
};

template<class T, class U>
void avl_tree_io<T, U>::build_tree_from_txt(const std::string &filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
        throw std::invalid_argument(
                "Error in \"void avl_tree<T, U>::build_tree_from_txt(const std::string &filePath)\" | Could not open " +
                filePath);

    avl_tree<T, U>::clear();

    while (inFile.good()) {
        T inKey;
        U inData;
        inFile >> inKey;
        inFile >> inData;
        avl_tree<T, U>::insert_overwriting(inKey, inData);
    }
    inFile.close();
}

template<class T, class U>
void avl_tree_io<T, U>::print_nodes_inOrder(binary_node<T, U> *&node) {
    if (node != nullptr) {
        print_nodes_inOrder(node->left);
        std::cout << node->key << std::endl;
        print_nodes_inOrder(node->right);
    }
}

template<class T, class U>
void avl_tree_io<T, U>::print_levelOrder() {
    int height = avl_tree<T, U>::nodeCount;
    for (int i = 0; i < height; ++i) {
        print_console_current_level(avl_tree<T, U>::root, i);
    }
}

template<class T, class U>
void avl_tree_io<T, U>::print_levelOrder(const std::string &filePath) {
    std::ofstream outFile;
    outFile.open("../avl_tree/buildCorpus.txt");
    if (!outFile.is_open())
        throw std::invalid_argument(
                "Error in \"void print_levelOrder(const std::string& filePath)\" | Could not open " + filePath);

    int height = avl_tree<T, U>::nodeCount;
    for (int i = 0; i < height; ++i) {
        print_stream_current_level(outFile, avl_tree<T, U>::root, i);
    }
    outFile << std::endl;
    outFile.close();
}

template<class T, class U>
void avl_tree_io<T, U>::print_console_current_level(binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        std::cout << node->key << "," << node->data << std::endl;
    } else if (level > 1) {
        print_console_current_level(node->left, level - 1);
        print_console_current_level(node->right, level - 1);
    }
}

template<class T, class U>
void avl_tree_io<T, U>::print_stream_current_level(std::ofstream &outStr, binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        outStr << node->key << " " << node->data << std::endl;
    } else if (level > 1) {
        print_stream_current_level(outStr, node->left, level - 1);
        print_stream_current_level(outStr, node->right, level - 1);
    }
}

#endif //INC_22S_FINAL_PROJ_AVL_TREE_IO_H
