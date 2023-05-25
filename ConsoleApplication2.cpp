#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include "Fuzzer.h"
#include "sortation.h"

std::unordered_map<std::string, std::string> urlResponses;
std::shared_mutex responseMutex;

int main()
{
    StartFuzzer();
    DataSorter sorter;
    sorter.printData();
}
