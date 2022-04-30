#!/bin/bash
echo BUILDING AND RUNNING SEARCH ENGINE

mkdir -p build
cd build || exit
cmake ..
make
clear
./22s_final_proj global ../../data/2018_01_112b52537b67659ad3609a234388c50a
