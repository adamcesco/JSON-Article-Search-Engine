//
// Created by Adam Escobedo on 4/8/2022.
//

#include "../catch.hpp"
#include "avl_tree.h"
#include <iostream>
#include <vector>
#include <unordered_set>

TEST_CASE("Testing avl_tree construct and destructor", "[avl_tree]") {
    SECTION("Testing default constructor and destructor") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, 2);
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.get_at(1) == 2);
    }

    SECTION("Testing avl tree filling (LL rotations only) and destructor") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, -1);
        testDummy.insert(2, -2);
        testDummy.insert(3, -3);
        testDummy.insert(4, -4);
        testDummy.insert(5, -5);
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.contains(2));
        REQUIRE(testDummy.contains(3));
        REQUIRE(testDummy.contains(4));
        REQUIRE(testDummy.contains(5));
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree filling (RR rotations only) and destructor") {
        avl_tree<int, int> testDummy;
        testDummy.insert(5, -5);
        testDummy.insert(4, -4);
        testDummy.insert(3, -3);
        testDummy.insert(2, -2);
        testDummy.insert(1, -1);
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.contains(2));
        REQUIRE(testDummy.contains(3));
        REQUIRE(testDummy.contains(4));
        REQUIRE(testDummy.contains(5));
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree filling (LL rotations only) and destructor | key: int, value: std::vector<int>") {
        avl_tree<int, std::vector<int>> testDummy;
        testDummy.insert_overwriting(1, {-1});
        testDummy.insert_overwriting(2, {-2});
        testDummy.insert_overwriting(3, {-3});
        testDummy.insert_overwriting(4, {-4});
        testDummy.insert_overwriting(5, {-5});
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.contains(2));
        REQUIRE(testDummy.contains(3));
        REQUIRE(testDummy.contains(4));
        REQUIRE(testDummy.contains(5));
        REQUIRE(testDummy.is_balanced());

        testDummy.get_at(1).push_back(-10);
        REQUIRE(testDummy.get_at(1)[0] == -1);
        REQUIRE(testDummy.get_at(1)[1] == -10);
    }

    SECTION("Testing avl tree random filling (all rotations) and destructor") {
        avl_tree<int, int> testDummy;

        for (int i = 0; i < 3330; ++i) {
            testDummy.insert(rand(), i);
        }
        REQUIRE(testDummy.is_balanced());
        REQUIRE(testDummy.size() == 3330);
    }

    SECTION("Testing avl tree random filling (all rotations) and destructor | key: int, value: std::vector<int>") {
        avl_tree<int, std::vector<int>> testDummy;

        for (int i = 0; i < 33300; ++i) {
            testDummy.insert_overwriting(rand(), {i});
        }
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree filling and reading | key: int, value: std::vector<int>") {
        avl_tree<int, std::vector<int>> testDummy;

        for (int i = 0; i < 1000; ++i) {
            testDummy.insert_overwriting(i, {i});
        }
        REQUIRE(testDummy.is_balanced());

        for (int i = 0; i < 1000; ++i) {
            REQUIRE(testDummy.get_at(i).size() == 1);
            testDummy.get_at(i).push_back(0);
            REQUIRE(testDummy.get_at(i)[1] == 0);
        }
        REQUIRE(testDummy.is_balanced());

        for (int i = 0; i < 1000; ++i) {
            testDummy.get_at(0).push_back(0);
        }
        REQUIRE(testDummy.get_at(0).size() == 1002);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing") {
        avl_tree<int, int> testDummy;
        testDummy.insert(5, -5);
        testDummy.insert(4, -4);
        testDummy.insert(3, -3);
        testDummy.insert(2, -2);
        testDummy.insert(1, -1);
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.contains(2));
        REQUIRE(testDummy.contains(3));
        REQUIRE(testDummy.contains(4));
        REQUIRE(testDummy.contains(5));
        REQUIRE(testDummy.is_balanced());

        testDummy.delete_node(5);

        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy.contains(2));
        REQUIRE(testDummy.contains(3));
        REQUIRE(testDummy.contains(4));
        REQUIRE(!testDummy.contains(5));
        REQUIRE(testDummy.is_balanced());
        REQUIRE(testDummy.size() == 4);
    }

    SECTION("Testing random avl tree node deletion and destructing") {
        avl_tree<int, std::vector<int>> testDummy;
        std::unordered_set<int> insertedNums;

        for (int i = 0; i < 1000; ++i) {
            int toInsert = rand() % 1000;
            insertedNums.emplace(toInsert);
            testDummy.insert_overwriting(toInsert, {i});
        }

        for (const auto &it: insertedNums) {
            testDummy.delete_node(it);
        }
        REQUIRE(testDummy.size() == 0);
    }
}