#include <iostream>
#include "catch_setup.h"
#include "SearchEngine.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        runCatchTests();
        return 0;
    } else {
        SearchEngine engine(argv[1]);
        engine.generateIndex();
        if (argc == 3) {
            std::vector<std::string> results = engine.speedSearchFor(argv[2]);
            std::cout << results.size() << std::endl;
            for (const auto &it: results) {
                std::cout << it << std::endl;
            }
        }
    }
    return 0;
}
