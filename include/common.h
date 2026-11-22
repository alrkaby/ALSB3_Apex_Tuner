#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <chrono>

#define APP_TITLE_ID        "APEX00001"
#define APP_NAME            "OrbisApex"
#define APP_VERSION         "01.00"
#define APP_SUBTITLE        "Apex Gaming Mode & System Tuner"

// System paths in GoldHEN / Orbis environment
#define PATH_APP_DB         "/system_data/priv/mms/app.db"
#define PATH_APP_DB_BAK     "/system_data/priv/mms/app.db.apex_bak"
#define PATH_USER_DOWNLOAD  "/user/download"
#define PATH_DATA_PKG       "/data/pkg"
#define PATH_APEX_DATA      "/data/OrbisApex"
#define PATH_SYSTEM_LOGS    "/system_data/priv/log"
#define PATH_ERROR_LOGS     "/user/home"

// Logging macro
#define APEX_TAG "[OrbisApex] "
#define LOGI(...) printf(APEX_TAG __VA_ARGS__); printf("\n")
#define LOGE(...) fprintf(stderr, APEX_TAG "ERROR: " __VA_ARGS__); fprintf(stderr, "\n")

struct GameItem {
    std::string titleId;
    std::string name;
    std::string iconPath;
    uint64_t sizeBytes;
    bool isRunning;
};

struct SystemMetrics {
    int socTempCelsius;
    int fanSpeedPercent;
    uint64_t freeRamBytes;
    uint64_t totalRamBytes;
    uint64_t freeStorageBytes;
    uint64_t totalStorageBytes;
    std::string activeFocusedGame;
    bool gameModeActive;
};
