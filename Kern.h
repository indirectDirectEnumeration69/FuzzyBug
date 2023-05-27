#include <Windows.h>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <thread>
#include <mutex>

class SystemInfo {
public:
    SystemInfo() {
        running = true;
        updateThread = std::thread(&SystemInfo::Update, this);
    }

    ~SystemInfo() {
        running = false;
        if (updateThread.joinable()) {
            updateThread.join();
        }
    }

    std::map<std::string, std::string> Obtain() const {
        std::lock_guard<std::mutex> lock(infoMutex);
        return infoMap;
    }

private:
    void Update() {
        while (running) {
            MEMORYSTATUSEX statex{};
            SYSTEM_INFO sysInfo{};

            statex.dwLength = sizeof(statex);

            GlobalMemoryStatusEx(&statex);
            GetSystemInfo(&sysInfo);

            std::lock_guard<std::mutex> lock(infoMutex);
            infoMap["Memory Load"] = std::to_string(statex.dwMemoryLoad);
            infoMap["Total Physical Memory (KB)"] = std::to_string(statex.ullTotalPhys / 1024);
            infoMap["Available Physical Memory (KB)"] = std::to_string(statex.ullAvailPhys / 1024);
            infoMap["Number of Processors"] = std::to_string(sysInfo.dwNumberOfProcessors);
            infoMap["Page Size (Bytes)"] = std::to_string(sysInfo.dwPageSize);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    mutable std::mutex infoMutex;
    std::map<std::string, std::string> infoMap;
    std::thread updateThread;
    bool running;
};


