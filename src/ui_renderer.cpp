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
        fontTitle = TTF_OpenFont(p, 28);
        if (fontTitle) {
            fontRegular = TTF_OpenFont(p, 19);
            fontSmall = TTF_OpenFont(p, 14);
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
    else if (fontSize <= 15) f = fontSmall;

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
    fillRect(0, 0, 1920, 80, {16, 20, 28, 255});
    fillRect(0, 78, 1920, 2, {0, 240, 255, 255}); // Cyan Accent

    drawText("ALSB3 APEX TUNER", 50, 14, 28, {0, 240, 255, 255});
    drawText("BY ALSB3 DEV  |  PS4 GOD-TIER OPTIMIZER & NETWORK OVERDRIVE", 52, 48, 14, {255, 215, 0, 255});

    drawText("DEVELOPER: ALSB3 DEV", 1630, 18, 15, {255, 215, 0, 255});
    drawText("VERSION: 02.00 GOD-TIER", 1630, 44, 13, {139, 148, 158, 255});

    // Hardware Badges
    fillRect(680, 16, 190, 44, {26, 31, 42, 255});
    drawRect(680, 16, 190, 44, {48, 54, 61, 255}, 1);
    drawText("TEMP: " + std::to_string(metrics.socTempCelsius) + " C", 700, 28, 16, {0, 255, 157, 255});

    uint64_t ramMb = metrics.freeRamBytes / (1024 * 1024);
    fillRect(885, 16, 210, 44, {26, 31, 42, 255});
    drawRect(885, 16, 210, 44, {48, 54, 61, 255}, 1);
    drawText("RAM: " + std::to_string(ramMb) + " MB FREE", 900, 28, 16, {0, 240, 255, 255});

    if (metrics.gameModeActive) {
        fillRect(1110, 16, 480, 44, {45, 35, 10, 255});
        drawRect(1110, 16, 480, 44, {255, 215, 0, 255}, 2);
        drawText("ALSB3 GUARD ACTIVE: " + metrics.activeFocusedGame, 1125, 28, 16, {255, 215, 0, 255});
    }
}

void UiRenderer::drawDiagnosticBanner(const DiagnosticReport& report, const NetworkStressReport& netReport) {
    // 1. Hardware Diagnostic Banner
    fillRect(50, 92, 1820, 38, {22, 27, 36, 255});
    drawRect(50, 92, 1820, 38, {0, 240, 255, 255}, 1);
    std::string diagText = "[SYSTEM AUTO-SCAN]  Model: " + (report.modelName.empty() ? "PS4" : report.modelName) +
                           "  |  Fan: " + std::to_string(report.optimalFanThreshold) + "C" +
                           "  |  DB Frag: " + std::to_string(report.dbFragmentationPercent) + "%" +
                           "  |  Profile: " + (report.recommendedProfile.empty() ? "Overdrive" : report.recommendedProfile);
    drawText(diagText, 70, 102, 14, {201, 209, 217, 255});

    // 2. Deep Network Stress Benchmark Banner
    fillRect(50, 136, 1820, 38, {18, 32, 26, 255});
    drawRect(50, 136, 1820, 38, {0, 255, 157, 255}, 1);
    std::string netText = "[NETWORK BENCHMARK]  Interface: " + (netReport.interfaceName.empty() ? "eth0" : netReport.interfaceName) +
                          "  |  Ping: " + std::to_string(netReport.lowestLatencyMs) + "ms" +
                          "  |  Jitter: " + std::to_string(netReport.lowestJitterMs) + "ms" +
                          "  |  Zero-Frag MTU: " + std::to_string(netReport.optimalMtu) +
                          "  |  DNS: " + (netReport.primaryDns.empty() ? "1.1.1.1" : netReport.primaryDns) +
                          "  |  QoS: " + (netReport.alsb3QosStatus.empty() ? "Active" : netReport.alsb3QosStatus);
    drawText(netText, 70, 146, 14, {0, 255, 157, 255});
}

void UiRenderer::drawActionButtons(int selectedButtonIndex, bool isFocusOnButtons) {
    struct ButtonDef {
        std::string title;
        std::string subtitle;
        ColorRGBA color;
    };

    ButtonDef buttons[4] = {
        {"[X] GOD MODE", "60 FPS + Max OC + Net Overdrive", {255, 215, 0, 255}},       // Gold
        {"[X] NET STRESS & BOOST", "Probe Ping + MTU 1500 + DNS", {0, 255, 157, 255}},  // Emerald
        {"[X] CLEAN & VACUUM", "Defrag app.db + Purge Junk", {0, 240, 255, 255}},      // Cyan
        {"[X] THERMAL GUARD", "Set Fan 60C + Silent Cooling", {88, 166, 255, 255}}     // Ice Blue
    };

    const int startX = 50;
    const int startY = 186;
    const int btnWidth = 437;
    const int btnHeight = 74;
    const int gap = 24;

    for (int i = 0; i < 4; ++i) {
        int x = startX + i * (btnWidth + gap);
        bool isSelected = (i == selectedButtonIndex);

        ColorRGBA bg = isSelected && isFocusOnButtons ?
            ColorRGBA{36, 46, 62, 255} : ColorRGBA{20, 24, 33, 255};

        fillRect(x, startY, btnWidth, btnHeight, bg);

        if (isSelected && isFocusOnButtons) {
            drawRect(x, startY, btnWidth, btnHeight, buttons[i].color, 3);
            // Highlight glow bar
            fillRect(x, startY, btnWidth, 4, buttons[i].color);
            drawText(">> ACTIVE BUTTON <<", x + btnWidth - 165, startY + 12, 12, buttons[i].color);
        } else if (isSelected) {
            drawRect(x, startY, btnWidth, btnHeight, {70, 85, 110, 255}, 2);
        } else {
            drawRect(x, startY, btnWidth, btnHeight, {40, 48, 62, 255}, 1);
        }

        drawText(buttons[i].title, x + 20, startY + 12, 20, buttons[i].color);
        drawText(buttons[i].subtitle, x + 20, startY + 42, 13, {160, 172, 188, 255});
    }
}

void UiRenderer::drawGameGrid(const std::vector<GameItem>& games, int selectedIndex, bool isGameModeActive, const std::string& activeTitleId, bool isFocusOnGames) {
    drawText("INSTALLED GAMES LIBRARY  (NAVIGATE WITH DPAD & PRESS [X] TO FOCUS GAME)", 50, 275, 17, {240, 246, 252, 255});

    if (games.empty()) {
        fillRect(50, 305, 1820, 160, {22, 27, 36, 255});
        drawRect(50, 305, 1820, 160, {0, 240, 255, 255}, 1);
        drawText("[!] NO INSTALLED GAMES FOUND IN SYSTEM DATABASES OR STORAGE DIRECTORIES.", 80, 345, 20, {255, 215, 0, 255});
        drawText("PRESS [O] (CIRCLE) ON DUALSHOCK 4 TO RESCAN INTERNAL & EXTERNAL USB STORAGE", 80, 385, 16, {139, 148, 158, 255});
        return;
    }

    const int startY = 305;
    const int itemHeight = 92;
    const int maxVisible = 6;
    int scrollOffset = std::max(0, selectedIndex - (maxVisible - 1));

    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)games.size(); ++i) {
        int idx = i + scrollOffset;
        const auto& game = games[idx];
        bool isSelected = (idx == selectedIndex);
        bool isFocused = (isGameModeActive && game.titleId == activeTitleId);

        int y = startY + i * (itemHeight + 10);

        ColorRGBA bg = (isSelected && isFocusOnGames) ? ColorRGBA{32, 45, 66, 255} : ColorRGBA{18, 22, 30, 255};
        fillRect(50, y, 1820, itemHeight, bg);

        if (isFocused) {
            drawRect(50, y, 1820, itemHeight, {255, 215, 0, 255}, 3);
        } else if (isSelected && isFocusOnGames) {
            drawRect(50, y, 1820, itemHeight, {0, 240, 255, 255}, 2);
        } else if (isSelected) {
            drawRect(50, y, 1820, itemHeight, {65, 80, 105, 255}, 1);
        } else {
            drawRect(50, y, 1820, itemHeight, {35, 42, 54, 255}, 1);
        }

        drawImage(game.iconPath, 62, y + 6, 80, 80);
        drawText(game.name, 165, y + 14, 22, isSelected ? ColorRGBA{255, 255, 255, 255} : ColorRGBA{201, 209, 217, 255});
        drawText("TITLE ID: " + game.titleId + "  |  OPTIMIZED BY ALSB3 DEV", 165, y + 48, 14, {139, 148, 158, 255});

        if (isFocused) {
            fillRect(1520, y + 24, 330, 44, {255, 215, 0, 45});
            drawRect(1520, y + 24, 330, 44, {255, 215, 0, 255}, 2);
            drawText("ALSB3 APEX FOCUS ACTIVE", 1545, y + 36, 16, {255, 215, 0, 255});
        } else if (isSelected && isFocusOnGames) {
            fillRect(1520, y + 24, 330, 44, {0, 240, 255, 30});
            drawRect(1520, y + 24, 330, 44, {0, 240, 255, 255}, 1);
            drawText("[X] FOCUS & LAUNCH GAME", 1545, y + 36, 15, {0, 240, 255, 255});
        }
    }
}

void UiRenderer::drawFooter(const std::string& statusMessage, NavFocusArea currentFocus) {
    fillRect(0, 995, 1920, 85, {16, 20, 28, 255});
    fillRect(0, 995, 1920, 2, {48, 54, 61, 255});

    // Left status string
    drawText("alsb3 dev >> " + statusMessage, 50, 1024, 16, {0, 240, 255, 255});

    // Right controller legend
    std::string legend = (currentFocus == FOCUS_ACTIONS) ?
        "[◄/►] Switch Action Card  |  [X] Execute Action  |  [▼] Go to Game Library  |  [O] Rescan" :
        "[▲/▼] Scroll Games  |  [X] Engage Focus & Launch  |  [▲] Go to Action Cards  |  [O] Rescan";

    drawText(legend, 960, 1024, 15, {255, 215, 0, 255});
}
