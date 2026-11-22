#pragma once

#include "common.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class GameModeEngine {
public:
    GameModeEngine();
    ~GameModeEngine();

    bool activateGameMode(const std::string& titleId);
    void deactivateGameMode();

    bool isGameModeActive() const { return gameModeRunning; }
    std::string getActiveTitleId() const;
    uint32_t getElapsedMinutes() const;
    uint32_t getCleanCount() const { return totalHourlyCleans; }

    // Manual or scheduled trigger
    bool runIntelligentRamPurge(const std::string& exemptTitleId);

private:
    std::atomic<bool> gameModeRunning{false};
    std::string currentFocusedTitleId;
    std::atomic<uint32_t> totalHourlyCleans{0};
    std::chrono::steady_clock::time_point launchTimestamp;

    mutable std::mutex modeMutex;
    std::thread watchdogThread;

    void watchdogLoop();
    void logPurgeEvent(const std::string& message);
};
