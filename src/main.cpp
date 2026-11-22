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
#include <orbis/Sysmodule.h>
#include <orbis/libkernel.h>
#endif

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    setvbuf(stdout, nullptr, _IONBF, 0);

#ifdef __ORBIS__
    sceSysmoduleLoadModule(ORBIS_SYSMODULE_FREETYPE_OL);
    sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
    sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_PAD);
    sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_VIDEO_OUT);
    sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_AUDIOOUT);
    sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET);
#endif

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

    NavFocusArea currentFocus = FOCUS_ACTIONS;
    int selectedButtonIndex = 0;
    int selectedGameIndex = 0;
    bool running = true;
    std::string statusText = "Ready! Select an on-screen Action Card or choose a Game to focus.";

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
        const auto& games = scanner.getGames();
        if (selectedGameIndex >= (int)games.size() && !games.empty()) {
            selectedGameIndex = (int)games.size() - 1;
        }

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
            } else if (ev.type == SDL_CONTROLLERBUTTONDOWN) {
                Uint8 btn = ev.cbutton.button;

                // NAVIGATION: D-PAD UP
                if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP) {
                    if (currentFocus == FOCUS_GAMES) {
                        if (selectedGameIndex > 0) {
                            --selectedGameIndex;
                        } else {
                            currentFocus = FOCUS_ACTIONS;
                        }
                    }
                }
                // NAVIGATION: D-PAD DOWN
                else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
                    if (currentFocus == FOCUS_ACTIONS) {
                        if (!games.empty()) {
                            currentFocus = FOCUS_GAMES;
                        }
                    } else if (currentFocus == FOCUS_GAMES) {
                        if (selectedGameIndex + 1 < (int)games.size()) {
                            ++selectedGameIndex;
                        }
                    }
                }
                // NAVIGATION: D-PAD LEFT
                else if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
                    if (currentFocus == FOCUS_ACTIONS && selectedButtonIndex > 0) {
                        --selectedButtonIndex;
                    }
                }
                // NAVIGATION: D-PAD RIGHT
                else if (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    if (currentFocus == FOCUS_ACTIONS && selectedButtonIndex < 3) {
                        ++selectedButtonIndex;
                    }
                }
                // SELECTION: CROSS (X) BUTTON
                else if (btn == SDL_CONTROLLER_BUTTON_A) {
                    if (currentFocus == FOCUS_ACTIONS) {
                        if (selectedButtonIndex == ACTION_BTN_GOD_MODE) {
                            statusText = "Engaging GOD MODE: Database Tuning, Zero-Frag MTU, Max Clocks & Cooling...";
                            tuner.optimizeDatabases();
                            tuner.purgeSystemJunk();
                            tuner.setFanThreshold(initialReport.optimalFanThreshold > 0 ? initialReport.optimalFanThreshold : 60);
                            netBooster.applyUltimateNetworkTuning(netReport);
                            overclock.applyMaxPerformanceClocks();

                            if (!games.empty() && selectedGameIndex < (int)games.size()) {
                                const auto& target = games[selectedGameIndex];
                                overclock.inject60FpsPatch(target.titleId);
                                overclock.optimizeFramePacing(target.titleId);
                                gameMode.activateGameMode(target.titleId);
                                statusText = "GOD MODE ACTIVE: " + target.name + " Running with Hourly RAM Guard & 60FPS!";
                            } else {
                                statusText = "GOD MODE ENGAGED: Clocks Maxed, Storage Cleaned, Network Overdriven!";
                            }
                        } else if (selectedButtonIndex == ACTION_BTN_NET_BOOST) {
                            statusText = "Running Deep Network Stress Benchmark & Latency Overdrive...";
                            netReport = netBooster.runDeepNetworkBenchmark();
                            netBooster.applyUltimateNetworkTuning(netReport);
                            statusText = "Network Overdrive Locked! DNS: " + netReport.primaryDns +
                                         " (" + std::to_string(netReport.lowestLatencyMs) + "ms) | MTU: " + std::to_string(netReport.optimalMtu);
                        } else if (selectedButtonIndex == ACTION_BTN_CLEAN_VACUUM) {
                            statusText = "Compacting app.db & Purging Junk Cache Files...";
                            tuner.optimizeDatabases();
                            uint64_t freed = tuner.purgeSystemJunk();
                            statusText = "Clean & Vacuum Completed! Reclaimed " + std::to_string(freed / (1024 * 1024)) + " MB of storage.";
                        } else if (selectedButtonIndex == ACTION_BTN_THERMAL_GUARD) {
                            tuner.setFanThreshold(60);
                            statusText = "Thermal Guard Active: Fan threshold configured to 60C (Silent & Safe).";
                        }
                    } else if (currentFocus == FOCUS_GAMES) {
                        if (!games.empty() && selectedGameIndex < (int)games.size()) {
                            const auto& target = games[selectedGameIndex];
                            statusText = "Engaging Apex Focus on " + target.name + "... Standby RAM Purged!";
                            overclock.inject60FpsPatch(target.titleId);
                            overclock.optimizeFramePacing(target.titleId);
                            gameMode.activateGameMode(target.titleId);
                            statusText = "ALSB3 APEX ACTIVE: " + target.name + " Running with 1-Hour RAM Guard!";
                        }
                    }
                }
                // RESCAN: CIRCLE (O) BUTTON
                else if (btn == SDL_CONTROLLER_BUTTON_B) {
                    statusText = "Scanning system databases and storage for installed games...";
                    scanner.scanInstalledGames();
                    statusText = "Scan Complete: Indexed " + std::to_string(scanner.getGames().size()) + " games.";
                }
                // QUICK SHORTCUT: SQUARE (VACUUM)
                else if (btn == SDL_CONTROLLER_BUTTON_X) {
                    tuner.optimizeDatabases();
                    statusText = "Database optimization completed (VACUUM app.db).";
                }
                // QUICK SHORTCUT: TRIANGLE (PURGE JUNK)
                else if (btn == SDL_CONTROLLER_BUTTON_Y) {
                    uint64_t freed = tuner.purgeSystemJunk();
                    statusText = "Purged junk files! Reclaimed " + std::to_string(freed / (1024 * 1024)) + " MB.";
                }
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    running = false;
                } else if (key == SDLK_UP) {
                    if (currentFocus == FOCUS_GAMES) {
                        if (selectedGameIndex > 0) --selectedGameIndex;
                        else currentFocus = FOCUS_ACTIONS;
                    }
                } else if (key == SDLK_DOWN) {
                    if (currentFocus == FOCUS_ACTIONS && !games.empty()) currentFocus = FOCUS_GAMES;
                    else if (currentFocus == FOCUS_GAMES && selectedGameIndex + 1 < (int)games.size()) ++selectedGameIndex;
                } else if (key == SDLK_LEFT && currentFocus == FOCUS_ACTIONS && selectedButtonIndex > 0) {
                    --selectedButtonIndex;
                } else if (key == SDLK_RIGHT && currentFocus == FOCUS_ACTIONS && selectedButtonIndex < 3) {
                    ++selectedButtonIndex;
                } else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (currentFocus == FOCUS_ACTIONS && selectedButtonIndex == 0) {
                        overclock.applyMaxPerformanceClocks();
                        statusText = "GOD MODE Activated via Keyboard!";
                    } else if (currentFocus == FOCUS_GAMES && !games.empty()) {
                        gameMode.activateGameMode(games[selectedGameIndex].titleId);
                        statusText = "Focused game: " + games[selectedGameIndex].name;
                    }
                }
            }
        }

        // Live telemetry
        SystemMetrics metrics = tuner.getSystemMetrics(gameMode.isGameModeActive(), gameMode.getActiveTitleId());
        metrics.alsb3Signature = APP_DEVELOPER;

        // Render Frame
        ui.beginFrame();
        ui.drawHeader(metrics);
        ui.drawDiagnosticBanner(diagnostics.getLastReport(), netBooster.getLastReport());
        ui.drawActionButtons(selectedButtonIndex, currentFocus == FOCUS_ACTIONS);
        ui.drawGameGrid(scanner.getGames(), selectedGameIndex, gameMode.isGameModeActive(), gameMode.getActiveTitleId(), currentFocus == FOCUS_GAMES);
        ui.drawFooter(statusText, currentFocus);
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
