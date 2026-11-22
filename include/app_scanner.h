#pragma once

#include "common.h"
#include <vector>

class AppScanner {
public:
    AppScanner();
    ~AppScanner();

    bool scanInstalledGames();
    const std::vector<GameItem>& getGames() const { return games; }
    const GameItem* findGame(const std::string& titleId) const;

private:
    std::vector<GameItem> games;
};
