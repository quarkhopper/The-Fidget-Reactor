// debug_console.cpp
#include "debug_console.hpp" // Updated include path for debug_console.hpp
#include <deque>
#include <string>
#include <unordered_set>
#include <SDL2/SDL_ttf.h>
#include <sstream>

static std::deque<std::string> logBuffer;
static std::deque<std::string> mmBuffer;
static const size_t maxLines = 20;
static TTF_Font* font = nullptr;
static std::string activeMMTag;
static std::unordered_set<std::string> knownTags;

bool DebugConsole::init(const std::string& fontPath, int fontSize) {
    if (TTF_Init() != 0) return false;
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    return font != nullptr;
}

void DebugConsole::shutdown() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void DebugConsole::setActiveMMTag(const std::string& tag) {
    activeMMTag = tag;
}

void DebugConsole::log(const std::string& message) {
    // Always add message to main console (logBuffer)
    logBuffer.push_back(message);
    if (logBuffer.size() > maxLines) logBuffer.pop_front();
    
    // For tagged messages, also add to MM if the tag matches activeMMTag
    if (message.rfind("[", 0) == 0) {
        size_t tagEnd = message.find("]");
        if (tagEnd != std::string::npos) {
            std::string tag = message.substr(1, tagEnd - 1);
            knownTags.insert(tag);
            if (tag == activeMMTag) {
                mmBuffer.push_back(message);
                if (mmBuffer.size() > maxLines) mmBuffer.pop_front();
            }
        }
    }
}

void DebugConsole::render(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    SDL_Color textColor = {255, 255, 255, 255};
    int offsetY = 0;
    for (const auto& line : logBuffer) {
        SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect destRect = {x + 10, y + offsetY, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            offsetY += 18;
        }
    }
}

void DebugConsole::renderMM(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    SDL_Color textColor = {200, 200, 255, 255};
    int offsetY = 0;
    for (const auto& line : mmBuffer) {
        SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect destRect = {x + 10, y + offsetY, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            offsetY += 18;
        }
    }
}

namespace {
    struct DebugStreamBuf : std::streambuf {
        std::ostringstream oss;

        int overflow(int c) override {
            if (c == '\n') {
                DebugConsole::log(oss.str());
                oss.str("");
                oss.clear();
            } else {
                oss.put(static_cast<char>(c));
            }
            return c;
        }
    } debugBuf;
}

DebugCoutRedirect::DebugCoutRedirect() {
    oldBuf = std::cout.rdbuf(&debugBuf);
}

DebugCoutRedirect::~DebugCoutRedirect() {
    std::cout.rdbuf(oldBuf);
}
