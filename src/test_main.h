#pragma once

#include "test.h"
#include <vector>
#include <string>
#include <unordered_set>

class TestMain {
private:
    std::vector<std::string> args;
    std::vector<Test> tests;
    std::unordered_set<const TestSignature*> signatures;
    std::mt19937 gen;

private:
    void usage();

public:
    TestMain(int argc, char **argv);

    TestMain &add(Test test);
    TestMain &fuzz(TestSignature *testSignature, int n);
    void run();
};
