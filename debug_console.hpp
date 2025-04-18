// debug_console.hpp
#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <iostream>

namespace DebugConsole {
    bool init(const std::string& fontPath, int fontSize);
    void shutdown();
    void log(const std::string& message);
    void render(SDL_Renderer* renderer, int x, int y);
    void renderMM(SDL_Renderer* renderer, int x, int y);
    void setActiveMMTag(const std::string& tag);
}

// Redirect std::cout to DebugConsole
struct DebugCoutRedirect {
    DebugCoutRedirect();
    ~DebugCoutRedirect();

private:
    std::streambuf* oldBuf;
};
