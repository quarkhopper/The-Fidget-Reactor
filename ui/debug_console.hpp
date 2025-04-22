// debug_console.hpp
#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // Added to make TTF_Font available
#include <iostream>
#include <sstream>  // Added for std::ostringstream
#include <vector>

namespace DebugConsole {
    bool init(const std::string& fontPath, int fontSize);
    void shutdown();
    void log(const std::string& message, bool isError = false);
    void render(SDL_Renderer* renderer, int x, int y);
    void renderMM(SDL_Renderer* renderer, int x, int y);
    void setActiveMMTag(const std::string& tag);
    
    // New function to handle mouse wheel events for scrolling
    void handleMouseWheel(int x, int y, int wheelY);
    
    // Fixed signature to match implementation
    std::vector<std::string> wrapText(const std::string& text, TTF_Font* font, int maxWidth);
}

// Redirect both std::cout and std::cerr to DebugConsole
struct DebugStreamBuf : std::streambuf {
    std::ostringstream oss;
    bool isError;

    DebugStreamBuf(bool isError = false) : isError(isError) {}

    int overflow(int c) override {
        if (c == '\n') {
            std::string msg = oss.str();
            DebugConsole::log(msg, isError);
            oss.str("");
            oss.clear();
        } else {
            oss.put(static_cast<char>(c));
        }
        return c;
    }
};

struct DebugCoutRedirect {
    DebugCoutRedirect() : coutBuf(false), cerrBuf(true) {
        oldCoutBuf = std::cout.rdbuf(&coutBuf);
        oldCerrBuf = std::cerr.rdbuf(&cerrBuf);
    }
    
    ~DebugCoutRedirect() {
        std::cout.rdbuf(oldCoutBuf);
        std::cerr.rdbuf(oldCerrBuf);
    }

private:
    DebugStreamBuf coutBuf;
    DebugStreamBuf cerrBuf;
    std::streambuf* oldCoutBuf;
    std::streambuf* oldCerrBuf;
};
