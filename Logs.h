#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include "Awareness.h"

std::string getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    struct std::tm timeInfo;
    localtime_s(&timeInfo, &now);
    char timestamp[64];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeInfo);
    return std::string(timestamp);
}
void logMessage(const std::string& Logs)
{
    std::ofstream logfile("ApplicationLog", std::ios::app);
    if (logfile.is_open())
    {
        logfile << getCurrentTimestamp() << ": " << Logs << std::endl;
        logfile.close();
    }
    else
    {
        std::cerr << "Failed to open log file!" << std::endl;
    }
}


