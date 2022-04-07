//
// Created by Adam Escobedo on 4/7/2022.
//

#include "../catch.hpp"
#include "HashOrderedMap.h"
#include <iostream>

TEST_CASE("Testing basic constructors and destructor", "[HashOrderedMap]"){
    SECTION("Testing default constructor"){
        HashOrderedMap<char, int> testDummy;
        REQUIRE(testDummy.size() == 0);
        REQUIRE(testDummy.is_empty() == true);
    }
}

TEST_CASE("Testing HashOrderedMap filling", "[HashOrderedMap]"){
    SECTION("Testing \"U& HashOrderedMap<T, U>::operator[](const T&)\""){
        HashOrderedMap<char, int> testDummy;
        testDummy['A'] = 123;
        REQUIRE(testDummy['A'] == 123);
        REQUIRE(testDummy.size() == 1);
        REQUIRE(testDummy.is_empty() == false);

        HashOrderedMap<std::string, int> homTestString;
        homTestString["Adam"] = 123;
        REQUIRE(homTestString["Adam"] == 123);
        REQUIRE(homTestString.size() == 1);
        REQUIRE(homTestString.is_empty() == false);
    }

    SECTION("Testing \"HashOrderedMap<T, U>& HashOrderedMap<T, U>::emplace_pair(const T &key, const U &value)\""){
        HashOrderedMap<char, int> testDummy;
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.read_at('A') == 123);
    }

    SECTION("Testing \"void HashOrderedMap<T, U>::increase_max_cap()\" by building with \"U& HashOrderedMap<T, U>::operator[](const T&)\""){
        HashOrderedMap<char, int> testDummy;
        int counter[27];
        for (int & it : counter) {
            it = 0;
        }

        for (int i = 0; i < 100; ++i) {
            char key = (i % 26) + 'a';
            testDummy[key] += i;
            counter[key & 31] += i;
        }
        REQUIRE(testDummy.size() == 26);
        REQUIRE(testDummy.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'a';
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }

    SECTION("Testing \"void HashOrderedMap<T, U>::increase_max_cap()\" by building with \"HashOrderedMap<T, U>& HashOrderedMap<T, U>::emplace_pair(const T &key, const U &value)\""){
        HashOrderedMap<char, int> testDummy;
        int counter[27];
        for (int & it : counter) {
            it = 0;
        }

        for (int i = 0; i < 100; ++i) {
            char key = (i % 26) + 'a';
            int randNum = rand();
            testDummy[key] = randNum;
            counter[key & 31] = randNum;
        }
        REQUIRE(testDummy.size() == 26);
        REQUIRE(testDummy.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'a';
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }
}