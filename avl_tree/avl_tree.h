//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_AVL_TREE_H
#define INC_22S_FINAL_PROJ_AVL_TREE_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <queue>
#include <tbb/concurrent_vector.h>

template<class T, class U>
struct binary_node {
    void emplace_left(const T &pKey, const U &value) {
        delete left;
        left = new binary_node<T, U>();
        left->key = pKey;
        left->data = value;
    }

    void emplace_right(const T &pKey, const U &value) {
        delete right;
        right = new binary_node<T, U>();
        right->key = pKey;
        right->data = value;
    }

    void copy_subtree(const binary_node<T, U> *pNode) {
        if (pNode == nullptr)
            return;
        key = pNode->key;
        data = pNode->data;
        maxHeight = pNode->maxHeight;
        delete this->left;
        if (pNode->left != nullptr) {
            this->left = new binary_node<T, U>();
            this->left->copy_subtree(pNode->left);
            this->left->parent = this;
        }
        delete this->right;
        if (pNode->right != nullptr) {
            this->right = new binary_node<T, U>();
            this->right->copy_subtree(pNode->right);       //stitch parents
            this->right->parent = this;
        }
    }

    ~binary_node() {
        delete left;
        delete right;
    }

    T key;
    U data;
    int maxHeight = 0;
    binary_node<T, U> *parent = nullptr;
    binary_node<T, U> *left = nullptr;
    binary_node<T, U> *right = nullptr;
};

#include "../external/cereal/archives/json.hpp"
#include "../external/cereal/archives/binary.hpp"
#include "../external/cereal/types/vector.hpp"
#include "../external/cereal/types/string.hpp"
#include "../external/cereal/types/utility.hpp"

template<class T, class U>
class avl_tree {
public:

    avl_tree() = default;

    avl_tree(const avl_tree &);

    avl_tree &operator=(const avl_tree &);

    /**
     * @brief Returns true if the passed key is within the tree, otherwise returns false.
     * @param pKey This is the value that will be searched for within the avl_tree
     * @attention O(lg n)
     * @attention Uses "==", "<", and ">" operators.
     * */
    bool contains(const T &pKey);

    /**
     * @brief Returns the value bound to the passed key by reference.
     * @param pKey This is the value that will be searched for within the avl_tree, in order to get the value associated with it.
     * @attention O(lg n)
     * @attention Uses "==", "<", and ">" operators.
     * */
    U &operator[](const T &pKey);

    unsigned int size() { return nodeCount; }

    bool is_empty() { return nodeCount == 0; }

    /**
     * @brief Places the passed key/value pair into the avl_tree. If the passed key is already found within this avl_tree, then the passed append function will be used to append the passed value to the original value within that pre-existing node.
     * @param pKey This is the value that will be referenced for placement position within the avl_tree, and it will be the key that is paired/bound with the passed value parameter.
     * @param pValue This is the value that will be placed into the newly created avl_tree node.
     * @param append This is the function that will be used in the case where a node with a key equal to the passed key already exist. This is where the funciton will "append" the passed value to the original value within that pre-existing node.
     * @attention O(lg n)
     * @attention Uses "=", "==", "<", and ">" operators.
     * */
    avl_tree &
    insert(const T &pKey, const U &pValue, void (*append)(U &, const U &));

    /**
     * @brief Places the passed key/value pair into the avl_tree. If the passed key is already found within this avl_tree, then the "+=" operator will be called to append the passed value into the original value within that pre-existing node.
     * @param pKey This is the value that will be referenced for placement position within the avl_tree, and it will be the key that is paired/bound with the passed value parameter.
     * @param pValue This is the value that will be placed into the newly created avl_tree node.
     * @attention O(lg n)
     * @attention Uses "=", "+=", "==", "<", and ">" operators.
     * */
    avl_tree &insert(const T &pKey, const U &pValue);

    /**
     * @brief Places the passed key/value pair into the avl_tree. If the passed key is already found within this avl_tree, then the "=" operator will be called to overwrite the original value within that pre-existing node with the passed value.
     * @param pKey This is the value that will be referenced for placement position within the avl_tree, and it will be the key that is paired/bound with the passed value parameter.
     * @param pValue This is the value that will be placed into the newly created avl_tree node.
     * @attention O(lg n)
     * @attention Uses "=", "==", "<", and ">" operators.
     * */
    avl_tree &insert_overwriting(const T &pKey, const U &pValue);

    bool is_balanced();

    avl_tree &delete_node(const T &pKey);

    avl_tree &clear() {
        delete root;
        root = nullptr;
        nodeCount = 0;
        return *this;
    }

    void print_keys_in_order() { in_order_backbone(avl_tree<T, U>::root); }

    void print_keys_level_order();

    void archive_tree(std::string filename);

    void load_from_archive(std::string filename);

    friend void print_top_25(avl_tree<std::string, std::vector<std::pair<std::string, double>>> &wordTree);

    ~avl_tree();

protected:

    enum INSERT_OPERATION {
        INSERTED, MASKED
    };

    enum DIRECTION {    //used to determine which directing super-parent stitches
        LEFT, RIGHT
    };

    binary_node<T, U> *LL_rotate(binary_node<T, U> *&alpha, DIRECTION stitchDir);

    binary_node<T, U> *RR_rotate(binary_node<T, U> *&alpha, DIRECTION stitchDir);

    void LR_rotate(binary_node<T, U> *&alpha);

    void RL_rotate(binary_node<T, U> *&alpha);

    bool balance_alpha(binary_node<T, U> *&alpha);

    binary_node<T, U> *unbalanced_insert(const T &pKey, const U &pValue, INSERT_OPERATION &operation,
                                         void (*append)(U &, const U &));

    binary_node<T, U> *unbalanced_insert_appending(const T &pKey, const U &pValue, INSERT_OPERATION &operation);

    binary_node<T, U> *unbalanced_insert_overwriting(const T &pKey, const U &pValue, INSERT_OPERATION &operation);

    int node_height_difference(binary_node<T, U> *leftNode, binary_node<T, U> *rightNode);

    static int node_height(binary_node<T, U> *node);

    int update_height_of_subtree(binary_node<T, U> *node);

    bool check_balance(binary_node<T, U> *&node);

    binary_node<T, U> *find_place_of_from(binary_node<T, U> *&node, const T &pKey, DIRECTION &);

    void in_order_backbone(binary_node<T, U> *&node);

    void print_console_current_level(binary_node<T, U> *&node, int level);

    void archive_current_level(cereal::JSONOutputArchive &archive, binary_node<T, U> *&node, int level);

    unsigned int nodeCount = 0;
    binary_node<T, U> *root = nullptr;
};

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert(const T &pKey, const U &pValue,
                                       void (*append)(U &, const U &)) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert(pKey, pValue, operation, append);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *prev = nullptr;
    while (curNode != nullptr) {  //O(lg n)
        curNode->maxHeight = std::max(node_height(curNode->left), node_height(curNode->right)) + 1;
        if (balance_alpha(curNode)) {
            curNode = prev;
            continue;
        }

        prev = curNode;
        curNode = curNode->parent;
    }

    ++nodeCount;

    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert(const T &pKey, const U &pValue, INSERT_OPERATION &operation,
                                  void (*append)(U &, const U &)) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (pKey == temp->key) {
            leftBranch = 0;
            break;
        } else if (pKey < temp->key) {
            temp = temp->left;
            leftBranch = -1;
        } else {
            temp = temp->right;
            leftBranch = 1;
        }
    }

    operation = INSERTED;
    if (leftBranch == 0) {
        if (temp == nullptr) {
            temp = new binary_node<T, U>();
            root = temp;
            temp->data = pValue;
        } else {
            append(temp->data, pValue);
            operation = MASKED;
        }
        temp->key = pKey;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(pKey, pValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(pKey, pValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
bool avl_tree<T, U>::balance_alpha(binary_node<T, U> *&alpha) {
    int balance = node_height_difference(alpha->left, alpha->right);   //needs nullptr checks, nullptr = -1
    if (balance > 1) {
        DIRECTION nodeDir = LEFT;
        if (alpha->parent != nullptr) {
            if (alpha->parent->left == alpha)
                nodeDir = LEFT;
            else if (alpha->parent->right == alpha)
                nodeDir = RIGHT;
        }

        if (node_height_difference(alpha->left->left, alpha->left->right) > 0)  //logic is correct
            LL_rotate(alpha, nodeDir);
        else
            RL_rotate(alpha);
        return true;
    } else if (balance < -1) {
        DIRECTION nodeDir = LEFT;
        if (alpha->parent != nullptr) {
            if (alpha->parent->left == alpha)
                nodeDir = LEFT;
            else if (alpha->parent->right == alpha)
                nodeDir = RIGHT;
        }

        if (node_height_difference(alpha->right->left, alpha->right->right) < 0)    //logic is correct
            RR_rotate(alpha, nodeDir);
        else
            LR_rotate(alpha);
        return true;
    }
    return false;
}

template<class T, class U>
bool avl_tree<T, U>::contains(const T &pKey) {
    binary_node<T, U> *temp = root;
    while (temp != nullptr) {
        if (pKey == temp->key) {
            return true;
        } else if (pKey < temp->key) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }
    return false;
}

template<class T, class U>
int avl_tree<T, U>::update_height_of_subtree(binary_node<T, U> *node) {
    int height = 0;
    if (node != nullptr) {
        int left_height = update_height_of_subtree(node->left);
        int right_height = update_height_of_subtree(node->right);
        node->maxHeight = height = std::max(left_height, right_height);
        height += 1;
    }
    return height;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::LL_rotate(binary_node<T, U> *&alpha, DIRECTION stitchDir) {   //alpha = x, pivot = y
    binary_node<T, U> *pivot;
    binary_node<T, U> *alphaParent = alpha->parent;
    pivot = alpha->left;

    alpha->left = pivot->right;
    if (alpha->left != nullptr)
        alpha->left->parent = alpha;
    pivot->right = alpha;
    alpha->parent = pivot;


    if (alpha == root)
        root = pivot;
    if (alphaParent != nullptr) {
        if (stitchDir == LEFT)
            alphaParent->left = pivot;
        else
            alphaParent->right = pivot;
    }
    pivot->parent = alphaParent;

    alpha->maxHeight = std::max(node_height(alpha->left), node_height(alpha->right)) + 1;

    return pivot;       //highest node that needs changing is "pivot"
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::RR_rotate(binary_node<T, U> *&alpha, DIRECTION stitchDir) {   //alpha = x, pivot = y
    binary_node<T, U> *pivot;
    binary_node<T, U> *alphaParent = alpha->parent;
    pivot = alpha->right;

    alpha->right = pivot->left;
    if (alpha->right != nullptr)
        alpha->right->parent = alpha;
    pivot->left = alpha;
    alpha->parent = pivot;


    if (alpha == root)
        root = pivot;
    if (alphaParent != nullptr) {
        if (stitchDir == LEFT)
            alphaParent->left = pivot;
        else
            alphaParent->right = pivot;
    }
    pivot->parent = alphaParent;

    alpha->maxHeight = std::max(node_height(alpha->left), node_height(alpha->right)) + 1;

    return pivot;       //highest node that needs changing is "pivot"
}

template<class T, class U>
void avl_tree<T, U>::LR_rotate(binary_node<T, U> *&alpha) {
    alpha->right = LL_rotate(alpha->right, RIGHT);

    DIRECTION nodeDir = LEFT;
    if (alpha->parent != nullptr) {
        if (alpha->parent->left == alpha)
            nodeDir = LEFT;
        else
            nodeDir = RIGHT;
    }
    RR_rotate(alpha, nodeDir);
}

template<class T, class U>
void avl_tree<T, U>::RL_rotate(binary_node<T, U> *&alpha) {   //alpha = x, pivot = y
    alpha->left = RR_rotate(alpha->left, LEFT);

    DIRECTION nodeDir = LEFT;
    if (alpha->parent != nullptr) {
        if (alpha->parent->left == alpha)
            nodeDir = LEFT;
        else
            nodeDir = RIGHT;
    }
    LL_rotate(alpha, nodeDir);
}

template<class T, class U>
avl_tree<T, U>::~avl_tree() {
    delete root;
}

template<class T, class U>
int avl_tree<T, U>::node_height_difference(binary_node<T, U> *leftNode, binary_node<T, U> *rightNode) {
    int leftDiff = -1;
    int rightDiff = -1;
    if (leftNode != nullptr)
        leftDiff = leftNode->maxHeight;
    if (rightNode != nullptr)
        rightDiff = rightNode->maxHeight;

    return leftDiff - rightDiff;
}

template<class T, class U>
U &avl_tree<T, U>::operator[](const T &pKey) {
    binary_node<T, U> *temp = root;
    while (temp != nullptr) {
        if (pKey == temp->key) {
            return temp->data;
        } else if (pKey < temp->key) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }

    throw std::invalid_argument(
            "Error in \"U &avl_tree<T, U>::operator[](const T&)\" | Passed key cannot be found within tree.");
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert(const T &pKey, const U &pValue) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert_appending(pKey, pValue, operation);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *prev = nullptr;
    while (curNode != nullptr) {  //O(lg n)
        curNode->maxHeight = std::max(node_height(curNode->left), node_height(curNode->right)) + 1;
        if (balance_alpha(curNode)) {
            curNode = prev;
            continue;
        }

        prev = curNode;
        curNode = curNode->parent;
    }

    ++nodeCount;

    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert_appending(const T &pKey, const U &pValue, INSERT_OPERATION &operation) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (pKey == temp->key) {
            leftBranch = 0;
            break;
        } else if (pKey < temp->key) {
            temp = temp->left;
            leftBranch = -1;
        } else {
            temp = temp->right;
            leftBranch = 1;
        }
    }

    operation = INSERTED;
    if (leftBranch == 0) {
        if (temp == nullptr) {
            temp = new binary_node<T, U>();
            root = temp;
            temp->data = pValue;
        } else {
            temp->data += pValue;
            operation = MASKED;
        }
        temp->key = pKey;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(pKey, pValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(pKey, pValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert_overwriting(const T &pKey, const U &pValue) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert_overwriting(pKey, pValue, operation);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *prev = nullptr;
    while (curNode != nullptr) {  //O(lg n)
        curNode->maxHeight = std::max(node_height(curNode->left), node_height(curNode->right)) + 1;
        if (balance_alpha(curNode)) {
            curNode = prev;
            continue;
        }

        prev = curNode;
        curNode = curNode->parent;
    }

    ++nodeCount;

    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert_overwriting(const T &pKey, const U &pValue, INSERT_OPERATION &operation) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (pKey == temp->key) {
            leftBranch = 0;
            break;
        } else if (pKey < temp->key) {
            temp = temp->left;
            leftBranch = -1;
        } else {
            temp = temp->right;
            leftBranch = 1;
        }
    }

    operation = INSERTED;
    if (leftBranch == 0) {
        if (temp == nullptr) {
            temp = new binary_node<T, U>();
            root = temp;
            temp->data = pValue;
        } else {
            temp->data = pValue;
            operation = MASKED;
        }
        temp->key = pKey;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(pKey, pValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(pKey, pValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
inline int avl_tree<T, U>::node_height(binary_node<T, U> *node) {
    return (node == nullptr) ? -1 : node->maxHeight;
}

template<class T, class U>
bool avl_tree<T, U>::is_balanced() {
    return check_balance(root);
}

template<class T, class U>
bool avl_tree<T, U>::check_balance(binary_node<T, U> *&node) {
    if (node != nullptr) {
        if (check_balance(node->left) == false)
            return false;
        if (check_balance(node->right) == false)
            return false;

        int balance = node_height_difference(node->left, node->right);
        if (balance > 1)
            return false;
        else if (balance < -1)
            return false;

        int nodeHeight = std::max(node_height(node->left), node_height(node->right)) + 1;
        if (nodeHeight != node->maxHeight)
            return false;
    }
    return true;
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::delete_node(const T &pKey) {
    if (nodeCount == 0)
        throw std::invalid_argument(
                "Error in \"avl_tree<T, U> &avl_tree<T, U>::delete_node(const T &)\" | avl tree is empty.");

    DIRECTION stitchDir = LEFT;
    binary_node<T, U> *place = find_place_of_from(root, pKey, stitchDir);    //O(lg n)
    binary_node<T, U> *node = nullptr;

    if ((place->left == nullptr) || (place->right == nullptr)) {
        binary_node<T, U> *temp = place->left != nullptr ? place->left : place->right;

        // No child case
        if (temp == nullptr) {
            temp = place;
            node = place->parent;
            if (node != nullptr) {
                if (stitchDir == LEFT)
                    node->left = nullptr;
                else
                    node->right = nullptr;
            }
            if (place == root)
                root = node;
            place = nullptr;
        } else { // One child case
            place->key = temp->key;
            place->data = temp->data;
            place->left = temp->left;
            place->right = temp->right;
            place->maxHeight = std::max(node_height(place->left), node_height(place->right)) + 1;
            if (temp->left != nullptr)
                temp->left->parent = place;
            if (temp->right != nullptr)
                temp->right->parent = place;
            node = place;
        }

        nodeCount--;
        temp->left = nullptr;
        temp->right = nullptr;
        temp->parent = nullptr;
        delete temp;

        binary_node<T, U> *prev = node;
        while (node != nullptr) {
            node->maxHeight = std::max(node_height(node->left), node_height(node->right)) + 1;
            if (balance_alpha(node)) {
                node = prev;
                continue;
            }

            prev = node;
            node = node->parent;
        }

        return *this;
    }

    binary_node<T, U> *successor = place->right;
    while (successor->left != nullptr)
        successor = successor->left;

    T succKey = successor->key;
    U succData = successor->data;

    delete_node(succKey);

    place->key = succKey;
    place->data = succData;

    return *this;
}

template<class T, class U>
binary_node<T, U> *avl_tree<T, U>::find_place_of_from(binary_node<T, U> *&node, const T &pKey, DIRECTION &stitchDir) {
    binary_node<T, U> *temp = node;
    while (temp != nullptr) {
        if (pKey > temp->key) {
            temp = temp->right;
            stitchDir = RIGHT;
        } else if (pKey < temp->key) {
            temp = temp->left;
            stitchDir = LEFT;
        } else {
            return temp;
        }
    }
    throw std::invalid_argument(
            "Error in \"binary_node<T, U> *avl_tree<T, U>::find_place_of_from(const T &pKey, DIRECTION &stitchDir)\" | Passed value cannot be found within tree.");
}

template<class T, class U>
avl_tree<T, U>::avl_tree(const avl_tree &toCopy) {
    if (toCopy.root != nullptr) {
        root = new binary_node<T, U>();
        root->copy_subtree(toCopy.root);
    }
    nodeCount = toCopy.nodeCount;
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::operator=(const avl_tree &toAssign) {
    if (this == &toAssign)
        return *this;

    delete root;
    root = nullptr;

    if (toAssign.root != nullptr) {
        root = new binary_node<T, U>();
        root->copy_subtree(toAssign.root);
    }
    nodeCount = toAssign.nodeCount;
    return *this;
}

template<class T, class U>
void avl_tree<T, U>::in_order_backbone(binary_node<T, U> *&node) {
    if (node != nullptr) {
        in_order_backbone(node->left);
        std::cout << node->key << std::endl;
        in_order_backbone(node->right);
    }
}

template<class T, class U>
void avl_tree<T, U>::print_keys_level_order() {
    int height = avl_tree<T, U>::nodeCount;
    for (int i = 0; i < height; ++i) {
        print_console_current_level(avl_tree<T, U>::root, i);
    }
}

template<class T, class U>
void avl_tree<T, U>::print_console_current_level(binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        std::cout << node->key << std::endl;
    } else if (level > 1) {
        print_console_current_level(node->left, level - 1);
        print_console_current_level(node->right, level - 1);
    }
}

template<class T, class U>
void avl_tree<T, U>::archive_tree(std::string filename) {
    std::ofstream outFile;
    outFile.open(filename, std::ios::binary);
    if (!outFile.is_open())
        throw std::invalid_argument(
                "Error in \"void avl_tree_io<T, U>::archive_tree(std::string filename)\" | Could not open " + filename);

    outFile << nodeCount << std::endl;

    cereal::JSONOutputArchive ar(outFile);

    int height = nodeCount;
    for (int i = 0; i < height; ++i) {
        archive_current_level(ar, root, i);
    }
}

template<class T, class U>
void avl_tree<T, U>::archive_current_level(cereal::JSONOutputArchive &archive, binary_node<T, U> *&node, int level) {
    if (node == nullptr)
        return;
    if (level == 1) {
        archive(node->key, node->data);
    } else if (level > 1) {
        archive_current_level(archive, node->left, level - 1);
        archive_current_level(archive, node->right, level - 1);
    }
}

template<class T, class U>
void avl_tree<T, U>::load_from_archive(std::string filename) {
    std::ifstream inFile;
    inFile.open(filename, std::ios::binary);
    if (!inFile.is_open())
        throw std::invalid_argument(
                "Error in \"void avl_tree<T, U>::load_from_archive(std::string filename)\" | Could not open " +
                filename);

    int totalNodes;
    inFile >> totalNodes;

    if (totalNodes > 0) {
        cereal::JSONInputArchive ar(inFile);
        for (int i = 0; i < totalNodes; ++i) {
            T inKey;
            U inData;
            ar(inKey, inData);
            insert_overwriting(inKey, inData);
        }
    }
}

#endif //INC_22S_FINAL_PROJ_AVL_TREE_H
