#include "test_main.h"
#include <iostream>

int main(int argc, char **argv) {
    auto n = primitiveType(PrimitiveType::INT, 0, 10);
    auto tInt = primitiveType(PrimitiveType::INT);
    TestSignature fib{ "fib", { n }, tInt, "/home/tsarn/Coding/UnitTestFuzzerCPP/examples/fib.cpp" };

    std::cout << fib.callSerialized("15") << std::endl;
    return 0;

    TestMain(argc, argv)
        .fuzz(&fib, 5)
        .run();
    return 0;
}
