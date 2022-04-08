#include <iostream>
#include "catch_setup.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        runCatchTests();
        return 0;
    } else {
        std::cout << "Usage: " << argv[1] << std::endl;
    }
    return 0;
}
