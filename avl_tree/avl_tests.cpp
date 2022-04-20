//
// Created by Adam Escobedo on 4/8/2022.
//

#include "../catch.hpp"
#include "avl_tree.h"
#include "avl_tree_io.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

TEST_CASE("Testing avl_tree construct and destructor", "[avl_tree]") {
    SECTION("Testing default constructor and destructor") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, 2);
        REQUIRE(testDummy.contains(1));
        REQUIRE(testDummy[1] == 2);
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

        testDummy[1].push_back(-10);
        REQUIRE(testDummy[1][0] == -1);
        REQUIRE(testDummy[1][1] == -10);
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
            REQUIRE(testDummy[i].size() == 1);
            testDummy[i].push_back(0);
            REQUIRE(testDummy[i][1] == 0);
        }
        REQUIRE(testDummy.is_balanced());

        for (int i = 0; i < 1000; ++i) {
            testDummy[0].push_back(0);
        }
        REQUIRE(testDummy[0].size() == 1002);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing copy constructor and destructor with empty avl_tree") {
        avl_tree<int, int> testDummy;
        avl_tree<int, int> testAVL(testDummy);
        REQUIRE(testAVL.size() == 0);
    }

    SECTION("Testing copy constructor and destructor with avl_tree size 1") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, 2);

        avl_tree<int, int> testAVL(testDummy);
        REQUIRE(testAVL.contains(1));
        REQUIRE(testAVL.size() == 1);
    }

    SECTION("Testing copy constructor and destructor with avl_tree size 5") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, -1);
        testDummy.insert(2, -2);
        testDummy.insert(3, -3);
        testDummy.insert(4, -4);
        testDummy.insert(5, -5);

        avl_tree<int, int> testAVL(testDummy);
        REQUIRE(testAVL.contains(1));
        REQUIRE(testAVL.contains(2));
        REQUIRE(testAVL.contains(3));
        REQUIRE(testAVL.contains(4));
        REQUIRE(testAVL.contains(5));
        REQUIRE(testAVL.size() == 5);
        REQUIRE(testAVL.is_balanced());
    }

    SECTION("Testing assignment operator overload and destructor with empty avl_tree") {
        avl_tree<int, int> testDummy;
        avl_tree<int, int> testAVL;
        testAVL = testDummy;
        REQUIRE(testAVL.size() == 0);
    }

    SECTION("Testing assignment operator overload and destructor with avl_tree size 1") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, 2);

        avl_tree<int, int> testAVL;
        testAVL = testDummy;
        REQUIRE(testAVL.contains(1));
        REQUIRE(testAVL.size() == 1);
    }

    SECTION("Testing assignment operator overload and destructor with avl_tree size 5") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, -1);
        testDummy.insert(2, -2);
        testDummy.insert(3, -3);
        testDummy.insert(4, -4);
        testDummy.insert(5, -5);

        avl_tree<int, int> testAVL;
        testAVL = testDummy;
        REQUIRE(testAVL.contains(1));
        REQUIRE(testAVL.contains(2));
        REQUIRE(testAVL.contains(3));
        REQUIRE(testAVL.contains(4));
        REQUIRE(testAVL.contains(5));
        REQUIRE(testAVL.size() == 5);
        REQUIRE(testAVL.is_balanced());
    }
}

TEST_CASE("Testing AVL tree deletion") {
    avl_tree<int, int> testDummy;
    testDummy.insert(0, 0);
    testDummy.insert(-6, 1);
    testDummy.insert(2, 2);
    testDummy.insert(-7, 3);

    SECTION("Testing avl tree node deletion and destructing | 1") {
        testDummy.delete_node(-7);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 2") {
        testDummy.delete_node(-6);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 3") {
        testDummy.delete_node(0);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 4") {
        testDummy.delete_node(2);
        REQUIRE(testDummy.is_balanced());
    }

    testDummy.insert(1, 4);

    SECTION("Testing avl tree node deletion and destructing | 5") {
        testDummy.delete_node(2);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 5") {
        testDummy.delete_node(1);
        REQUIRE(testDummy.is_balanced());
    }

    testDummy.insert(3, 4);

    SECTION("Testing avl tree node deletion and destructing | 6") {
        testDummy.delete_node(2);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 7") {
        testDummy.delete_node(3);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing avl tree node deletion and destructing | 8") {
        testDummy.delete_node(0);
        REQUIRE(testDummy.is_balanced());
    }

    SECTION("Testing random avl tree node deletion and destructing") {
        avl_tree<int, int> testDummy;
        std::unordered_set<int> insertedNums;

        for (int i = 0; i < 1000; ++i) {
            int toInsert = rand() % 20;
            insertedNums.emplace(toInsert);
            testDummy.insert_overwriting(toInsert, i);
        }

        for (const auto it: insertedNums) {
            try {
                testDummy.delete_node(it);
            }
            catch (const std::invalid_argument &e) { std::cout << "Could not delete: " << it << std::endl; }
            if (!testDummy.is_balanced())
                std::cout << it << std::endl;
            REQUIRE(testDummy.is_balanced());
        }
        REQUIRE(testDummy.size() == 0);
    }
}

TEST_CASE("Testing AVL tree saving and building to/from file", "avl_tree_io") {
    avl_tree_io<char, int> testDummy;

    SECTION("Testing avl_tree_io saving to file and building from a file") {
        for (int i = 0; i < 26; ++i) {
            testDummy.insert('a' + i, i);
        }
        testDummy.print_levelOrder("../avl_tree/buildCorpus.txt");
        testDummy.clear();
        testDummy.build_tree_from_txt("../avl_tree/buildCorpus.txt");
        for (int i = 0; i < 26; ++i) {
            REQUIRE(testDummy['a' + i] == i);
        }
    }
}

void append_alias(std::vector<int> &vect1, const std::vector<int> &vect2) {
    vect1.push_back(vect2[0]);
}

void push_back_alias(std::vector<int> &vect1, const int &i) {
    vect1.push_back(i);
}

TEST_CASE("Testing AVL tree with list data saving and building to/from file", "avl_tree_io_list") {
    avl_tree_io_list<char, std::vector<int>, int> testDummy;
    std::unordered_map<char, std::vector<int>> tracker;

    SECTION("Testing avl_tree_io saving to file and building from a file") {
        for (int i = 0; i < 50; ++i) {
            char cc = 'a' + rand() % 26;
            testDummy.insert(cc, {i}, &append_alias);
            tracker[cc].push_back(i);
        }
        testDummy.print_levelOrder("../avl_tree/buildCorpus.txt");
        testDummy.clear();
        testDummy.build_tree_from_txt("../avl_tree/buildCorpus.txt", &push_back_alias);
        for (const auto &it: tracker) {
            REQUIRE(testDummy[it.first] == it.second);
        }
    }
}