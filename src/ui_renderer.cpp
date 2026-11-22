#include "ui_renderer.h"
#include <algorithm>

UiRenderer::UiRenderer() {}
UiRenderer::~UiRenderer() { shutdown(); }

bool UiRenderer::init(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        LOGE("TTF_Init failed: %s", TTF_GetError());
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    window = SDL_CreateWindow("ALSB3 Apex Tuner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    const char* fontPaths[] = {
        "/app0/assets/fonts/font.ttf",
        "/data/ALSB3_Apex/font.ttf",
        "assets/fonts/font.ttf"
    };

    for (const char* p : fontPaths) {
        fontTitle = TTF_OpenFont(p, 30);
        if (fontTitle) {
            fontRegular = TTF_OpenFont(p, 20);
            fontSmall = TTF_OpenFont(p, 15);
            break;
        }
    }

    LOGI("ALSB3 DEV UI Renderer successfully loaded");
    return true;
}

void UiRenderer::shutdown() {
    for (auto& pair : textureCache) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    textureCache.clear();

    if (fontTitle) TTF_CloseFont(fontTitle);
    if (fontRegular) TTF_CloseFont(fontRegular);
    if (fontSmall) TTF_CloseFont(fontSmall);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void UiRenderer::beginFrame() {
    SDL_SetRenderDrawColor(renderer, 10, 13, 18, 255); // Premium Obsidian
    SDL_RenderClear(renderer);
}

void UiRenderer::endFrame() {
    SDL_RenderPresent(renderer);
}

void UiRenderer::fillRect(int x, int y, int w, int h, ColorRGBA color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void UiRenderer::drawRect(int x, int y, int w, int h, ColorRGBA color, int thickness) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int t = 0; t < thickness; ++t) {
        SDL_Rect rect{x - t, y - t, w + t * 2, h + t * 2};
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void UiRenderer::drawText(const std::string& text, int x, int y, int fontSize, ColorRGBA color) {
    TTF_Font* f = fontRegular;
    if (fontSize >= 26) f = fontTitle;
    else if (fontSize <= 16) f = fontSmall;

    if (!f || text.empty()) return;

    SDL_Color sdlColor{color.r, color.g, color.b, color.a};
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), sdlColor);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        SDL_Rect dst{x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

SDL_Texture* UiRenderer::loadTexture(const std::string& path) {
    if (textureCache.find(path) != textureCache.end()) return textureCache[path];
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    textureCache[path] = tex;
    return tex;
}

void UiRenderer::drawImage(const std::string& imagePath, int x, int y, int w, int h) {
    SDL_Texture* tex = loadTexture(imagePath);
    if (tex) {
        SDL_Rect dst{x, y, w, h};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
    } else {
        fillRect(x, y, w, h, {26, 31, 38, 255});
        drawRect(x, y, w, h, {45, 52, 64, 255}, 1);
    }
}

void UiRenderer::drawHeader(const SystemMetrics& metrics) {
    fillRect(0, 0, 1920, 90, {18, 22, 30, 255});
    fillRect(0, 88, 1920, 2, {0, 240, 255, 255}); // Cyan Accent

    drawText("ALSB3 APEX TUNER", 50, 18, 30, {0, 240, 255, 255});
    drawText("BY ALSB3 DEV  |  PS4 GOD-TIER OPTIMIZER & NETWORK OVERDRIVE", 52, 55, 14, {255, 215, 0, 255});

    drawText("DEVELOPER: ALSB3 DEV", 1620, 22, 16, {255, 215, 0, 255});
    drawText("VERSION: 02.00 GOD-TIER", 1620, 52, 14, {139, 148, 158, 255});

    fillRect(700, 20, 200, 48, {28, 33, 44, 255});
    drawRect(700, 20, 200, 48, {48, 54, 61, 255}, 1);
    drawText("TEMP: " + std::to_string(metrics.socTempCelsius) + " C", 720, 32, 17, {0, 255, 157, 255});

    uint64_t ramMb = metrics.freeRamBytes / (1024 * 1024);
    fillRect(915, 20, 220, 48, {28, 33, 44, 255});
    drawRect(915, 20, 220, 48, {48, 54, 61, 255}, 1);
    drawText("RAM: " + std::to_string(ramMb) + " MB FREE", 930, 32, 17, {0, 240, 255, 255});

    if (metrics.gameModeActive) {
        fillRect(1150, 20, 440, 48, {45, 35, 10, 255});
        drawRect(1150, 20, 440, 48, {255, 215, 0, 255}, 2);
        drawText("ALSB3 GUARD: " + metrics.activeFocusedGame, 1165, 32, 17, {255, 215, 0, 255});
    }
}

void UiRenderer::drawDiagnosticBanner(const DiagnosticReport& report, const NetworkStressReport& netReport) {
    // 1. Hardware Diagnostic Banner
    fillRect(50, 105, 1820, 42, {22, 27, 36, 255});
    drawRect(50, 105, 1820, 42, {0, 240, 255, 255}, 1);
    std::string diagText = "[ALSB3 SYSTEM AUTO-SCAN]  Model: " + (report.modelName.empty() ? "PS4" : report.modelName) +
                           "  |  Fan: " + std::to_string(report.optimalFanThreshold) + "C" +
                           "  |  DB Frag: " + std::to_string(report.dbFragmentationPercent) + "%" +
                           "  |  Profile: " + (report.recommendedProfile.empty() ? "Overdrive" : report.recommendedProfile);
    drawText(diagText, 70, 117, 15, {201, 209, 217, 255});

    // 2. Deep Network Stress Benchmark Banner
    fillRect(50, 155, 1820, 42, {20, 35, 30, 255}); // Emerald dark
    drawRect(50, 155, 1820, 42, {0, 255, 157, 255}, 1); // Emerald accent
    std::string netText = "[ALSB3 NETWORK BENCHMARK]  Interface: " + (netReport.interfaceName.empty() ? "eth0" : netReport.interfaceName) +
                          "  |  Ping: " + std::to_string(netReport.lowestLatencyMs) + "ms" +
                          "  |  Jitter: " + std::to_string(netReport.lowestJitterMs) + "ms" +
                          "  |  Zero-Frag MTU: " + std::to_string(netReport.optimalMtu) +
                          "  |  DNS: " + (netReport.primaryDns.empty() ? "1.1.1.1" : netReport.primaryDns) +
                          "  |  QoS Priority: " + (netReport.alsb3QosStatus.empty() ? "Active" : netReport.alsb3QosStatus);
    drawText(netText, 70, 167, 15, {0, 255, 157, 255});
}

void UiRenderer::drawGameGrid(const std::vector<GameItem>& games, int selectedIndex, bool isGameModeActive, const std::string& activeTitleId) {
    drawText("INSTALLED LIBRARY (SELECT A GAME TO FOCUS IN ALSB3 APEX MODE)", 50, 215, 19, {240, 246, 252, 255});

    if (games.empty()) {
        fillRect(50, 245, 1820, 200, {22, 27, 34, 255});
        drawText("Scanning /system_data/priv/mms/app.db for games...", 80, 310, 22, {139, 148, 158, 255});
        return;
    }

    const int startY = 245;
    const int itemHeight = 100;
    const int maxVisible = 6;
    int scrollOffset = std::max(0, selectedIndex - (maxVisible - 1));

    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)games.size(); ++i) {
        int idx = i + scrollOffset;
        const auto& game = games[idx];
        bool isSelected = (idx == selectedIndex);
        bool isFocused = (isGameModeActive && game.titleId == activeTitleId);

        int y = startY + i * (itemHeight + 10);

        ColorRGBA bg = isSelected ? ColorRGBA{32, 44, 62, 255} : ColorRGBA{20, 24, 32, 255};
        fillRect(50, y, 1820, itemHeight, bg);

        if (isFocused) {
            drawRect(50, y, 1820, itemHeight, {255, 215, 0, 255}, 3);
        } else if (isSelected) {
            drawRect(50, y, 1820, itemHeight, {0, 240, 255, 255}, 2);
        } else {
            drawRect(50, y, 1820, itemHeight, {45, 52, 64, 255}, 1);
        }

        drawImage(game.iconPath, 65, y + 6, 88, 88);
        drawText(game.name, 175, y + 18, 23, isSelected ? ColorRGBA{255, 255, 255, 255} : ColorRGBA{201, 209, 217, 255});
        drawText("TITLE ID: " + game.titleId + "  |  OPTIMIZED BY ALSB3 DEV", 175, y + 54, 15, {139, 148, 158, 255});

        if (isFocused) {
            fillRect(1560, y + 28, 280, 42, {255, 215, 0, 40});
            drawRect(1560, y + 28, 280, 42, {255, 215, 0, 255}, 1);
            drawText("ALSB3 APEX FOCUS ACTIVE", 1585, y + 40, 16, {255, 215, 0, 255});
        }
    }
}

void UiRenderer::drawFooter(const std::string& statusMessage) {
    fillRect(0, 995, 1920, 85, {18, 22, 30, 255});
    fillRect(0, 995, 1920, 2, {48, 54, 61, 255});

    drawText("alsb3 dev  >>  " + statusMessage, 50, 1025, 17, {0, 240, 255, 255});

    std::string controls = "[DPAD-RIGHT] Deep Net Stress Test  |  [L1+R1] Full Auto-Tune  |  [R1+X] GOD MODE";
    drawText(controls, 880, 1025, 16, {255, 215, 0, 255});
}
