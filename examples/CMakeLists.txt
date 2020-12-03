add_executable(fib_generator fib_generator.cpp)
target_link_libraries(fib_generator PRIVATE fuzz::fuzz)
