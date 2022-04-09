//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_AVL_TREE_H
#define INC_22S_FINAL_PROJ_AVL_TREE_H

#include <algorithm>
#include <iostream>

template<class T, class U>
struct binary_node {
    void emplace_left(const T &pFace, const U &value) {
        delete left;
        left = new binary_node<T, U>();
        left->face = pFace;
        left->data = value;
    }

    void emplace_right(const T &pFace, const U &value) {
        delete right;
        right = new binary_node<T, U>();
        right->face = pFace;
        right->data = value;
    }

    ~binary_node() {
        delete left;
        delete right;
    }

    T face;
    U data;
    int maxHeight = 0;
    binary_node<T, U> *parent = nullptr;
    binary_node<T, U> *left = nullptr;
    binary_node<T, U> *right = nullptr;
};

template<class T, class U>
class avl_tree {
public:
    avl_tree() = default;

    avl_tree(const avl_tree &);

    avl_tree &operator=(const avl_tree &);

    bool contains(const T &);

    U &get_at(const T &);

    unsigned int size() { return nodeCount; }

    avl_tree &insert(const T &, const U &, void (*)(U &, const U &));   //O(n lg n)
    avl_tree &insert(const T &, const U &);

    avl_tree &insert_overwriting(const T &, const U &);

    bool clear_node_at(const T &);

    U &operator[](const T &);

    void print_tree_inorder() { print_inorder(root); }

    int node_height_difference(binary_node<T, U> *, binary_node<T, U> *);

    ~avl_tree();

private:
    enum INSERT_OPERATION {
        INSERTED, MASKED
    };

    enum DIRECTION {    //used to determine which directing super-parent stitches
        LEFT, RIGHT
    };

    binary_node<T, U> *LL_rotate(binary_node<T, U> *&, DIRECTION);

    binary_node<T, U> *RR_rotate(binary_node<T, U> *&, DIRECTION);

    void LR_rotate(binary_node<T, U> *&);

    void RL_rotate(binary_node<T, U> *&);

    void balance_alpha(binary_node<T, U> *&);

    binary_node<T, U> *unbalanced_insert(const T &, const U &, INSERT_OPERATION &, void (*)(U &, const U &));

    binary_node<T, U> *unbalanced_insert_appending(const T &, const U &, INSERT_OPERATION &);

    binary_node<T, U> *unbalanced_insert_overwriting(const T &, const U &, INSERT_OPERATION &);

    int update_height_of_subtree(binary_node<T, U> *);

    void print_inorder(binary_node<T, U> *&);

    binary_node<T, U> *root = nullptr;
    unsigned int nodeCount = 0;
};

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert(const T &face, const U &value, void (*append)(U &, const U &)) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert(face, value, operation, append);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *finalAlpha = curNode;
    while (finalAlpha->parent != nullptr && finalAlpha->parent->maxHeight < finalAlpha->maxHeight + 1) {  //O(lg n)
        ++finalAlpha->parent->maxHeight;
        finalAlpha = finalAlpha->parent;
    }
    finalAlpha = finalAlpha->parent;

    curNode = curNode->parent;
    while (curNode != finalAlpha) {    //O(lg n)
        balance_alpha(curNode);      //O(lg n)
        curNode = curNode->parent;
    }
    ++nodeCount;
    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert(const T &passedFace, const U &passedValue, INSERT_OPERATION &operation,
                                  void (*append)(U &, const U &)) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (passedFace == temp->face) {
            leftBranch = 0;
            break;
        } else if (passedFace < temp->face) {
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
            temp->data = passedValue;
        } else {
            append(temp->data, passedValue);
            operation = MASKED;
        }
        temp->face = passedFace;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(passedFace, passedValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(passedFace, passedValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
void avl_tree<T, U>::balance_alpha(binary_node<T, U> *&node) {
    int balance = node_height_difference(node->left, node->right);   //needs nullptr checks, nullptr = -1
    if (balance > 1) {
        DIRECTION nodeDir = LEFT;
        if (node->parent != nullptr) {
            if (node->parent->left == node)
                nodeDir = LEFT;
            else if (node->parent->right == node)
                nodeDir = RIGHT;
        }

        if (node_height_difference(node->left->left, node->left->right) > 0)  //logic is correct
            update_height_of_subtree(LL_rotate(node, nodeDir));
        else
            RL_rotate(node);
    } else if (balance < -1) {
        DIRECTION nodeDir = LEFT;
        if (node->parent != nullptr) {
            if (node->parent->left == node)
                nodeDir = LEFT;
            else if (node->parent->right == node)
                nodeDir = RIGHT;
        }

        if (node_height_difference(node->right->left, node->right->right) < 0)    //logic is correct
            update_height_of_subtree(RR_rotate(node, nodeDir));
        else
            LR_rotate(node);
    }
}

template<class T, class U>
bool avl_tree<T, U>::contains(const T &passedFace) {
    binary_node<T, U> *temp = root;
    while (temp != nullptr) {
        if (passedFace == temp->face) {
            return true;
        } else if (passedFace < temp->face) {
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
binary_node<T, U> *avl_tree<T, U>::LL_rotate(binary_node<T, U> *&parent, DIRECTION stitch) {   //parent = x, pivot = y
    binary_node<T, U> *pivot;
    binary_node<T, U> *pivotParent = parent->parent;
    pivot = parent->left;

    parent->left = pivot->right;
    if (parent->left != nullptr)
        parent->left->parent = parent;
    pivot->right = parent;
    parent->parent = pivot;


    if (parent == root)
        root = pivot;
    if (pivotParent != nullptr) {
        if (stitch == LEFT)
            pivotParent->left = pivot;
        else
            pivotParent->right = pivot;
    }

    pivot->parent = pivotParent;

    return pivot;       //highest node that needs changing is "pivot"
}

template<class T, class U>
binary_node<T, U> *avl_tree<T, U>::RR_rotate(binary_node<T, U> *&parent, DIRECTION stitch) {   //parent = x, pivot = y
    binary_node<T, U> *pivot;
    binary_node<T, U> *pivotParent = parent->parent;
    pivot = parent->right;

    parent->right = pivot->left;
    if (parent->right != nullptr)
        parent->right->parent = parent;
    pivot->left = parent;
    parent->parent = pivot;


    if (parent == root)
        root = pivot;
    if (pivotParent != nullptr) {
        if (stitch == LEFT)
            pivotParent->left = pivot;
        else
            pivotParent->right = pivot;
    }

    pivot->parent = pivotParent;

    return pivot;       //highest node that needs changing is "pivot"
}

template<class T, class U>
void avl_tree<T, U>::LR_rotate(binary_node<T, U> *&parent) {
    parent->right = LL_rotate(parent->right, RIGHT);

    DIRECTION nodeDir = LEFT;
    if (parent->parent != nullptr) {
        if (parent->parent->left == parent)
            nodeDir = LEFT;
        else
            nodeDir = RIGHT;
    }
    update_height_of_subtree(RR_rotate(parent, nodeDir));
}

template<class T, class U>
void avl_tree<T, U>::RL_rotate(binary_node<T, U> *&parent) {   //parent = x, pivot = y
    parent->left = RR_rotate(parent->left, LEFT);

    DIRECTION nodeDir = LEFT;
    if (parent->parent != nullptr) {
        if (parent->parent->left == parent)
            nodeDir = LEFT;
        else
            nodeDir = RIGHT;
    }
    update_height_of_subtree(LL_rotate(parent, nodeDir));
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
U &avl_tree<T, U>::get_at(const T &passedFace) {
    binary_node<T, U> *temp = root;
    while (temp != nullptr) {
        if (passedFace == temp->face) {
            return temp->data;
        } else if (passedFace < temp->face) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }

    throw std::invalid_argument(
            "Error in \"U &avl_tree<T, U>::get_at(const T&)\" | Passed value cannot be found within tree.");
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert(const T &face, const U &value) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert_appending(face, value, operation);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *finalAlpha = curNode;
    while (finalAlpha->parent != nullptr && finalAlpha->parent->maxHeight < finalAlpha->maxHeight + 1) {  //O(lg n)
        ++finalAlpha->parent->maxHeight;
        finalAlpha = finalAlpha->parent;
    }
    finalAlpha = finalAlpha->parent;

    curNode = curNode->parent;
    while (curNode != finalAlpha) {    //O(lg n)
        balance_alpha(curNode);      //O(lg n)
        curNode = curNode->parent;
    }
    ++nodeCount;
    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert_appending(const T &passedFace, const U &passedValue, INSERT_OPERATION &operation) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (passedFace == temp->face) {
            leftBranch = 0;
            break;
        } else if (passedFace < temp->face) {
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
            temp->data = passedValue;
        } else {
            temp->data += passedValue;
            operation = MASKED;
        }
        temp->face = passedFace;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(passedFace, passedValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(passedFace, passedValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
avl_tree<T, U> &avl_tree<T, U>::insert_overwriting(const T &face, const U &value) {
    INSERT_OPERATION operation = INSERTED;
    binary_node<T, U> *curNode = unbalanced_insert_overwriting(face, value, operation);        //O(lg n)
    if (operation == MASKED)
        return *this;

    binary_node<T, U> *finalAlpha = curNode;
    while (finalAlpha->parent != nullptr && finalAlpha->parent->maxHeight < finalAlpha->maxHeight + 1) {  //O(lg n)
        ++finalAlpha->parent->maxHeight;
        finalAlpha = finalAlpha->parent;
    }
    finalAlpha = finalAlpha->parent;

    curNode = curNode->parent;
    while (curNode != finalAlpha) {    //O(lg n)
        balance_alpha(curNode);      //O(lg n)
        curNode = curNode->parent;
    }
    ++nodeCount;
    return *this;
}

template<class T, class U>
binary_node<T, U> *
avl_tree<T, U>::unbalanced_insert_overwriting(const T &passedFace, const U &passedValue, INSERT_OPERATION &operation) {
    binary_node<T, U> *temp = root;
    binary_node<T, U> *y = nullptr;
    int leftBranch = 0;

    while (temp != nullptr) {
        y = temp;
        if (passedFace == temp->face) {
            leftBranch = 0;
            break;
        } else if (passedFace < temp->face) {
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
            temp->data = passedValue;
        } else {
            temp->data = passedValue;
            operation = MASKED;
        }
        temp->face = passedFace;

        return temp;
    } else if (leftBranch == -1) {
        y->emplace_left(passedFace, passedValue);
        y->left->parent = y;
        return y->left;
    } else {
        y->emplace_right(passedFace, passedValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
void avl_tree<T, U>::print_inorder(binary_node<T, U> *&node) {
    if (node != nullptr) {
        print_inorder(node->left);
        std::cout << node->face << std::endl;
        print_inorder(node->right);
    }
}

#endif //INC_22S_FINAL_PROJ_AVL_TREE_H
