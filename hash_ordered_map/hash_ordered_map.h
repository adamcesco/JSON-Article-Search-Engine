//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_HASH_ORDERED_MAP_H
#define INC_22S_FINAL_PROJ_HASH_ORDERED_MAP_H

#include <functional>
#include <string>

template<class T, class U>
class hash_ordered_map{
public:
    hash_ordered_map();
    hash_ordered_map(const hash_ordered_map<T, U>&);                    //copy-constructor
    hash_ordered_map<T, U>& operator=(const hash_ordered_map<T, U>&);   //assignment operator overload
    hash_ordered_map<T, U>& clear() { delete[] data; data = new HashPair[20]; max_cap = 20; ele_count = 0; return *this; }    //resets all *this contents/data
    hash_ordered_map<T, U>& clear_value_at(const T& key);               //sets the value at "key" to nullptr
    int size() const { return ele_count; }
    bool is_empty() const { return ele_count == 0; }
    hash_ordered_map<T, U>& emplace_pair(const T& key, const U& value);       //places the key/value pair passed into *this, if the value at "key" is already defined, then the value at "key" is over-written and replaced
    U& operator[](const T& key);            //returns the values at "key" by reference; if the value at "key" has not been initialized, then the value at "key" will be initialized and "ele_count" will be incremented
    U read_at(const T& key) const;          //returns a copy of the value at "key", is const because changes to the contents returned are not reflected in *this
    bool contains(const T& key) const;      //returns true if the "key" is found within *this, else returns false
    hash_ordered_map<T, U>& merge_with(const hash_ordered_map<T, U>&);
    hash_ordered_map<T, U>& overlap_with(const hash_ordered_map<T, U>&);
    ~hash_ordered_map() { delete[] data; }  //destructor

private:
    explicit hash_ordered_map(int capReq);             //creates a hash_ordered_map that has an initial "max_cap" equal to "capReq" (capacity-requirement)
    int increase_max_cap(unsigned int hashIndex);     //used to increase the maximum amount of elements *this can contain, and creates the next "clean_index" to edit based on the passed "hashIndex"

    struct HashPair{    //used to store the hash value of each key, and the value of each key
        HashPair()=default;
        HashPair(unsigned int hashNum, const U& val) { hash = hashNum; value = new U(val); }
        HashPair(const HashPair& toCopy) { hash = toCopy.hash; if(toCopy.value != nullptr){ value = new U(*toCopy.value); } }
        HashPair& operator=(const HashPair& toAssign) {
            if(this == &toAssign){ return *this; }
            hash = toAssign.hash; delete value; value = nullptr;
            if(toAssign.value != nullptr){ value = new U(*toAssign.value); }
            return *this; }
        ~HashPair() { delete value; }

        unsigned int hash = 0;
        U* value = nullptr;
    };

    HashPair* data = nullptr;   //holds data for *this
    int ele_count = 0;          //element counter, used for size metric
    int max_cap;                //used to document the maximum amount of elements "data" can hold, and documents the largest index possible of "data"
};

template<class T, class U>
hash_ordered_map<T, U>::hash_ordered_map() {
    data = new HashPair[20];
    max_cap = 20;
}

template<class T, class U>
hash_ordered_map<T, U>::hash_ordered_map(int capReq) {
    data = new HashPair[capReq];
    max_cap = capReq;
}

template<class T, class U>
hash_ordered_map<T, U>& hash_ordered_map<T, U>::emplace_pair(const T &key, const U &value) {
    std::hash<T> hashObj;
    const unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    HashPair toInsert(index, value);

    while(index_clean < max_cap && data[index_clean].value != nullptr){
        if(data[index_clean].hash == index) {
            --ele_count;
            break;
        }
        ++index_clean;
    }

    if(index_clean >= max_cap) {
        index_clean = increase_max_cap(index);
    }

    data[index_clean] = toInsert;
    ++ele_count;

    return *this;
}

template<class T, class U>
U& hash_ordered_map<T, U>::operator[](const T &key) {
    std::hash<T> hashObj;
    const unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
        ++index_clean;
    }

    if(index_clean >= max_cap) {
        index_clean = increase_max_cap(index);
    }

    data[index_clean].hash = index;

    if(data[index_clean].value == nullptr) {
        data[index_clean].value = new U();
        ++ele_count;
    }

    return *data[index_clean].value;
}

template<class T, class U>
U hash_ordered_map<T, U>::read_at(const T &key) const {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
        ++index_clean;
    }

    if(index_clean >= max_cap || data[index_clean].value == nullptr) {
        throw std::invalid_argument("Error in \"U hash_ordered_map<T, U>::read_at(const T &key) const\" | key not found");
    }

    return *data[index_clean].value;
}

template<class T, class U>      //pass and return index values because all index values are going to be changed due to their dependence on "max_cap"
int hash_ordered_map<T, U>::increase_max_cap(unsigned int hashIndex) {
    int mcCopy = max_cap;
    max_cap *= 2;
    auto* dataCopy = new HashPair[max_cap];
    for (int i = 0; i < mcCopy; ++i) {
        if(data[i].value == nullptr)
            continue;
        const unsigned int index = data[i].hash;
        int index_clean = index % max_cap;
        HashPair toInsert(index, *data[i].value);

        while(index_clean < max_cap && dataCopy[index_clean].value != nullptr){
            if(dataCopy[index_clean].hash == index) {
                break;
            }
            ++index_clean;
        }
        dataCopy[index_clean] = toInsert;
    }
    delete[] data;
    data = dataCopy;

    int index_clean = hashIndex % max_cap;    //now that "data" has been resized, we can compute the "clean_index" value based off of the "passedIndex" and the new "max_cap" value
    while(index_clean < max_cap && data[index_clean].value != nullptr){
        if(data[index_clean].hash == hashIndex) {
            break;
        }
        ++index_clean;
    }
    return index_clean;
}

template<class T, class U>
hash_ordered_map<T, U> &hash_ordered_map<T, U>::clear_value_at(const T &key) {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
        ++index_clean;
    }

    if(index_clean >= max_cap || data[index_clean].value == nullptr) {
        throw std::invalid_argument("Error in \"hash_ordered_map<T, U> &hash_ordered_map<T, U>::clear_value_at(const T &key)\" | key not found");
    }

    data[index_clean].hash = 0;
    delete data[index_clean].value;
    data[index_clean].value = nullptr;
    --ele_count;

    return *this;
}

template<class T, class U>
hash_ordered_map<T, U>::hash_ordered_map(const hash_ordered_map<T, U> &toCopy) {
    ele_count = toCopy.ele_count;
    max_cap = toCopy.max_cap;
    data = new HashPair[max_cap];
    for (int i = 0; i < max_cap; ++i) {
        if(toCopy.data[i].value == nullptr)
            continue;
        data[i] = toCopy.data[i];
    }
}

template<class T, class U>
hash_ordered_map<T, U> &hash_ordered_map<T, U>::operator=(const hash_ordered_map<T, U> &toAssign) {
    if(this == &toAssign)
        return *this;

    ele_count = toAssign.ele_count;
    max_cap = toAssign.max_cap;
    delete[] data;
    data = new HashPair[max_cap];
    for (int i = 0; i < max_cap; ++i) {
        if(toAssign.data[i].value == nullptr)
            continue;
        data[i] = toAssign.data[i];
    }

    return *this;
}

template<class T, class U>
bool hash_ordered_map<T, U>::contains(const T &key) const {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
        ++index_clean;
    }

    if(index_clean >= max_cap || data[index_clean].value == nullptr) {
        return false;
    }

    return true;
}

template<class T, class U>
hash_ordered_map<T, U> &hash_ordered_map<T, U>::merge_with(const hash_ordered_map<T, U> & passedMap) {
    for (int i = 0; i < passedMap.max_cap; ++i) {
        if(passedMap.data[i].value == nullptr)
            continue;

        const unsigned int index = passedMap.data[i].hash;
        int index_clean = index % max_cap;
        while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
            ++index_clean;
        }

        if(index_clean >= max_cap) {
            index_clean = increase_max_cap(index);
        }

        data[index_clean].hash = index;

        if(data[index_clean].value == nullptr) {
            data[index_clean].value = new U();
            *data[index_clean].value = *passedMap.data[i].value;
            ++ele_count;
        }
        else
            *data[index_clean].value += *passedMap.data[i].value;   //adds the two "value"s together
    }

    return *this;
}

template<class T, class U>
hash_ordered_map<T, U> &hash_ordered_map<T, U>::overlap_with(const hash_ordered_map<T, U> &passedMap) {
    for (int i = 0; i < passedMap.max_cap; ++i) {
        if(passedMap.data[i].value == nullptr)
            continue;

        const unsigned int index = passedMap.data[i].hash;
        int index_clean = index % max_cap;
        while(index_clean < max_cap && data[index_clean].value != nullptr && data[index_clean].hash != index){
            ++index_clean;
        }

        if(index_clean >= max_cap) {
            index_clean = increase_max_cap(index);
        }

        data[index_clean].hash = index;

        if(data[index_clean].value == nullptr) {
            data[index_clean].value = new U();
            ++ele_count;
        }

        *data[index_clean].value = *passedMap.data[i].value;    //overwrites *this' version of "value" with the "passedMap" version of "value"
    }

    return *this;
}

#endif //INC_22S_FINAL_PROJ_HASH_ORDERED_MAP_H
