#pragma once

#include <vector>
#include <string>

class Subprocess {
private:
    std::vector<std::string> cmd;
    std::string capturedStdout;
    int exitCode;

public:
    explicit Subprocess(std::vector<std::string> cmd);
    explicit operator bool() const;

    Subprocess &run(std::string input);
    [[nodiscard]] std::string output() const;
};
