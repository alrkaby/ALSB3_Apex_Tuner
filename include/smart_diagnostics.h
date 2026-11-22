#pragma once

#include "common.h"
#include <string>

class SmartDiagnostics {
public:
    SmartDiagnostics();
    ~SmartDiagnostics();

    // 1. Comprehensive System Health Probe & Diagnostic Scan
    DiagnosticReport runFullSystemScan();

    // 2. Intelligent Auto-Tune (Applies the optimal configuration automatically)
    bool applyOptimalTuning(const DiagnosticReport& report);

    const DiagnosticReport& getLastReport() const { return lastReport; }

private:
    DiagnosticReport lastReport{};

    ConsoleHardwareModel detectConsoleModel();
    int analyzeDatabaseFragmentation();
    std::pair<std::string, int> probeFastestDns();
};
