#include "network_booster.h"
#include <fstream>
#include <cstdlib>

NetworkBooster::NetworkBooster() {}
NetworkBooster::~NetworkBooster() {}

bool NetworkBooster::applyGamingDns() {
    LOGI("Applying ultra-fast low-ping Gaming DNS (Cloudflare 1.1.1.1 + Google 8.8.8.8)...");

    // Resolv.conf configuration in FreeBSD Orbis network stack
    std::ofstream resolv("/etc/resolv.conf");
    if (resolv.is_open()) {
        resolv << "nameserver 1.1.1.1\n";
        resolv << "nameserver 8.8.8.8\n";
        resolv << "nameserver 1.0.0.1\n";
        resolv.close();
    }

    // GoldHEN network settings API fallback
    std::string cmd = "curl --silent --max-time 3 -X POST http://127.0.0.1:12800/api/network "
                      "-d '{\"primary_dns\":\"1.1.1.1\",\"secondary_dns\":\"8.8.8.8\"}' 2>/dev/null || true";
    system(cmd.c_str());
    return true;
}

bool NetworkBooster::optimizeMtuSettings() {
    LOGI("Optimizing MTU size to 1500 for zero packet fragmentation...");
    system("ifconfig eth0 mtu 1500 2>/dev/null || ifconfig wlan0 mtu 1500 2>/dev/null || true");
    return true;
}

bool NetworkBooster::flushAndTuneNetworkStack() {
    LOGI("Tuning TCP buffers and flushing network sockets...");
    system("sysctl -w net.inet.tcp.sendspace=65536 2>/dev/null || true");
    system("sysctl -w net.inet.tcp.recvspace=65536 2>/dev/null || true");
    system("sysctl -w net.inet.tcp.delayed_ack=0 2>/dev/null || true"); // Disable delayed ACKs for lowest gaming ping
    return true;
}
