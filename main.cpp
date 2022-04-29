#include <iostream>
#include "catch_setup.h"
#include "SearchEngine.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        runCatchTests();
        return 0;
    } else {
        SearchEngine engine(argv[2]);
        engine.InitiateConsoleInterface();


//        engine.buildAvlTreeFromCache();
//        engine.cacheAvlTree();
    }
    return 0;
}
