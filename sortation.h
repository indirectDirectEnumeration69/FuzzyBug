#pragma once
#include "Fuzzer.h"

struct datasortType {
    char characters = 'a' - 'Z';
    std::string wordsfromChars;
    int Numbers = 0 - 9;

    datasortType() {
        std::lock_guard<std::mutex> lock(responseMutex);

    }
    ~datasortType() {}
};