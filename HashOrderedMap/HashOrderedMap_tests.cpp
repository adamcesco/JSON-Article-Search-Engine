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

TEST_CASE("Testing HashOrderedMap filling and reading methods", "[HashOrderedMap]"){
    HashOrderedMap<char, int> testDummy;

    SECTION("Testing \"U& HashOrderedMap<T, U>::operator[](const T&)\""){
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
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.read_at('A') == 123);
        REQUIRE(testDummy.size() == 1);
        REQUIRE(testDummy.is_empty() == false);
    }

    SECTION("Testing \"U HashOrderedMap<T, U>::read_at(const T &key) const\""){
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.read_at('A') == 123);

        bool error = false;
        try{
            testDummy.read_at('?');
        }
        catch (const std::invalid_argument& e){
            error = true;
        }
        REQUIRE(error);
    }

    SECTION("Testing \"void HashOrderedMap<T, U>::increase_max_cap()\" by building with \"U& HashOrderedMap<T, U>::operator[](const T&)\""){
        int counter[27];
        for (int & it : counter) {
            it = 0;
        }

        for (int i = 0; i < 100; ++i) {
            char key = (i % 26) + 'A';
            testDummy[key] += i;
            counter[key & 31] += i;
        }
        REQUIRE(testDummy.size() == 26);
        REQUIRE(testDummy.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }

    SECTION("Testing \"void HashOrderedMap<T, U>::increase_max_cap()\" by building with \"HashOrderedMap<T, U>& HashOrderedMap<T, U>::emplace_pair(const T &key, const U &value)\""){
        int counter[27];
        for (int & it : counter) {
            it = 0;
        }

        for (int i = 0; i < 100; ++i) {
            char key = (i % 26) + 'A';
            int randNum = rand();
            testDummy.emplace_pair(key, randNum);
            counter[key & 31] = randNum;
        }
        REQUIRE(testDummy.size() == 26);
        REQUIRE(testDummy.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }
}

TEST_CASE("Testing HashOrderedMap clearing methods", "[HashOrderedMap]") {
    HashOrderedMap<char, int> testDummy;

    SECTION("Testing \"HashOrderedMap<T, U> &HashOrderedMap<T, U>::clear_value_at(const T &key)\""){
        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            int randNum = rand();
            testDummy.emplace_pair(key, randNum);
        }
        REQUIRE(testDummy.size() == 26);

        testDummy.clear_value_at('A');
        REQUIRE(testDummy.size() == 25);

        testDummy.clear_value_at('B');
        REQUIRE(testDummy.size() == 24);

        bool error = false;
        try{
            testDummy.clear_value_at('?');
        }
        catch (const std::invalid_argument& e){
            error = true;
        }
        REQUIRE(error);
    }

    SECTION("Testing \"HashOrderedMap<T, U> &HashOrderedMap<T, U>::clear()\""){
        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            int randNum = rand();
            testDummy.emplace_pair(key, randNum);
        }
        REQUIRE(testDummy.size() == 26);

        testDummy.clear();
        REQUIRE(testDummy.size() == 0);
        REQUIRE(testDummy.is_empty() == true);

        bool error = false;
        try{
            testDummy.clear_value_at('A');
        }
        catch (const std::invalid_argument& e){
            error = true;
        }
        REQUIRE(error);
    }
}