#pragma once

#include "common.h"
#include <string>

class SysTuner {
public:
    SysTuner();
    ~SysTuner();

    // 1. Database Acceleration
    bool optimizeDatabases();

    // 2. Storage & Junk Cleaner
    uint64_t purgeSystemJunk();

    // 3. Thermal Guard & Fan Control
    bool setFanThreshold(int targetCelsius);

    // 4. Metrics Retrieval
    SystemMetrics getSystemMetrics(bool gameModeActive, const std::string& activeGame);

private:
    uint64_t calculateDirectorySize(const std::string& dirPath);
};
