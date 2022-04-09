//
// Created by Adam Escobedo on 4/8/2022.
//

#include "../catch.hpp"
#include "avl_tree.h"
#include <iostream>

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
    }

    SECTION("Testing avl tree random filling (all rotations) and destructor") {
        avl_tree<int, int> testDummy;

        for (int i = 0; i <
                        3330; ++i) {  //when the last element of this loop is inserted, there is misalignment amongst nodes and their parents, find this bug
            testDummy.insert(rand(),
                             i);    //step through the creation of this tree, and see where the pointers are misaligned
        }
    }
}