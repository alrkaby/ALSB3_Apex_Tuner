#include "app_scanner.h"
#include <sqlite3.h>
#include <sys/stat.h>
#include <algorithm>

AppScanner::AppScanner() {}
AppScanner::~AppScanner() {}

static bool fileExists(const std::string& path) {
    struct stat buffer{};
    return (stat(path.c_str(), &buffer) == 0);
}

bool AppScanner::scanInstalledGames() {
    games.clear();

    sqlite3* db = nullptr;
    if (sqlite3_open_v2(PATH_APP_DB, &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        LOGE("Failed to open app.db for game scan");
        return false;
    }

    // Query installed application records (tbl_appbrowse_*)
    const char* query = "SELECT titleId, titleName, uiCategory FROM tbl_appbrowse_0 "
                        "WHERE uiCategory IN ('gd', 'gdc', 'game') "
                        "ORDER BY titleName COLLATE NOCASE ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        LOGE("Failed to prepare game scan query: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* tid = sqlite3_column_text(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);

        if (!tid || !name) continue;

        std::string titleId = reinterpret_cast<const char*>(tid);
        std::string titleName = reinterpret_cast<const char*>(name);

        // Filter system apps, self, and hidden containers
        if (titleId == APP_TITLE_ID || titleId.rfind("NPXS", 0) == 0) continue;

        GameItem item;
        item.titleId = titleId;
        item.name = titleName;
        item.isRunning = false;
        item.sizeBytes = 0;

        // Resolve icon0.png from appmeta directories
        std::string iconCandidate = "/user/appmeta/" + titleId + "/icon0.png";
        if (!fileExists(iconCandidate)) {
            iconCandidate = "/system_data/priv/appmeta/" + titleId + "/icon0.png";
        }
        if (!fileExists(iconCandidate)) {
            iconCandidate = "/data/appmeta/" + titleId + "/icon0.png";
        }
        item.iconPath = iconCandidate;

        games.push_back(item);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    LOGI("Discovered %zu installed games in library", games.size());
    return !games.empty();
}

const GameItem* AppScanner::findGame(const std::string& titleId) const {
    for (const auto& g : games) {
        if (g.titleId == titleId) return &g;
    }
    return nullptr;
}
