#include "test_main.h"

#include <iostream>
#include <cstdlib>

TestMain::TestMain(int argc, char **argv) : gen { std::random_device{}() } {
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
}

TestMain &TestMain::add(Test test) {
    tests.push_back(std::move(test));
    return *this;
}

void TestMain::usage() {
    std::cerr << "Usage: " << args[0] << " <-g|-t>";
    std::exit(1);
}

void TestMain::run() {
    if (args.size() != 2) {
        usage();
    }

    if (args[1] == "-g") {
        for (const auto& test : tests) {
            test.printPreludeGenerator(std::cout);
        }
    } else if (args[1] == "-t") {
        for (const auto& test : tests) {
            test.print(std::cout);
        }
    } else {
        usage();
    }
}

TestMain &TestMain::fuzz(const TestSignature& testSignature, int n) {
    for (int i = 0; i < n; ++i) {
        add(Test::generate(gen, "test_" + std::to_string(i), testSignature));
    }

    return *this;
}
