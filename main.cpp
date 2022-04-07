#include <iostream>
#include "catch_setup.h"

int main(int argc, char** argv) {
    if(argc == 1) {
        runCatchTests();
        return 0;
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
