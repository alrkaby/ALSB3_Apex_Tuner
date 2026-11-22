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

    window = SDL_CreateWindow("OrbisApex", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        LOGE("Failed to create SDL window: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    // Try loading bundled font
    const char* fontPaths[] = {
        "/app0/assets/fonts/font.ttf",
        "/data/OrbisApex/font.ttf",
        "assets/fonts/font.ttf"
    };

    for (const char* p : fontPaths) {
        fontTitle = TTF_OpenFont(p, 32);
        if (fontTitle) {
            fontRegular = TTF_OpenFont(p, 22);
            fontSmall = TTF_OpenFont(p, 16);
            break;
        }
    }

    LOGI("UI Renderer initialized successfully");
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
    SDL_SetRenderDrawColor(renderer, 13, 17, 23, 255); // Dark Slate
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
    if (fontSize >= 28) f = fontTitle;
    else if (fontSize <= 18) f = fontSmall;

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
    if (textureCache.find(path) != textureCache.end()) {
        return textureCache[path];
    }
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
        // Fallback placeholder box
        fillRect(x, y, w, h, {33, 38, 45, 255});
        drawRect(x, y, w, h, {48, 54, 61, 255}, 1);
    }
}

void UiRenderer::drawHeader(const SystemMetrics& metrics) {
    // Top banner
    fillRect(0, 0, 1920, 95, {22, 27, 34, 255});
    fillRect(0, 93, 1920, 3, {0, 240, 255, 255}); // Neon Cyan accent line

    drawText("ORBIS APEX", 50, 22, 34, {0, 240, 255, 255});
    drawText("APEX GAMING MODE & SYSTEM TUNER", 52, 60, 16, {139, 148, 158, 255});

    // Metrics Badges
    // 1. SOC Temp
    fillRect(800, 25, 200, 48, {30, 36, 46, 255});
    drawRect(800, 25, 200, 48, {48, 54, 61, 255}, 1);
    drawText("TEMP: " + std::to_string(metrics.socTempCelsius) + " C", 820, 38, 18, {0, 255, 157, 255});

    // 2. Free RAM
    uint64_t ramMb = metrics.freeRamBytes / (1024 * 1024);
    fillRect(1020, 25, 240, 48, {30, 36, 46, 255});
    drawRect(1020, 25, 240, 48, {48, 54, 61, 255}, 1);
    drawText("FREE RAM: " + std::to_string(ramMb) + " MB", 1040, 38, 18, {0, 240, 255, 255});

    // 3. Game Mode Badge
    if (metrics.gameModeActive) {
        fillRect(1280, 25, 590, 48, {40, 30, 15, 255});
        drawRect(1280, 25, 590, 48, {255, 215, 0, 255}, 2);
        drawText("APEX MODE: " + metrics.activeFocusedGame + " (HOURLY TRIM ACTIVE)", 1300, 38, 18, {255, 215, 0, 255});
    } else {
        fillRect(1280, 25, 590, 48, {30, 36, 46, 255});
        drawRect(1280, 25, 590, 48, {48, 54, 61, 255}, 1);
        drawText("APEX MODE: STANDBY (SELECT GAME BELOW)", 1300, 38, 18, {139, 148, 158, 255});
    }
}

void UiRenderer::drawGameGrid(const std::vector<GameItem>& games, int selectedIndex, bool isGameModeActive, const std::string& activeTitleId) {
    drawText("INSTALLED LIBRARY (SELECT A GAME TO FOCUS IN APEX MODE)", 50, 125, 22, {240, 246, 252, 255});

    if (games.empty()) {
        fillRect(50, 180, 1820, 300, {22, 27, 34, 255});
        drawText("No installed games found in /system_data/priv/mms/app.db", 80, 240, 24, {139, 148, 158, 255});
        return;
    }

    // Display visible rows (e.g., 6 items visible per page)
    const int startY = 175;
    const int itemHeight = 110;
    const int maxVisible = 6;
    int scrollOffset = std::max(0, selectedIndex - (maxVisible - 1));

    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)games.size(); ++i) {
        int idx = i + scrollOffset;
        const auto& game = games[idx];
        bool isSelected = (idx == selectedIndex);
        bool isFocused = (isGameModeActive && game.titleId == activeTitleId);

        int y = startY + i * (itemHeight + 12);

        ColorRGBA bg = isSelected ? ColorRGBA{30, 45, 65, 255} : ColorRGBA{22, 27, 34, 255};
        fillRect(50, y, 1820, itemHeight, bg);

        if (isFocused) {
            drawRect(50, y, 1820, itemHeight, {255, 215, 0, 255}, 3); // Gold border
        } else if (isSelected) {
            drawRect(50, y, 1820, itemHeight, {0, 240, 255, 255}, 2); // Cyan border
        } else {
            drawRect(50, y, 1820, itemHeight, {48, 54, 61, 255}, 1);
        }

        // Game Icon
        drawImage(game.iconPath, 65, y + 10, 90, 90);

        // Title and Title ID
        drawText(game.name, 175, y + 22, 26, isSelected ? ColorRGBA{255, 255, 255, 255} : ColorRGBA{201, 209, 217, 255});
        drawText("TITLE ID: " + game.titleId, 175, y + 62, 17, {139, 148, 158, 255});

        // Focus Badge
        if (isFocused) {
            fillRect(1580, y + 32, 260, 45, {255, 215, 0, 40});
            drawRect(1580, y + 32, 260, 45, {255, 215, 0, 255}, 1);
            drawText("ACTIVE APEX FOCUS", 1605, y + 44, 18, {255, 215, 0, 255});
        }
    }
}

void UiRenderer::drawFooter(const std::string& statusMessage) {
    fillRect(0, 995, 1920, 85, {22, 27, 34, 255});
    fillRect(0, 995, 1920, 2, {48, 54, 61, 255});

    // Status / Message
    drawText(statusMessage, 50, 1025, 20, {0, 240, 255, 255});

    // Control Legends
    std::string controls = "[X] Engage Apex Mode   [SQUARE] Defrag Database   [TRIANGLE] Clean Junk   [OPTIONS] Thermal 62C";
    drawText(controls, 880, 1025, 17, {139, 148, 158, 255});
}
