#pragma once

#include "common.h"
#include <string>

class GameOverclock {
public:
    GameOverclock();
    ~GameOverclock();

    // 1. Force Max Clocks (CPU 1.6GHz/2.13GHz & GPU 800MHz/911MHz)
    bool applyMaxPerformanceClocks();

    // 2. GoldHEN 60 FPS & Resolution Patch Injector
    bool inject60FpsPatch(const std::string& titleId);

    // 3. Disable VSync & Input Lag Reducer
    bool optimizeFramePacing(const std::string& titleId);
};
