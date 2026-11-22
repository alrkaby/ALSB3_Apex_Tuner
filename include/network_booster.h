#pragma once

#include "common.h"
#include <string>

class NetworkBooster {
public:
    NetworkBooster();
    ~NetworkBooster();

    // 1. Set Gaming DNS (1.1.1.1 & 8.8.8.8)
    bool applyGamingDns();

    // 2. MTU Packet Tuning (1500 / 1472 for zero fragmentation)
    bool optimizeMtuSettings();

    // 3. Flush DNS Cache & TCP Window Tuning
    bool flushAndTuneNetworkStack();
};
