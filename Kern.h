#include <Windows.h>
#include <iostream>
#include <string>
#include <map>
#include <sstream>

auto Obtain() -> std::map<std::string, std::string>
{
    MEMORYSTATUSEX statex{};
    SYSTEM_INFO sysInfo{};

    statex.dwLength = sizeof(statex);

    GlobalMemoryStatusEx(&statex);
    GetSystemInfo(&sysInfo);

    std::map<std::string, std::string> infoMap;

    infoMap["Memory Load"] = std::to_string(statex.dwMemoryLoad);
    infoMap["Total Physical Memory (KB)"] = std::to_string(statex.ullTotalPhys / 1024);
    infoMap["Available Physical Memory (KB)"] = std::to_string(statex.ullAvailPhys / 1024);
    infoMap["Number of Processors"] = std::to_string(sysInfo.dwNumberOfProcessors);
    infoMap["Page Size (Bytes)"] = std::to_string(sysInfo.dwPageSize);

    return infoMap;
}
