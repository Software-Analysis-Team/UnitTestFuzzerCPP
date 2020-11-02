#pragma once

#include "test.h"
#include <vector>
#include <string>

class TestMain {
private:
    std::vector<std::string> args;
    std::vector<Test> tests;
    std::mt19937 gen;

private:
    void usage();

public:
    TestMain(int argc, char **argv);

    TestMain &add(Test test);
    TestMain &fuzz(const TestSignature& testSignature, int n);
    void run();
};
