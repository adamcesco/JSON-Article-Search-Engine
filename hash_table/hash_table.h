//
// Created by Adam Escobedo on 4/7/2022.
//

#ifndef INC_22S_FINAL_PROJ_HASH_TABLE_H
#define INC_22S_FINAL_PROJ_HASH_TABLE_H

#include <functional>
#include <string>
#include <iostream>

template<class T, class U>
class hash_table {
private:
    struct HashPair {    //used to store the hash value of each key, and the value of each key
        HashPair() = default;

        HashPair(unsigned int hashNum, const T &pData, const U &pValue) {
            hash = hashNum;
            key = new T(pData);
            value = new U(pValue);
        }

        HashPair(const HashPair &toCopy) {
            hash = toCopy.hash;
            if (toCopy.key != nullptr) { key = new T(*toCopy.key); }
            if (toCopy.value != nullptr) { value = new U(*toCopy.value); }
        }

        HashPair &operator=(const HashPair &toAssign) {
            if (this == &toAssign) { return *this; }
            hash = toAssign.hash;

            delete key;
            key = nullptr;
            if (toAssign.key != nullptr) { key = new T(*toAssign.key); }

            delete value;
            value = nullptr;
            if (toAssign.value != nullptr) { value = new U(*toAssign.value); }

            return *this;
        }

        ~HashPair() {
            delete key;
            delete value;
        }

        unsigned int hash = 0;
        T *key = nullptr;
        U *value = nullptr;
    };

    struct Iterator {
        Iterator(HashPair *pPtr, int pMax) {
            dataPtr = pPtr;
            max = pMax;
        }

        HashPair &operator*() const {
            return *dataPtr;
        }

        Iterator &operator++() {
            ++dataPtr;
            ++i;
            while (i < max && dataPtr->key == nullptr) {
                ++dataPtr;
                ++i;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++dataPtr;
            ++i;
            while (i < max && dataPtr->key == nullptr) {
                ++dataPtr;
                ++i;
            }
            return temp;
        }

        friend bool operator==(const Iterator &a, const Iterator &b) {
            return (a.dataPtr == b.dataPtr);
        };

        friend bool operator!=(const Iterator &a, const Iterator &b) {
            return (a.dataPtr != b.dataPtr);
        };

        HashPair *dataPtr;
        int i = 0;
        int max;
    };

    //used to increase the maximum amount of elements *this can contain, and creates the next "clean_index" to edit based on the passed "hashIndex"
    int increase_max_cap(unsigned int hashIndex);

    float read_load_factor() {
        return float(ele_count) / max_cap;
    }

    inline static unsigned int hasher(const T &pKey) {
        std::hash<T> hashObj;
        return hashObj(pKey);
    }

    HashPair *data = nullptr;   //holds key for *this
    int ele_count = 0;          //element counter, used for size metric
    int max_cap;                //used to document the maximum amount of elements "key" can hold, and documents the largest index possible of "key"
    unsigned int (*hash_func)(const T &) = &hasher;

public:

    Iterator begin() {
        HashPair *dataPtr = data;
        int i = 0;
        while (i < max_cap && dataPtr->key == nullptr) {
            ++dataPtr;
            ++i;
        }
        return Iterator(dataPtr, max_cap);
    }

    Iterator end() { return Iterator(data + max_cap, 0); }

    hash_table();

    hash_table(const hash_table<T, U> &);                    //copy-constructor

    //creates a hash_table that has an initial "max_cap" equal to "capReq" (capacity-requirement)
    explicit hash_table(int capReq);

    hash_table<T, U> &operator=(const hash_table<T, U> &);   //assignment operator overload

    //resets all *this contents/key
    hash_table<T, U> &clear() {
        delete[] data;
        data = new HashPair[500];
        max_cap = 500;
        ele_count = 0;
        return *this;
    }

    hash_table<T, U> &clear_value_at(const T &pKey);               //sets the value at "pKey" to nullptr

    int size() const { return ele_count; }

    bool is_empty() const { return ele_count == 0; }

    //places the pKey/pValue pair passed into *this, if the pValue at "pKey" is already defined, then the pValue at "pKey" is over-written and replaced
    hash_table<T, U> &emplace_pair(const T &pKey, const U &pValue);

    //returns the values at "pKey" by reference; if the value at "pKey" has not been initialized, then the value at "pKey" will be initialized and "ele_count" will be incremented
    U &operator[](const T &pKey);

    //returns a copy of the value at "pKey", is const because changes to the contents returned are not reflected in *this
    U read_at(const T &pKey) const;

    bool contains(const T &pKey) const;      //returns true if the "pKey" is found within *this, else returns false

    hash_table<T, U> &emplace_merge(const hash_table<T, U> &pMap);

    hash_table<T, U> &emplace_mask(const hash_table<T, U> &pMap);

    hash_table<T, U> &set_hash_function(unsigned int (*custom_hash_func)(const T &)) { hash_func = custom_hash_func; }

    ~hash_table() { delete[] data; }  //destructor
};

template<class T, class U>
hash_table<T, U>::hash_table() {
    data = new HashPair[500];
    max_cap = 500;
}

template<class T, class U>
hash_table<T, U>::hash_table(int capReq) {
    data = new HashPair[capReq];
    max_cap = capReq;
}

template<class T, class U>
hash_table<T, U> &hash_table<T, U>::emplace_pair(const T &pKey, const U &pValue) {
    const unsigned int index = hash_func(pKey);
    int index_clean = index % max_cap;
    HashPair toInsert(index, pKey, pValue);

    while (index_clean < max_cap && data[index_clean].key != nullptr) {
        if (data[index_clean].hash == index) {
            --ele_count;
            break;
        }
        ++index_clean;
    }

    if (index_clean >= max_cap) {
        index_clean = increase_max_cap(index);
    }

    data[index_clean] = toInsert;
    ++ele_count;

    return *this;
}

template<class T, class U>
U &hash_table<T, U>::operator[](const T &pKey) {
    const unsigned int index = hash_func(pKey);
    int index_clean = index % max_cap;
    while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
        ++index_clean;
    }

    if (index_clean >= max_cap) {
        index_clean = increase_max_cap(index);
    }

    data[index_clean].hash = index;

    if (data[index_clean].key == nullptr) {
        data[index_clean].key = new T();
        ++ele_count;
    }
    *data[index_clean].key = pKey;

    if (data[index_clean].value == nullptr)
        data[index_clean].value = new U();

    return *data[index_clean].value;
}

template<class T, class U>
U hash_table<T, U>::read_at(const T &pKey) const {
    unsigned int index = hash_func(pKey);
    int index_clean = index % max_cap;
    while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
        ++index_clean;
    }

    if (index_clean >= max_cap || data[index_clean].key == nullptr) {
        throw std::invalid_argument(
                "Error in \"U hash_table<T, U>::read_at(const T &pKey) const\" | pKey not found");
    }

    return *data[index_clean].value;
}

template<class T, class U>
//pass and return index values because all index values are going to be changed due to their dependence on "max_cap"
int hash_table<T, U>::increase_max_cap(unsigned int hashIndex) {
    int mcCopy = max_cap;
    max_cap *= 10;
    auto *dataCopy = new HashPair[max_cap];
    for (int i = 0; i < mcCopy; ++i) {
        if (data[i].hash == 0)
            continue;
        const unsigned int index = data[i].hash;
        int index_clean = index % max_cap;
        HashPair toInsert(index, *data[i].key, *data[i].value);

        while (index_clean < max_cap && dataCopy[index_clean].key != nullptr) {
            if (dataCopy[index_clean].hash == index) {
                break;
            }
            ++index_clean;
        }
        dataCopy[index_clean] = toInsert;
    }
    delete[] data;
    data = dataCopy;

    //now that "key" has been resized, we can compute the "clean_index" value based off of the "passedIndex" and the new "max_cap" value
    int index_clean = hashIndex % max_cap;
    while (index_clean < max_cap && data[index_clean].key != nullptr) {
        if (data[index_clean].hash == hashIndex) {
            break;
        }
        ++index_clean;
    }
    return index_clean;
}


template<class T, class U>
hash_table<T, U> &hash_table<T, U>::clear_value_at(const T &pKey) {
    unsigned int index = hash_func(pKey);
    int index_clean = index % max_cap;
    while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
        ++index_clean;
    }

    if (index_clean >= max_cap || data[index_clean].key == nullptr) {
        throw std::invalid_argument(
                "Error in \"hash_table<T, U> &hash_table<T, U>::clear_value_at(const T &pKey)\" | passed key was not found.");
    }

    data[index_clean].hash = 0;

    delete data[index_clean].key;
    data[index_clean].key = nullptr;

    delete data[index_clean].value;
    data[index_clean].value = nullptr;

    --ele_count;

    return *this;
}

template<class T, class U>
hash_table<T, U>::hash_table(const hash_table<T, U> &toCopy) {
    ele_count = toCopy.ele_count;
    max_cap = toCopy.max_cap;
    data = new HashPair[max_cap];
    for (int i = 0; i < max_cap; ++i) {
        if (toCopy.data[i].value == nullptr)
            continue;
        data[i] = toCopy.data[i];
    }
}

template<class T, class U>
hash_table<T, U> &hash_table<T, U>::operator=(const hash_table<T, U> &toAssign) {
    if (this == &toAssign)
        return *this;

    ele_count = toAssign.ele_count;
    max_cap = toAssign.max_cap;
    delete[] data;
    data = new HashPair[max_cap];
    for (int i = 0; i < max_cap; ++i) {
        if (toAssign.data[i].value == nullptr)
            continue;
        data[i] = toAssign.data[i];
    }

    return *this;
}

template<class T, class U>
bool hash_table<T, U>::contains(const T &pKey) const {
    unsigned int index = hash_func(pKey);
    int index_clean = index % max_cap;
    while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
        ++index_clean;
    }

    if (index_clean >= max_cap || data[index_clean].key == nullptr) {
        return false;
    }

    return true;
}

template<class T, class U>
hash_table<T, U> &hash_table<T, U>::emplace_merge(const hash_table<T, U> &pMap) {
    for (int i = 0; i < pMap.max_cap; ++i) {
        if (pMap.data[i].hash == 0)
            continue;

        const unsigned int index = pMap.data[i].hash;
        int index_clean = index % max_cap;
        while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
            ++index_clean;
        }

        if (index_clean >= max_cap) {
            index_clean = increase_max_cap(index);
        }

        data[index_clean].hash = index;

        if (data[index_clean].key == nullptr) {
            data[index_clean].key = new T();
            *data[index_clean].key = *pMap.data[i].key;
            ++ele_count;
        } else
            *data[index_clean].key = *pMap.data[i].key;

        if (data[index_clean].value == nullptr) {
            data[index_clean].value = new U();
            *data[index_clean].value = *pMap.data[i].value;
        } else
            *data[index_clean].value += *pMap.data[i].value;   //adds the two "value"s together
    }

    return *this;
}

template<class T, class U>
hash_table<T, U> &hash_table<T, U>::emplace_mask(const hash_table<T, U> &pMap) {
    for (int i = 0; i < pMap.max_cap; ++i) {
        if (pMap.data[i].value == nullptr)
            continue;

        const unsigned int index = pMap.data[i].hash;
        int index_clean = index % max_cap;
        while (index_clean < max_cap && data[index_clean].key != nullptr && data[index_clean].hash != index) {
            ++index_clean;
        }

        if (index_clean >= max_cap) {
            index_clean = increase_max_cap(index);
        }

        data[index_clean].hash = index;

        if (data[index_clean].key == nullptr) {
            data[index_clean].key = new T();
            ++ele_count;
        }

        if (data[index_clean].value == nullptr) {
            data[index_clean].value = new U();
        }

        *data[index_clean].key = *pMap.data[i].key;    //overwrites *this' version of "value" with the "pMap" version of "value"
        *data[index_clean].value = *pMap.data[i].value;
    }

    return *this;
}

#endif //INC_22S_FINAL_PROJ_HASH_TABLE_H
