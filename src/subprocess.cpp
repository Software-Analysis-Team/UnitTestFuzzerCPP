#include "subprocess.h"
#include <utility>
#include <array>
#include <cstring>
#include <iostream>
#include <system_error>
#include <unistd.h>
#include <sys/wait.h>

Subprocess::Subprocess(std::vector<std::string> cmd) : cmd(std::move(cmd)), exitCode(0) {}

Subprocess &Subprocess::run(std::string input) {
    exitCode = -1;
    capturedStdout.clear();

    std::array<char, 4096> buf;

    int inputPipe[2];
    int outputPipe[2];
    int statusPipe[2];

    if (pipe(inputPipe) != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    if (pipe(outputPipe) != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    if (pipe(statusPipe) != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    pid_t pid = fork();

    if (pid < 0) {
        throw std::system_error(errno, std::generic_category());
    }

    int ready = 0;

    if (pid == 0) {
        // child
        close(inputPipe[1]);
        close(outputPipe[0]);
        close(statusPipe[1]);

        read(statusPipe[0], &ready, sizeof(ready));
        close(statusPipe[0]);

        dup2(inputPipe[0], 0); // remap stdin
        dup2(outputPipe[1], 1); // remap stdout

        std::vector<char*> argv;
        for (auto s : cmd) {
            argv.push_back(strdup(s.c_str()));
        }

        execvp(cmd[0].c_str(), argv.data());
    }

    // parent

    close(outputPipe[1]);
    close(inputPipe[0]);
    close(statusPipe[0]);
    write(inputPipe[1], input.data(), input.size());
    close(inputPipe[1]);
    write(statusPipe[1], &ready, sizeof(ready));
    close(statusPipe[1]);

    int stat;
    waitpid(pid, &stat, 0);

    if (WIFEXITED(stat)) {
        exitCode = WEXITSTATUS(stat);
    } else if (WIFSIGNALED(stat)) {
        exitCode = -WTERMSIG(stat);
    }

    int rd;
    while ((rd = read(outputPipe[0], buf.data(), buf.size())) > 0) {
        std::copy(buf.begin(), buf.begin() + rd, std::back_inserter(capturedStdout));
    }

    close(outputPipe[0]);

    return *this;
}

std::string Subprocess::output() const {
    return capturedStdout;
}

Subprocess::operator bool() const {
    return exitCode == 0;
}
