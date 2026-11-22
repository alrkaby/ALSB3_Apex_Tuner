#pragma once

#include "common.h"
#include "smart_diagnostics.h"
#include "network_booster.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <string>

enum ActionButtonType {
    ACTION_BTN_GOD_MODE = 0,
    ACTION_BTN_NET_BOOST,
    ACTION_BTN_CLEAN_VACUUM,
    ACTION_BTN_THERMAL_GUARD,
    ACTION_BTN_COUNT
};

enum NavFocusArea {
    FOCUS_ACTIONS = 0,
    FOCUS_GAMES = 1
};

struct ColorRGBA {
    Uint8 r, g, b, a;
};

class UiRenderer {
public:
    UiRenderer();
    ~UiRenderer();

    bool init(int width = 1920, int height = 1080);
    void shutdown();

    void beginFrame();
    void endFrame();

    void drawHeader(const SystemMetrics& metrics);
    void drawDiagnosticBanner(const DiagnosticReport& report, const NetworkStressReport& netReport);
    void drawActionButtons(int selectedButtonIndex, bool isFocusOnButtons);
    void drawGameGrid(const std::vector<GameItem>& games, int selectedIndex, bool isGameModeActive, const std::string& activeTitleId, bool isFocusOnGames);
    void drawFooter(const std::string& statusMessage, NavFocusArea currentFocus);

    void fillRect(int x, int y, int w, int h, ColorRGBA color);
    void drawRect(int x, int y, int w, int h, ColorRGBA color, int thickness = 2);
    void drawText(const std::string& text, int x, int y, int fontSize, ColorRGBA color);
    void drawImage(const std::string& imagePath, int x, int y, int w, int h);

private:
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    TTF_Font* fontTitle{nullptr};
    TTF_Font* fontRegular{nullptr};
    TTF_Font* fontSmall{nullptr};

    std::unordered_map<std::string, SDL_Texture*> textureCache;

    SDL_Texture* loadTexture(const std::string& path);
};
