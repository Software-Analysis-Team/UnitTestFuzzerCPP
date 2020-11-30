#pragma once

#include <cstddef>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace test_rt {
static inline void writeHex(const void *ptr, std::size_t size) {
    std::stringstream ss;

    ss << std::setw(2) << std::setfill('0') << std::hex;

    for (std::size_t i = 0; i < size; ++i) {
        unsigned byte = static_cast<const unsigned char*>(ptr)[i];
        ss << byte;
    }

    std::cout << ss.str();
}

static inline void readHex(void *ptr, std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i) {
        char s[3];
        std::cin >> s[0] >> s[1];
        s[2] = 0;
        unsigned byte = std::strtol(s, nullptr, 16);
        static_cast<unsigned char*>(ptr)[i] = byte;
    }
}
}

#define READ(name, type) auto name = type{}; test_rt::readHex(&name, sizeof(name));1z
