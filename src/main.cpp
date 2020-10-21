#include "test.h"
#include <iostream>

int main() {
    Type tInt { PrimitiveType::INT };
    TestSignature max{ "std::max", { tInt, tInt }, tInt };

    std::mt19937 gen { std::random_device{}() };
    std::cout << Test::generate(gen, "ExampleTest", max).print() << std::endl;
    return 0;
}
