#pragma once
#include "Fuzzer.h"

struct datasortType {
    char characters = 'a' - 'Z';
    std::string wordsfromChars;
    int Numbers = 0 - 9;

    datasortType() {
        std::lock_guard<std::mutex> lock(responseMutex);
        for (const auto& pair : urlResponses) {
            std::string url = pair.first;
            std::string response = pair.second;
            std::sort(response.begin(), response.end());
            std::cout << "This is the response parsed: " + response + "\n" << std::endl;
        }
    }
    ~datasortType() {}
};
