cmake_minimum_required(VERSION 3.12)
project(fuzz VERSION 0.1)

add_library(fuzz src/test.cpp src/test.h src/test_main.h src/test_main.cpp)
add_library(fuzz::fuzz ALIAS fuzz)
target_include_directories(fuzz PUBLIC src/)

target_compile_features(fuzz PUBLIC cxx_std_17)

add_subdirectory(examples)
