cmake_minimum_required(VERSION 3.12)
project(fuzz VERSION 0.1)

add_executable(fuzz src/test.cpp src/test.h)
set_target_properties(fuzz PROPERTIES CXX_STANDARD 17)
