#include <string>
#include <utility>
#include <vector>
#include <array>
#include <system_error>
#include <unistd.h>
#include <sys/wait.h>

class Subprocess {
private:
    std::vector<std::string> cmd;
    std::string capturedStdout;
    int exitCode;

public:
    explicit Subprocess(std::vector<std::string> cmd);
    explicit operator bool() const {
        return exitCode == 0;
    }

    Subprocess &run(std::string input);
    [[nodiscard]] std::string output() const;
};

Subprocess::Subprocess(std::vector<std::string> cmd) : cmd(std::move(cmd)), exitCode(0) {}

Subprocess &Subprocess::run(std::string input) {
    exitCode = -1;
    capturedStdout.clear();

    int inputPipe[2];
    int outputPipe[2];

    if (pipe(inputPipe) != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    if (pipe(outputPipe) != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    pid_t pid = fork();

    if (pid < 0) {
        throw std::system_error(errno, std::generic_category());
    }

    if (pid == 0) {
        // child
        close(inputPipe[1]);
        close(outputPipe[0]);
        dup2(0, inputPipe[0]); // remap stdin
        dup2(1, outputPipe[1]); // remap stdout

        std::vector<char*> argv;
        for (auto s : cmd) {
            argv.push_back(s.data());
        }

        execvp(cmd[0].c_str(), argv.data());

    }

    // parent

    close(outputPipe[1]);
    write(inputPipe[0], input.data(), input.size());
    close(inputPipe[0]);

    int stat;
    waitpid(pid, &stat, 0);

    if (WIFEXITED(stat)) {
        exitCode = WEXITSTATUS(stat);
    } else if (WIFSIGNALED(stat)) {
        exitCode = -WTERMSIG(stat);
    }

    std::array<char, 4096> buf;
    int rd;

    while ((rd = read(outputPipe[0], buf.data(), buf.size())) > 0) {
        std::copy(buf.begin(), buf.begin() + rd, std::back_inserter(capturedStdout));
    }

    return *this;
}

std::string Subprocess::output() const {
    return capturedStdout;
}
