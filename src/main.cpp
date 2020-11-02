#include "test.h"
#include <iostream>

int main() {
    auto tInt = primitiveType(PrimitiveType::INT);
    TestSignature max{ "std::max", { tInt, tInt }, tInt };

    std::mt19937 gen { std::random_device{}() };
    auto test = Test::generate(gen, "ExampleTest", max);
    test.print(std::cout);
    std::cout << std::endl << std::endl;
    test.printPreludeGenerator(std::cout);
    std::cout << std::endl << std::endl;
    return 0;
}
