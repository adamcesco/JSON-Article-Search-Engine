#include <iostream>
#include "catch_setup.h"
#include "SearchEngine.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        runCatchTests();
        return 0;
    } else {
        std::cout << "Usage: " << argv[1] << std::endl;
        SearchEngine engine(argv[1]);
    }
    return 0;
}
