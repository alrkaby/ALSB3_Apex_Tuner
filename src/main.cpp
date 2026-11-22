#include "common.h"
#include "app_scanner.h"
#include "game_mode.h"
#include "sys_tuner.h"
#include "game_overclock.h"
#include "network_booster.h"
#include "ui_renderer.h"
#include <SDL2/SDL.h>

#ifdef __ORBIS__
#include <orbis/libkernel.h>
#endif

int main(int argc, char* argv[]) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    LOGI("Starting %s v%s [GOD-TIER EDITION]...", APP_NAME, APP_VERSION);

    AppScanner scanner;
    GameModeEngine gameMode;
    SysTuner tuner;
    GameOverclock overclock;
    NetworkBooster netBooster;
    UiRenderer ui;

    if (!ui.init(1920, 1080)) {
        LOGE("Failed to initialize UI renderer");
        return -1;
    }

    // Initial scan of installed games
    scanner.scanInstalledGames();
    const auto& games = scanner.getGames();

    int selectedGameIndex = 0;
    bool running = true;
    std::string statusText = "Ready - [X] Apex Mode  [R1+X] GOD MODE (All Tweaks+60FPS+Ping Boost)";

    SDL_GameController* controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                LOGI("Connected DualShock 4 controller: %s", SDL_GameControllerName(controller));
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

                if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP && selectedGameIndex > 0) {
                    --selectedGameIndex;
                } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN && selectedGameIndex + 1 < (int)games.size()) {
                    ++selectedGameIndex;
                } else if (btn == SDL_CONTROLLER_BUTTON_A) { // Cross (X) on PS4
                    if (!games.empty() && selectedGameIndex < (int)games.size()) {
                        const auto& target = games[selectedGameIndex];
                        if (r1Held) {
                            // GOD MODE TRIGGER: Execute every single tweak + Overclock + 60FPS + Launch
                            statusText = "ENGAGING GOD MODE: Tuning DB, Purging Junk, DNS Boost, 60FPS Patch & Launching " + target.name + "...";
                            tuner.optimizeDatabases();
                            tuner.purgeSystemJunk();
                            tuner.setFanThreshold(60);
                            netBooster.applyGamingDns();
                            netBooster.optimizeMtuSettings();
                            netBooster.flushAndTuneNetworkStack();
                            overclock.applyMaxPerformanceClocks();
                            overclock.inject60FpsPatch(target.titleId);
                            overclock.optimizeFramePacing(target.titleId);
                            gameMode.activateGameMode(target.titleId);
                            statusText = "GOD MODE ACTIVE: " + target.name + " running with 1-Hour RAM Guard!";
                        } else {
                            // Standard Apex Focus
                            statusText = "Engaging Apex Mode on " + target.name + "... Standby RAM purged!";
                            overclock.inject60FpsPatch(target.titleId);
                            gameMode.activateGameMode(target.titleId);
                        }
                    }
                } else if (btn == SDL_CONTROLLER_BUTTON_X) { // Square on PS4
                    statusText = "Compacting and optimizing app.db database (VACUUM)...";
                    bool res = tuner.optimizeDatabases();
                    statusText = res ? "Database optimization completed! XMB is now ultra-fast." : "Database optimization failed.";
                } else if (btn == SDL_CONTROLLER_BUTTON_Y) { // Triangle on PS4
                    statusText = "Purging cache, crash dumps, and orphaned downloads...";
                    uint64_t freed = tuner.purgeSystemJunk();
                    statusText = "Purged junk files! Reclaimed " + std::to_string(freed / (1024 * 1024)) + " MB of storage.";
                } else if (btn == SDL_CONTROLLER_BUTTON_START) { // Options on PS4
                    tuner.setFanThreshold(60);
                    netBooster.applyGamingDns();
                    statusText = "Thermal Guard (60C) & Low-Ping Gaming DNS (1.1.1.1) Applied!";
                } else if (btn == SDL_CONTROLLER_BUTTON_B) { // Circle on PS4
                    scanner.scanInstalledGames();
                    statusText = "Library refreshed.";
                }
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;
                if (key == SDLK_UP && selectedGameIndex > 0) --selectedGameIndex;
                if (key == SDLK_DOWN && selectedGameIndex + 1 < (int)games.size()) ++selectedGameIndex;
                if (key == SDLK_RETURN && !games.empty()) {
                    const auto& target = games[selectedGameIndex];
                    gameMode.activateGameMode(target.titleId);
                    statusText = "Engaged Apex Mode on " + target.name;
                }
                if (key == SDLK_ESCAPE) running = false;
            }
        }

        // Live telemetry
        SystemMetrics metrics = tuner.getSystemMetrics(gameMode.isGameModeActive(), gameMode.getActiveTitleId());

        // Render Frame
        ui.beginFrame();
        ui.drawHeader(metrics);
        ui.drawGameGrid(scanner.getGames(), selectedGameIndex, gameMode.isGameModeActive(), gameMode.getActiveTitleId());
        ui.drawFooter(statusText);
        ui.endFrame();

        SDL_Delay(16); // 60 FPS cap
    }

    if (controller) {
        SDL_GameControllerClose(controller);
    }
    ui.shutdown();
    LOGI("OrbisApex terminated gracefully");
    return 0;
}
