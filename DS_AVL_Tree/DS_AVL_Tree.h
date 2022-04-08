//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_DS_AVL_TREE_H
#define INC_22S_FINAL_PROJ_DS_AVL_TREE_H

template<class T>
class DS_AVL_Tree{
public:
    bool contains(const T&);
    bool insert(const T&);
    bool clear_node_at(const T&);

private:
    struct binary_node{
        T nodeData;
        int height = -1;
        binary_node* parent = nullptr;
        binary_node* left = nullptr;
        binary_node* right = nullptr;
    };
    void LL_rotate(binary_node&);
    void RR_rotate(binary_node&);
    void LR_rotate(binary_node&);
    void RL_rotate(binary_node&);
    binary_node* find_slot_for(const T&);

    int maxHeight = 0;
    binary_node* root;
};

template<class T>
void DS_AVL_Tree<T>::LL_rotate(DS_AVL_Tree::binary_node &) {

}

#endif //INC_22S_FINAL_PROJ_DS_AVL_TREE_H
