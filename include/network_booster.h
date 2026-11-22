#pragma once

#include "common.h"
#include <string>
#include <vector>

struct DnsBenchmarkResult {
    std::string ip;
    std::string provider;
    int averagePingMs;
    int jitterMs;
    int packetLossPercent;
    bool isFastest;
};

struct NetworkStressReport {
    std::string interfaceName; // eth0 (LAN) or wlan0 (Wi-Fi)
    int optimalMtu;
    std::string primaryDns;
    std::string secondaryDns;
    int lowestLatencyMs;
    int lowestJitterMs;
    bool zeroPacketLoss;
    uint32_t tcpBufferSend;
    uint32_t tcpBufferRecv;
    std::string alsb3QosStatus;
    std::vector<DnsBenchmarkResult> benchmarkHistory;
};

class NetworkBooster {
public:
    NetworkBooster();
    ~NetworkBooster();

    // 1. Full Deep Network Benchmark & Stress Test
    NetworkStressReport runDeepNetworkBenchmark();

    // 2. Intelligent Auto-Application of Optimal Network Settings
    bool applyUltimateNetworkTuning(const NetworkStressReport& report);

    const NetworkStressReport& getLastReport() const { return lastReport; }

private:
    NetworkStressReport lastReport{};

    std::string detectActiveInterface();
    int discoverOptimalMtu(const std::string& iface);
    DnsBenchmarkResult benchmarkServer(const std::string& ip, const std::string& provider);
    void applyTcpKernelTweaks(uint32_t sendBuf, uint32_t recvBuf);
    void applyQosPriorityTagging();
};
