#include <iostream>
#include <mutex>
#include <unordered_map>
#include "Fuzzer.h"
#include "sortation.h"

std::unordered_map<std::string, std::string> urlResponses;
std::mutex responseMutex;

int main()
{
    StartFuzzer();
    datasortType* NewDataLog = new datasortType;
    delete NewDataLog;
}
