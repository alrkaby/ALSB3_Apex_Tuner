#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <chrono>

#define APP_TITLE_ID        "APEX00001"
#define APP_NAME            "ALSB3 Apex Tuner"
#define APP_DEVELOPER       "alsb3 dev"
#define APP_VERSION         "02.00 (God-Tier Edition)"
#define APP_SIGNATURE       "Engineered by alsb3 dev - Ultimate PS4 Performance Suite"

// System paths in GoldHEN / Orbis environment
#define PATH_APP_DB         "/system_data/priv/mms/app.db"
#define PATH_APP_DB_BAK     "/system_data/priv/mms/app.db.apex_bak"
#define PATH_USER_DOWNLOAD  "/user/download"
#define PATH_DATA_PKG       "/data/pkg"
#define PATH_APEX_DATA      "/data/ALSB3_Apex"
#define PATH_SYSTEM_LOGS    "/system_data/priv/log"
#define PATH_ERROR_LOGS     "/user/home"

// Logging macro with alsb3 dev signature
#define ALSB3_TAG "[ALSB3 DEV] "
#define LOGI(...) printf(ALSB3_TAG __VA_ARGS__); printf("\n")
#define LOGE(...) fprintf(stderr, ALSB3_TAG "ERROR: " __VA_ARGS__); fprintf(stderr, "\n")

enum ConsoleHardwareModel {
    MODEL_UNKNOWN = 0,
    MODEL_PS4_FAT,
    MODEL_PS4_SLIM,
    MODEL_PS4_PRO
};

struct GameItem {
    std::string titleId;
    std::string name;
    std::string iconPath;
    uint64_t sizeBytes;
    bool isRunning;
};

struct DiagnosticReport {
    ConsoleHardwareModel model;
    std::string modelName;
    int socTempCelsius;
    int optimalFanThreshold;
    int dbFragmentationPercent;
    uint64_t reclaimableJunkBytes;
    std::string fastestDnsServer;
    int lowestPingMs;
    bool requiresDefrag;
    bool requiresRamFlush;
    std::string recommendedProfile;
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
    std::string alsb3Signature;
};
