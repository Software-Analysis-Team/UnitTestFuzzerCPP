#include "test_main.h"

int main(int argc, char **argv) {
    auto n = primitiveType(PrimitiveType::INT, 0, 20);
    auto tInt = primitiveType(PrimitiveType::INT);
    TestSignature fib{ "fib", { n }, tInt, "../examples/fib.cpp" };

    TestMain(argc, argv)
        .fuzz(&fib, 5)
        .run();
    return 0;
}
