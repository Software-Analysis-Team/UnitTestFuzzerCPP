#include "test_main.h"

int main(int argc, char **argv) {
    auto n = primitiveType(PrimitiveType::INT, 0, 10);
    auto tInt = primitiveType(PrimitiveType::INT);
    TestSignature fib{ "fib", { n }, tInt };

    TestMain(argc, argv)
        .fuzz(&fib, 5)
        .run();
    return 0;
}
