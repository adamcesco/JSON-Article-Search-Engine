cmake_minimum_required(VERSION 3.16)
project(22s_final_proj)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
link_libraries(stdc++fs)

#TBB
set(EXTERNAL_INSTALL_DIR ${CMAKE_BINARY_DIR}/external)
link_directories(${EXTERNAL_INSTALL_DIR}/lib)
include_directories(${EXTERNAL_INSTALL_DIR}/include)

#TBB
link_libraries(tbb)

add_executable(22s_final_proj main.cpp catch_setup.cpp hash_table/hash_table.h hash_table/hash_ordered_map_tests.cpp search_engine.cpp SearchEngine.h Processor.cpp StopWords.h TableBundle.h utils.h avl_tree/avl_tests.cpp include/porter2_stemmer/porter2_stemmer.cpp include/porter2_stemmer/porter2_stemmer.h include/porter2_stemmer/util/hash.h include/porter2_stemmer/util/string_view.h)

######################################################################
# Build rules for tbb
######################################################################

include(ExternalProject)
ExternalProject_Add(
        tbb
        GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/external/tbb
        GIT_TAG master
        INSTALL_DIR ${EXTERNAL_INSTALL_DIR}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DTBB_BUILD_SHARED=ON
        -DTBB_BUILD_STATIC=OFF
        -DTBB_BUILD_TBBMALLOC=OFF
        -DTBB_BUILD_TBBMALLOC_PROXY=OFF
        -DTBB_BUILD_TESTS=OFF
        -DTBB_CI_BUILD=OFF
)
add_dependencies(22s_final_proj tbb)
