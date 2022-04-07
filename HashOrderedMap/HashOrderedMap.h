//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_HASHORDEREDMAP_H
#define INC_22S_FINAL_PROJ_HASHORDEREDMAP_H

#include <functional>
#include <string>

template<class T, class U>
class HashOrderedMap{
public:
    HashOrderedMap();
    HashOrderedMap(const HashOrderedMap<T, U>&);
    HashOrderedMap<T, U>& operator=(const HashOrderedMap<T, U>&);
    HashOrderedMap<T, U>& clear() { delete[] data; data = new HashPair[20]; max_cap = 20; ele_count = 0; return *this; }
    HashOrderedMap<T, U>& clear_value_at(const T&);
    int size() const { return ele_count; }
    bool is_empty() const { return ele_count == 0; }
    HashOrderedMap<T, U>& emplace_pair(const T&, const U&);
    U& operator[](const T&);
    U read_at(const T&) const;
    bool contains(const T&) const;
    ~HashOrderedMap() { delete[] data; }

private:
    int increase_max_cap(unsigned int);

    struct HashPair{
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

    HashPair* data = nullptr;
    int ele_count = 0;
    int max_cap;
};

template<class T, class U>
HashOrderedMap<T, U>::HashOrderedMap() {
    data = new HashPair[20];
    max_cap = 20;
}

template<class T, class U>
HashOrderedMap<T, U>& HashOrderedMap<T, U>::emplace_pair(const T &key, const U &value) {
    std::hash<T> hashObj;
    const unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    HashPair toInsert(index, value);

    while(data[index_clean].value != nullptr && index_clean < max_cap){
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
U& HashOrderedMap<T, U>::operator[](const T &key) {
    std::hash<T> hashObj;
    const unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(data[index_clean].value != nullptr && data[index_clean].hash != index && index_clean < max_cap){
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
U HashOrderedMap<T, U>::read_at(const T &key) const {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while((data[index_clean].value == nullptr || data[index_clean].hash != index) && index_clean < max_cap){
        ++index_clean;
    }

    if(index_clean >= max_cap) {
        throw std::invalid_argument("Error in U HashOrderedMap<T, U>::read_at(const T &key) const | key not found");
    }

    return *data[index_clean].value;
}

template<class T, class U>
int HashOrderedMap<T, U>::increase_max_cap(unsigned int passedIndex) {
    int mcCopy = max_cap;
    max_cap *= 2;
    auto* dataCopy = new HashPair[max_cap];
    for (int i = 0; i < mcCopy; ++i) {
        if(data[i].value == nullptr)
            continue;
        const unsigned int index = data[i].hash;
        int index_clean = index % max_cap;
        HashPair toInsert(index, *data[i].value);

        while(dataCopy[index_clean].value != nullptr && index_clean < max_cap){
            if(dataCopy[index_clean].hash == index) {
                break;
            }
            ++index_clean;
        }
        dataCopy[index_clean] = toInsert;
    }
    delete[] data;
    data = dataCopy;

    int index_clean = passedIndex % max_cap;
    while(data[index_clean].value != nullptr && index_clean < max_cap){
        if(data[index_clean].hash == passedIndex) {
            break;
        }
        ++index_clean;
    }
    return index_clean;
}

template<class T, class U>
HashOrderedMap<T, U> &HashOrderedMap<T, U>::clear_value_at(const T &key) {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while((data[index_clean].value == nullptr || data[index_clean].hash != index) && index_clean < max_cap){
        ++index_clean;
    }
    if(index_clean >= max_cap) {
        throw std::invalid_argument("Error in U HashOrderedMap<T, U>::read_at(const T &key) const | key not found");
    }

    data[index_clean].hash = 0;
    delete data[index_clean].value;
    data[index_clean].value = nullptr;
    --ele_count;

    return *this;
}

template<class T, class U>
HashOrderedMap<T, U>::HashOrderedMap(const HashOrderedMap<T, U> &toCopy) {
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
HashOrderedMap<T, U> &HashOrderedMap<T, U>::operator=(const HashOrderedMap<T, U> &toAssign) {
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
bool HashOrderedMap<T, U>::contains(const T &key) const {
    std::hash<T> hashObj;
    unsigned int index = hashObj(key);
    int index_clean = index % max_cap;
    while(data[index_clean].value != nullptr && data[index_clean].hash != index && index_clean < max_cap){
        ++index_clean;
    }

    if(data[index_clean].value == nullptr || index_clean >= max_cap) {
        return false;
    }

    return true;
}

#endif //INC_22S_FINAL_PROJ_HASHORDEREDMAP_H
