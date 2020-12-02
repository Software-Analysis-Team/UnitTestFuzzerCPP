#include "test_main.h"

#include <iostream>
#include <cstdlib>

TestMain::TestMain(int argc, char **argv) : gen { std::random_device{}() } {
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
}

TestMain &TestMain::add(Test test) {
    signatures.insert(test.signature);
    tests.push_back(std::move(test));
    return *this;
}

void TestMain::usage() {
    std::cerr << "Usage: " << args[0] << std::endl;
    std::exit(1);
}

void TestMain::run() {
    if (args.size() != 1) {
        usage();
    }

    std::cout << "#include <gtest/gtest.h>\n";

    for (auto signature : signatures) {
        std::cout << signature->print();
    }

    for (const auto& test : tests) {
        std::cout << test.print();
    }
}

TestMain &TestMain::fuzz(TestSignature *testSignature, int n) {
    for (int i = 0; i < n; ++i) {
        add(Test::generate(gen, "test_" + std::to_string(i), testSignature));
    }

    return *this;
}
