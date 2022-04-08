//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_DS_AVL_TREE_H
#define INC_22S_FINAL_PROJ_DS_AVL_TREE_H

#include <algorithm>

template<class T, class U>
struct binary_node{

    void emplace_left(const T& pface, const U& value) { delete left; left = new binary_node<T, U>(); left->face = pface; left->data = value; }
    void emplace_right(const T& pface, const U& value) { delete right; right = new binary_node<T, U>(); right->face = pface; right->data = value; }

    T face;
    U data;
    int maxHeight = 0;
    binary_node<T, U>* parent = nullptr;
    binary_node<T, U>* left = nullptr;
    binary_node<T, U>* right = nullptr;
};

template<class T, class U>
class DS_AVL_Tree{
public:
    bool contains(const T&);
    DS_AVL_Tree& insert(const T&, const U&);
    bool clear_node_at(const T&);
    U& operator[](const T&);

private:
    void LL_rotate(binary_node<T, U>*);
    void RR_rotate(binary_node<T, U>*);
    void LR_rotate(binary_node<T, U>*);
    void RL_rotate(binary_node<T, U>*);
    void balance_alpha(binary_node<T, U>*);
    binary_node<T, U>* unbalanced_insert(const T&, const U&);   //O(lg n)
    binary_node<T, U>* update_maxHeights_via(binary_node<T, U>*);     //O(lg n)

    int maxHeight = 0;
    binary_node<T, U>* root = nullptr;
};

template<class T, class U>
DS_AVL_Tree<T, U>& DS_AVL_Tree<T, U>::insert(const T& face, const U& value) {
    binary_node<T, U>* curNode = unbalanced_insert(face, value);
    binary_node<T, U>* balanceRoot = update_maxHeights_via(curNode);
    if(balanceRoot->parent != nullptr)
        balanceRoot = balanceRoot->parent;

    while(curNode->parent != nullptr && curNode != balanceRoot){
        balance_alpha(curNode);

        curNode = curNode->parent;
    }

    //after rotating: updated all node max heights

    return *this;
}

template<class T, class U>
binary_node<T, U> *DS_AVL_Tree<T, U>::unbalanced_insert(const T & passedFace, const U& passedValue) {
    binary_node<T, U>* temp = root;
    binary_node<T, U>* y = nullptr;
    int leftBranch = 0;

    while(temp != nullptr){
        y = temp;
        if(passedFace == temp->face){
            leftBranch = 0;
            break;
        }
        else if(passedFace < temp->face) {
            temp = temp->left;
            leftBranch = -1;
        }
        else {
            temp = temp->right;
            leftBranch = 1;
        }
    }

    if(leftBranch == 0){
        if(temp == nullptr) {
            temp = new binary_node<T, U>();
        }
        temp->face += passedFace;
        temp->data = passedValue;
        return temp;
    }
    else if(leftBranch == -1) {
        y->emplace_left(passedFace, passedValue);
        y->left->parent = y;
        return y->left;
    }
    else{
        y->emplace_right(passedFace, passedValue);
        y->right->parent = y;
        return y->right;
    }
}

template<class T, class U>
binary_node<T, U>* DS_AVL_Tree<T, U>::update_maxHeights_via(binary_node<T, U>* node) {
    while(node->parent != nullptr && node->parent->maxHeight < node->maxHeight + 1){
        ++node->parent->maxHeight;
        node = node->parent;
    }
    return node;
}

template<class T, class U>
void DS_AVL_Tree<T, U>::balance_alpha(binary_node<T, U>* node) {

}

#endif //INC_22S_FINAL_PROJ_DS_AVL_TREE_H
