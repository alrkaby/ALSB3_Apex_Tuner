#pragma once

#include "common.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>

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
    void drawGameGrid(const std::vector<GameItem>& games, int selectedIndex, bool isGameModeActive, const std::string& activeTitleId);
    void drawFooter(const std::string& statusMessage);
    void drawNotificationToast(const std::string& toast);

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
