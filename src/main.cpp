#include "common.h"
#include "app_scanner.h"
#include "game_mode.h"
#include "sys_tuner.h"
#include "game_overclock.h"
#include "network_booster.h"
#include "smart_diagnostics.h"
#include "ui_renderer.h"
#include <SDL2/SDL.h>

#ifdef __ORBIS__
#include <orbis/libkernel.h>
#endif

int main(int argc, char* argv[]) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    LOGI("=================================================");
    LOGI("   %s v%s", APP_NAME, APP_VERSION);
    LOGI("   Developed by: %s", APP_DEVELOPER);
    LOGI("   %s", APP_SIGNATURE);
    LOGI("=================================================");

    AppScanner scanner;
    GameModeEngine gameMode;
    SysTuner tuner;
    GameOverclock overclock;
    NetworkBooster netBooster;
    SmartDiagnostics diagnostics;
    UiRenderer ui;

    if (!ui.init(1920, 1080)) {
        LOGE("Failed to initialize UI renderer");
        return -1;
    }

    // 1. Initial automated diagnostic scan
    DiagnosticReport initialReport = diagnostics.runFullSystemScan();

    // 2. Initial network benchmark
    NetworkStressReport netReport = netBooster.runDeepNetworkBenchmark();
    netBooster.applyUltimateNetworkTuning(netReport);

    // 3. Scan installed games library
    scanner.scanInstalledGames();
    const auto& games = scanner.getGames();

    int selectedGameIndex = 0;
    bool running = true;
    std::string statusText = "alsb3 dev >> Network Overdrive Active! [DPAD-RIGHT] Re-Test Ping  [L1+R1] Auto-Tune  [R1+X] GOD MODE";

    SDL_GameController* controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                LOGI("Connected DualShock 4: %s", SDL_GameControllerName(controller));
                break;
            }
        }
    }

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
            } else if (ev.type == SDL_CONTROLLERBUTTONDOWN) {
                Uint8 btn = ev.cbutton.button;
                bool r1Held = controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
                bool l1Held = controller && SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);

                // D-PAD RIGHT: DEEP NETWORK STRESS BENCHMARK & AUTO-BOOST
                if (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    statusText = "alsb3 dev >> Running Deep Network Stress Benchmark & Probing Gaming Backbones...";
                    netReport = netBooster.runDeepNetworkBenchmark();
                    netBooster.applyUltimateNetworkTuning(netReport);
                    statusText = "alsb3 dev >> Network Overdrive Locked! Fastest DNS: " + netReport.primaryDns +
                                 " (" + std::to_string(netReport.lowestLatencyMs) + "ms) | MTU: " + std::to_string(netReport.optimalMtu);
                } else if (l1Held && r1Held) {
                    // SMART AUTO-TUNE (L1 + R1)
                    statusText = "alsb3 dev >> Running Deep Hardware & Storage Diagnostic...";
                    DiagnosticReport report = diagnostics.runFullSystemScan();
                    diagnostics.applyOptimalTuning(report);
                    netReport = netBooster.runDeepNetworkBenchmark();
                    netBooster.applyUltimateNetworkTuning(netReport);
                    overclock.applyMaxPerformanceClocks();
                    statusText = "alsb3 dev >> Full System & Network Auto-Tuning Successfully Deployed!";
                } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP && selectedGameIndex > 0) {
                    --selectedGameIndex;
                } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN && selectedGameIndex + 1 < (int)games.size()) {
                    ++selectedGameIndex;
                } else if (btn == SDL_CONTROLLER_BUTTON_A) { // Cross (X) on PS4
                    if (!games.empty() && selectedGameIndex < (int)games.size()) {
                        const auto& target = games[selectedGameIndex];
                        if (r1Held) {
                            // GOD MODE TRIGGER (R1 + X)
                            statusText = "alsb3 dev >> GOD MODE: Tuning DB, MTU 1500, Gaming DNS, 60FPS Patch & Launching " + target.name + "...";
                            tuner.optimizeDatabases();
                            tuner.purgeSystemJunk();
                            tuner.setFanThreshold(initialReport.optimalFanThreshold);
                            netBooster.applyUltimateNetworkTuning(netReport);
                            overclock.applyMaxPerformanceClocks();
                            overclock.inject60FpsPatch(target.titleId);
                            overclock.optimizeFramePacing(target.titleId);
                            gameMode.activateGameMode(target.titleId);
                            statusText = "alsb3 dev >> GOD MODE ENGAGED: " + target.name + " Running with Hourly RAM Guard!";
                        } else {
                            // Standard Apex Focus
                            statusText = "alsb3 dev >> Engaging Apex Focus on " + target.name + "... Standby RAM purged!";
                            overclock.inject60FpsPatch(target.titleId);
                            gameMode.activateGameMode(target.titleId);
                        }
                    }
                } else if (btn == SDL_CONTROLLER_BUTTON_X) { // Square on PS4
                    statusText = "alsb3 dev >> Compacting and optimizing app.db database (VACUUM)...";
                    bool res = tuner.optimizeDatabases();
                    statusText = res ? "alsb3 dev >> Database optimization completed! XMB is now ultra-fast." : "Database optimization failed.";
                } else if (btn == SDL_CONTROLLER_BUTTON_Y) { // Triangle on PS4
                    statusText = "alsb3 dev >> Purging cache, crash dumps, and orphaned downloads...";
                    uint64_t freed = tuner.purgeSystemJunk();
                    statusText = "alsb3 dev >> Purged junk! Reclaimed " + std::to_string(freed / (1024 * 1024)) + " MB of storage.";
                } else if (btn == SDL_CONTROLLER_BUTTON_START) { // Options on PS4
                    tuner.setFanThreshold(initialReport.optimalFanThreshold);
                    netBooster.applyUltimateNetworkTuning(netReport);
                    statusText = "alsb3 dev >> Thermal Guard & Network Overdrive Re-Applied!";
                } else if (btn == SDL_CONTROLLER_BUTTON_B) { // Circle on PS4
                    scanner.scanInstalledGames();
                    statusText = "alsb3 dev >> Library refreshed.";
                }
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;
                if (key == SDLK_RIGHT) {
                    netReport = netBooster.runDeepNetworkBenchmark();
                    netBooster.applyUltimateNetworkTuning(netReport);
                    statusText = "alsb3 dev >> Network Stress Test Complete! Best DNS: " + netReport.primaryDns;
                }
                if (key == SDLK_UP && selectedGameIndex > 0) --selectedGameIndex;
                if (key == SDLK_DOWN && selectedGameIndex + 1 < (int)games.size()) ++selectedGameIndex;
                if (key == SDLK_RETURN && !games.empty()) {
                    const auto& target = games[selectedGameIndex];
                    gameMode.activateGameMode(target.titleId);
                    statusText = "alsb3 dev >> Engaged Apex Mode on " + target.name;
                }
                if (key == SDLK_ESCAPE) running = false;
            }
        }

        // Live telemetry
        SystemMetrics metrics = tuner.getSystemMetrics(gameMode.isGameModeActive(), gameMode.getActiveTitleId());
        metrics.alsb3Signature = APP_DEVELOPER;

        // Render Frame
        ui.beginFrame();
        ui.drawHeader(metrics);
        ui.drawDiagnosticBanner(diagnostics.getLastReport(), netBooster.getLastReport());
        ui.drawGameGrid(scanner.getGames(), selectedGameIndex, gameMode.isGameModeActive(), gameMode.getActiveTitleId());
        ui.drawFooter(statusText);
        ui.endFrame();

        SDL_Delay(16); // 60 FPS cap
    }

    if (controller) {
        SDL_GameControllerClose(controller);
    }
    ui.shutdown();
    LOGI("ALSB3 Apex Tuner terminated gracefully");
    return 0;
}
