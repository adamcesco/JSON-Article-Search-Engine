//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_AVL_TREE_H
#define INC_22S_FINAL_PROJ_AVL_TREE_H

#include <algorithm>

template<class T, class U>
struct binary_node{
    void emplace_left(const T& pFace, const U& value) { delete left; left = new binary_node<T, U>(); left->face = pFace; left->data = value; }
    void emplace_right(const T& pFace, const U& value) { delete right; right = new binary_node<T, U>(); right->face = pFace; right->data = value; }

    T face;
    U data;
    int maxHeight = 0;
    binary_node<T, U>* parent = nullptr;
    binary_node<T, U>* left = nullptr;
    binary_node<T, U>* right = nullptr;
};

template<class T, class U>
class avl_tree{
public:
    bool contains(const T&);
    avl_tree& insert(const T&, const U&);   //O(n lg n)
    bool clear_node_at(const T&);
    U& operator[](const T&);

private:
    binary_node<T, U>* LL_rotate(binary_node<T, U>*);
    binary_node<T, U>* RR_rotate(binary_node<T, U>*);
    binary_node<T, U>* LR_rotate(binary_node<T, U>*);
    binary_node<T, U>* RL_rotate(binary_node<T, U>*);
    void balance_alpha(binary_node<T, U>*);
    binary_node<T, U>* unbalanced_insert(const T&, const U&);           //O(lg n)
    int update_height_of_subtree(binary_node<T, U>*);                   //O(n)

    binary_node<T, U>* root = nullptr;
};

template<class T, class U>
avl_tree<T, U>& avl_tree<T, U>::insert(const T& face, const U& value) {
    binary_node<T, U>* curNode = unbalanced_insert(face, value);        //O(lg n)

    binary_node<T, U>* finalAlpha = curNode;
    while(finalAlpha->parent != nullptr && finalAlpha->parent->maxHeight < finalAlpha->maxHeight + 1){  //O(lg n)
        ++finalAlpha->parent->maxHeight;
        finalAlpha = finalAlpha->parent;
    }
    finalAlpha = finalAlpha->parent;

    curNode = curNode->parent;
    while(curNode != finalAlpha){    //O(lg n)
        balance_alpha(curNode);     //O(n)
        curNode = curNode->parent;
    }

    return *this;
}

template<class T, class U>
binary_node<T, U> *avl_tree<T, U>::unbalanced_insert(const T & passedFace, const U& passedValue) {
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
void avl_tree<T, U>::balance_alpha(binary_node<T, U>* node) {
    int balance = node->left->maxHeight - node->right->maxHeight;   //needs nullptr checks, nullptr = -1
    if(balance > 1){
        if(node->left->left->maxHeight - node->left->right->maxHeight > 0)  //logic is correct
            LL_rotate(node);
        else
            RL_rotate(node);
    }
    else if (balance < -1){
        if(node->right->left->maxHeight - node->right->right->maxHeight < 0)    //logic is correct
            RR_rotate(node);
        else
            LR_rotate(node);
    }
    update_height_of_subtree(root);
}

template<class T, class U>
bool avl_tree<T, U>::contains(const T& passedFace) {
    binary_node<T, U>* temp = root;
    while(temp != nullptr){
        if(passedFace == temp->face){
            return true;
        }
        else if(passedFace < temp->face) {
            temp = temp->left;
        }
        else {
            temp = temp->right;
        }
    }
    return false;
}

template<class T, class U>
int avl_tree<T, U>::update_height_of_subtree(binary_node<T, U>* node) {
    int height = 0;
    if(node != nullptr){
        int left_height = update_height_of_subtree(node->left);
        int right_height = update_height_of_subtree(node->right);
        node->maxHeight = height = std::max(left_height, right_height);
        height += 1;
    }
    return height;
}

template<class T, class U>
binary_node<T, U>* avl_tree<T, U>::LL_rotate(binary_node<T, U>* parent) {   //parent = x, pivot = y
    binary_node<T, U>* pivot;
    pivot = parent->left;

    parent->left = pivot->right;
    pivot->right = parent;
    return pivot;
}

template<class T, class U>
binary_node<T, U>* avl_tree<T, U>::RR_rotate(binary_node<T, U>* parent) {   //parent = x, pivot = y
    binary_node<T, U>* pivot;
    pivot = parent->right;

    parent->right = pivot->left;
    pivot->left = parent;
    return pivot;
}

template<class T, class U>
binary_node<T, U>* avl_tree<T, U>::LR_rotate(binary_node<T, U>* parent) {
    parent->right = LL_rotate(parent->right);
    update_height_of_subtree(root);
    RR_rotate(parent);
}

template<class T, class U>
binary_node<T, U>* avl_tree<T, U>::RL_rotate(binary_node<T, U>* parent) {   //parent = x, pivot = y
    parent->left = RR_rotate(parent->left);
    update_height_of_subtree(root);
    LL_rotate(parent);
}

#endif //INC_22S_FINAL_PROJ_AVL_TREE_H
