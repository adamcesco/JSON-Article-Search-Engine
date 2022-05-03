#!/bin/bash
echo BUILDING AND RUNNING SEARCH ENGINE

mkdir -p build
cd build || exit
cmake ..
make
clear
./22s_final_proj global ../../data/
