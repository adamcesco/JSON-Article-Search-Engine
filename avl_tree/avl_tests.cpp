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
    }

    SECTION("Testing destructor") {
        avl_tree<int, int> testDummy;
        testDummy.insert(1, -1);
        testDummy.insert(2, -2);
        testDummy.insert(3, -3);
        testDummy.insert(4, -4);
        testDummy.insert(5, -5);
        REQUIRE(testDummy.contains(5));
    }
}