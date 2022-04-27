//
// Created by Adam Escobedo on 4/7/2022.
//

#include "../catch.hpp"
#include "hash_table.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

TEST_CASE("Testing basic constructors and destructor", "[hash_table]") {
    SECTION("Testing default constructor") {
        hash_table<char, int> testDummy;
        REQUIRE(testDummy.size() == 0);
        REQUIRE(testDummy.is_empty() == true);
    }
}

TEST_CASE("Testing hash_table filling and reading methods", "[hash_table]") {
    hash_table<char, int> testDummy;

    SECTION("Testing \"U& hash_table<T, U>::operator[](const T&)\"") {
        testDummy['A'] = 123;
        REQUIRE(testDummy['A'] == 123);
        REQUIRE(testDummy.size() == 1);
        REQUIRE(testDummy.is_empty() == false);

        hash_table<std::string, int> homTestString;
        homTestString["Adam"] = 123;
        REQUIRE(homTestString["Adam"] == 123);
        REQUIRE(homTestString.size() == 1);
        REQUIRE(homTestString.is_empty() == false);
    }

    SECTION("Testing \"hash_table<T, U>& hash_table<T, U>::emplace_pair(const T &key, const U &value)\"") {
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.read_at('A') == 123);
        REQUIRE(testDummy.size() == 1);
        REQUIRE(testDummy.is_empty() == false);
    }

    SECTION("Testing \"U hash_table<T, U>::read_at(const T &key) const\"") {
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.read_at('A') == 123);

        CHECK_THROWS(testDummy.read_at('?'));
    }

    SECTION("Testing bool hash_table<T, U>::contains(const T &key) const\"") {
        testDummy.emplace_pair('A', 123);
        REQUIRE(testDummy.contains('A') == true);
        REQUIRE(testDummy.contains('B') == false);
    }

    SECTION("Testing \"void hash_table<T, U>::increase_max_cap()\" by building with \"U& hash_table<T, U>::operator[](const T&)\"") {
        int counter[27];
        for (int &it: counter) {
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
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }

    SECTION("Testing \"void hash_table<T, U>::increase_max_cap()\" by building with \"hash_table<T, U>& hash_table<T, U>::emplace_pair(const T &key, const U &value)\"") {
        int counter[27];
        for (int &it: counter) {
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
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(testDummy[key] == counter[key & 31]);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
        }
    }

    SECTION("Testing large file reading via \"hash_table/test_Oedipus-King-of-Thebes.txt\" and comparing STL unordered_maps to HashOrderedMaps") {
        std::ifstream inFile;
        inFile.open("../hash_table/test_Oedipus-King-of-Thebes.txt");
        if (!inFile.is_open()) {
            std::cout
                    << "Error in Testing large file reading via \"../hash_table/test_Oedipus-King-of-Thebes.txt\" | file could not open"
                    << std::endl;
            REQUIRE(false);
            return;
        }

        hash_table<std::string, int> wordFreqCounterCUSTOM;
        std::unordered_map<std::string, int> wordFreqCounterSTL;
        while (inFile.good()) {
            std::string word;
            inFile >> word;
            wordFreqCounterCUSTOM[word]++;
            wordFreqCounterSTL[word]++;
        }
        inFile.close();

        REQUIRE(wordFreqCounterCUSTOM.size() == wordFreqCounterSTL.size());

        for (const auto &it: wordFreqCounterSTL) {
            REQUIRE(wordFreqCounterCUSTOM.contains(it.first) == true);
            REQUIRE(wordFreqCounterCUSTOM.read_at(it.first) == it.second);
        }

        for (const auto &it: wordFreqCounterCUSTOM) {
            REQUIRE(it.key != nullptr);
            REQUIRE(it.value != nullptr);
        }
    }
}

TEST_CASE("Testing hash_table clearing methods", "[hash_table]") {
    hash_table<char, int> testDummy;

    SECTION("Testing \"hash_table<T, U> &hash_table<T, U>::clear_value_at(const T &key)\"") {
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
        try {
            testDummy.clear_value_at('?');
        }
        catch (const std::invalid_argument &e) {
            error = true;
        }
        REQUIRE(error);

        error = false;
        try {
            testDummy.read_at('A');
        }
        catch (const std::invalid_argument &e) {
            error = true;
        }
        REQUIRE(error);

        REQUIRE(testDummy.contains('A') == false);
        REQUIRE(testDummy.contains('B') == false);
    }

    SECTION("Testing \"hash_table<T, U> &hash_table<T, U>::clear()\"") {
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
        try {
            testDummy.read_at('A');
        }
        catch (const std::invalid_argument &e) {
            error = true;
        }
        REQUIRE(error);
    }
}

TEST_CASE("Testing copy-constructor and assignment operator", "[hash_table]") {
    int counter[27];
    for (int &it: counter) {
        it = 0;
    }

    hash_table<char, int> testDummy;
    for (int i = 0; i < 26; ++i) {
        char key = i + 'A';
        int randNum = rand();
        testDummy.emplace_pair(key, randNum);
        counter[key & 31] = randNum;
    }

    SECTION("Testing copy-constructor") {
        hash_table<char, int> homChar(testDummy);
        REQUIRE(homChar.size() == 26);
        REQUIRE(homChar.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(homChar.contains(key) == true);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
            REQUIRE(homChar.read_at(key) == counter[key & 31]);
            REQUIRE(homChar.read_at(key) == testDummy.read_at(key));
        }
    }

    SECTION("Testing assignment operator") {
        hash_table<char, int> homChar;
        homChar = testDummy;
        REQUIRE(homChar.size() == 26);
        REQUIRE(homChar.is_empty() == false);

        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(homChar.contains(key) == true);
            REQUIRE(testDummy.read_at(key) == counter[key & 31]);
            REQUIRE(homChar.read_at(key) == counter[key & 31]);
            REQUIRE(homChar.read_at(key) == testDummy.read_at(key));
        }
    }
}

TEST_CASE("Testing appending-type methods", "[hash_table]") {
    hash_table<char, int> testDummy;
    for (int i = 0;
         i < 16; ++i) {  //fills until and including letter 'O', which of course stands for "OhGodHelpMeINeedAHotGothGF"
        char key = i + 'A';
        testDummy.emplace_pair(key, 1);
    }

    SECTION("Testing \"hash_table<T, U> &hash_table<T, U>::emplace_merge(const hash_table<T, U> & passedMap)\"") {
        hash_table<char, int> homChar;
        homChar.emplace_pair('A', -2);
        homChar.emplace_pair('B', 10);
        homChar.emplace_pair('Y', -3);
        homChar.emplace_pair('Z', -4);

        homChar.emplace_merge(testDummy);
        REQUIRE(testDummy.contains('A') == true);
        REQUIRE(homChar.contains('A') == true);
        REQUIRE(homChar.read_at('A') == -1);
        REQUIRE(testDummy.contains('B') == true);
        REQUIRE(homChar.contains('B') == true);
        REQUIRE(homChar.read_at('B') == 11);
        for (int i = 2; i < 16; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(homChar.contains(key) == true);
            REQUIRE(homChar.read_at(key) == testDummy.read_at(key));
        }
        REQUIRE(testDummy.contains('Y') == false);
        REQUIRE(homChar.contains('Y') == true);
        REQUIRE(homChar.read_at('Y') == -3);
        REQUIRE(testDummy.contains('Z') == false);
        REQUIRE(homChar.contains('Z') == true);
        REQUIRE(homChar.read_at('Z') == -4);
    }

    SECTION("Testing \"hash_table<T, U> &hash_table<T, U>::emplace_mask(const hash_table<T, U> & passedMap)\"") {
        hash_table<char, int> homChar;
        homChar.emplace_pair('A', -2);
        homChar.emplace_pair('B', 10);
        homChar.emplace_pair('Y', -3);
        homChar.emplace_pair('Z', -4);

        homChar.emplace_mask(testDummy);
        REQUIRE(testDummy.contains('A') == true);
        REQUIRE(homChar.contains('A') == true);
        REQUIRE(homChar.read_at('A') == 1);
        REQUIRE(testDummy.contains('B') == true);
        REQUIRE(homChar.contains('B') == true);
        REQUIRE(homChar.read_at('B') == 1);
        for (int i = 2; i < 16; ++i) {
            char key = i + 'A';
            REQUIRE(testDummy.contains(key) == true);
            REQUIRE(homChar.contains(key) == true);
            REQUIRE(homChar.read_at(key) == testDummy.read_at(key));
        }
        REQUIRE(testDummy.contains('Y') == false);
        REQUIRE(homChar.contains('Y') == true);
        REQUIRE(homChar.read_at('Y') == -3);
        REQUIRE(testDummy.contains('Z') == false);
        REQUIRE(homChar.contains('Z') == true);
        REQUIRE(homChar.read_at('Z') == -4);
    }
}

TEST_CASE("Testing hash_table iterator class", "[hash_map]") {
    hash_table<char, int> testDummy;
    SECTION("Testing a hash_table of greater size") {
        for (int i = 0; i < 26; ++i) {
            char key = i + 'A';
            int randNum = rand();
            testDummy.emplace_pair(key, randNum);
        }
        REQUIRE(testDummy.size() == 26);

        int counter = 0;
        for (auto &it: testDummy) {
            REQUIRE(it.key != nullptr);
            counter++;
        }
        REQUIRE(testDummy.size() == counter);
        REQUIRE(testDummy.size() == 26);
    }
}