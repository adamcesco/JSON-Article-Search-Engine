//
// Created by misc1 on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_HASHORDEREDMAP_H
#define INC_22S_FINAL_PROJ_HASHORDEREDMAP_H

template<class T, class U>
class HashOrderedMap{
public:
    HashOrderedMap();
    HashOrderedMap(const HashOrderedMap&);
    HashOrderedMap& operator=(const HashOrderedMap&);
    HashOrderedMap& clear();
    HashOrderedMap& erase_at(const T&);
    int size();
    bool is_empty();
    HashOrderedMap& emplace_pair(const T&, const U&);
    U& operator[](const T&);
    U read_at(const T&);
    ~HashOrderedMap();
private:
    U* data;
    int ele_count;
};

#endif //INC_22S_FINAL_PROJ_HASHORDEREDMAP_H
