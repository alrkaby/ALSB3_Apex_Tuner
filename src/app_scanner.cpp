#include "app_scanner.h"
#include <sqlite3.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <set>
#include <cstring>

AppScanner::AppScanner() {}
AppScanner::~AppScanner() {}

static bool fileExists(const std::string& path) {
    struct stat buffer{};
    return (stat(path.c_str(), &buffer) == 0);
}

static uint32_t readU32LE(const unsigned char* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

static uint16_t readU16LE(const unsigned char* p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

static std::string readParamSfoField(const std::string& sfoPath, const std::string& wantedKey) {
    std::ifstream file(sfoPath, std::ios::binary);
    if (!file.is_open()) return "";

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (data.size() < 20 || data[0] != 'P' || data[1] != 'S' || data[2] != 'F' || data[3] != 0) return "";

    const uint32_t keyOffset = readU32LE(&data[8]);
    const uint32_t dataOffset = readU32LE(&data[12]);
    const uint32_t count = readU32LE(&data[16]);
    const size_t indexStart = 20;

    if (keyOffset >= data.size() || dataOffset >= data.size() || count > 4096 || indexStart + count * 16 > data.size()) return "";

    for (uint32_t i = 0; i < count; ++i) {
        const unsigned char* entry = &data[indexStart + i * 16];
        const uint16_t keyRel = readU16LE(entry);
        const uint32_t length = readU32LE(entry + 4);
        const uint32_t valueRel = readU32LE(entry + 12);

        size_t keyPos = static_cast<size_t>(keyOffset) + keyRel;
        size_t valuePos = static_cast<size_t>(dataOffset) + valueRel;

        if (keyPos >= data.size() || valuePos >= data.size()) continue;

        std::string key = reinterpret_cast<const char*>(&data[keyPos]);
        if (key == wantedKey) {
            size_t actualLen = std::min(static_cast<size_t>(length), data.size() - valuePos);
            std::string val(reinterpret_cast<const char*>(&data[valuePos]), actualLen);
            // Trim null terminators
            val.erase(std::find(val.begin(), val.end(), '\0'), val.end());
            return val;
        }
    }
    return "";
}

bool AppScanner::scanInstalledGames() {
    games.clear();
    std::set<std::string> seen;

    // 1. Primary Discovery: Query app.db with broad table discovery
    sqlite3* db = nullptr;
    if (sqlite3_open_v2(PATH_APP_DB, &db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK) {
        // Collect all tables in app.db that might contain titles
        std::vector<std::string> targetTables;
        sqlite3_stmt* masterStmt = nullptr;
        const char* masterQuery = "SELECT name FROM sqlite_master WHERE type='table' AND (name LIKE 'tbl_app%' OR name LIKE '%appinfo%');";
        if (sqlite3_prepare_v2(db, masterQuery, -1, &masterStmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(masterStmt) == SQLITE_ROW) {
                const unsigned char* tblName = sqlite3_column_text(masterStmt, 0);
                if (tblName) {
                    targetTables.push_back(reinterpret_cast<const char*>(tblName));
                }
            }
            sqlite3_finalize(masterStmt);
        }

        // Add standard table names if none matched
        if (targetTables.empty()) {
            targetTables.push_back("tbl_appinfo");
            targetTables.push_back("tbl_appbrowse_0");
            targetTables.push_back("tbl_appbrowse_10000000");
        }

        for (const auto& tbl : targetTables) {
            std::string q = "SELECT titleId, titleName FROM " + tbl + " WHERE titleId NOT LIKE 'NPXS%' ORDER BY titleName ASC;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, q.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* tid = sqlite3_column_text(stmt, 0);
                    const unsigned char* name = sqlite3_column_text(stmt, 1);
                    if (!tid) continue;

                    std::string titleId = reinterpret_cast<const char*>(tid);
                    if (titleId.empty() || titleId == APP_TITLE_ID || !seen.insert(titleId).second) continue;

                    std::string titleName = (name && *name) ? reinterpret_cast<const char*>(name) : titleId;

                    GameItem item;
                    item.titleId = titleId;
                    item.name = titleName;
                    item.isRunning = false;
                    item.sizeBytes = 0;

                    // Resolve icon from internal or external storage
                    std::string iconCandidate = "/user/appmeta/" + titleId + "/icon0.png";
                    if (!fileExists(iconCandidate)) iconCandidate = "/mnt/ext0/user/appmeta/" + titleId + "/icon0.png";
                    if (!fileExists(iconCandidate)) iconCandidate = "/system_data/priv/appmeta/" + titleId + "/icon0.png";
                    if (!fileExists(iconCandidate)) iconCandidate = "/data/appmeta/" + titleId + "/icon0.png";
                    item.iconPath = iconCandidate;

                    games.push_back(item);
                }
                sqlite3_finalize(stmt);
            }
        }
        sqlite3_close(db);
    }

    // 2. Secondary Resilient Discovery: Scan filesystem (Internal & External USB HDD)
    const char* metaRoots[] = {
        "/user/appmeta",
        "/mnt/ext0/user/appmeta",
        "/system_data/priv/appmeta",
        "/data/appmeta",
        "/user/app",
        "/mnt/ext0/user/app"
    };

    for (const char* root : metaRoots) {
        DIR* dir = opendir(root);
        if (!dir) continue;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;
            std::string folderName = entry->d_name;

            // Only inspect CUSA / Title ID folders
            if (folderName.size() < 9 || folderName == APP_TITLE_ID || folderName.rfind("NPXS", 0) == 0) continue;
            if (!seen.insert(folderName).second) continue;

            std::string sfoPath = std::string(root) + "/" + folderName + "/param.sfo";
            if (!fileExists(sfoPath)) {
                sfoPath = std::string(root) + "/" + folderName + "/sce_sys/param.sfo";
            }

            std::string titleId = readParamSfoField(sfoPath, "TITLE_ID");
            if (titleId.empty()) titleId = folderName;

            std::string titleName = readParamSfoField(sfoPath, "TITLE");
            if (titleName.empty()) titleName = titleId;

            GameItem item;
            item.titleId = titleId;
            item.name = titleName;
            item.isRunning = false;
            item.sizeBytes = 0;

            std::string icon = std::string(root) + "/" + folderName + "/icon0.png";
            if (!fileExists(icon)) icon = std::string(root) + "/" + folderName + "/sce_sys/icon0.png";
            item.iconPath = icon;

            games.push_back(item);
        }
        closedir(dir);
    }

    std::sort(games.begin(), games.end(), [](const GameItem& a, const GameItem& b) {
        return a.name < b.name;
    });

    LOGI("ALSB3 DEV Scanner: Successfully indexed %zu installed games", games.size());
    return !games.empty();
}

const GameItem* AppScanner::findGame(const std::string& titleId) const {
    for (const auto& g : games) {
        if (g.titleId == titleId) return &g;
    }
    return nullptr;
}
