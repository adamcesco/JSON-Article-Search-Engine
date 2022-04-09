cmake_minimum_required(VERSION 3.16)
project(22s_final_proj)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
link_libraries(stdc++fs)

add_executable(22s_final_proj main.cpp catch_setup.cpp hash_ordered_map/hash_ordered_map.h hash_ordered_map/hash_ordered_map_tests.cpp search_engine.cpp SearchEngine.h Processor.cpp Processor.h TableBundle.h utils.h)
