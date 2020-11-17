cmake_minimum_required(VERSION 3.12)
project(fuzz VERSION 0.1)

add_library(fuzz src/test.cpp src/test.h src/test_main.h src/test_main.cpp)
add_library(fuzz::fuzz ALIAS fuzz)
target_include_directories(fuzz PUBLIC src/)

target_compile_features(fuzz PUBLIC cxx_std_17)

add_subdirectory(examples)


set(LLVM_LINK_COMPONENTS support)
set(CMAKE_CXX_STANDARD 17)

add_clang_executable(test-builder
    TestBuilder.cpp
    src/test.cpp
    src/test_main.cpp
  )
  
target_link_libraries(test-builder
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )

include_directories(src)