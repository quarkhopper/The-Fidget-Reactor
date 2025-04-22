// debug_console.cpp
#include "debug_console.hpp"
#include <deque>
#include <string>
#include <unordered_set>
#include <sstream>

namespace DebugConsole {

// Add a structure to track message colors
struct LogMessage {
    std::string text;
    bool isError;
    
    LogMessage(const std::string& text, bool isError = false) 
        : text(text), isError(isError) {}
};

static std::deque<LogMessage> logBuffer;
static std::deque<LogMessage> mmBuffer;
static const size_t maxLines = 20;
static TTF_Font* font = nullptr;
static std::string activeMMTag;
static std::unordered_set<std::string> knownTags;

// Add scroll offset for console panels
static int logScrollOffset = 0;
static int mmScrollOffset = 0;
static const int lineHeight = 18; // Height of each line for scrolling calculations

bool init(const std::string& fontPath, int fontSize) {
    if (TTF_Init() != 0) return false;
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    return font != nullptr;
}

void shutdown() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void setActiveMMTag(const std::string& tag) {
    activeMMTag = tag;
}

// Updated to handle error messages - removed the default parameter here
void log(const std::string& message, bool isError) {
    // Always add message to main console (logBuffer)
    logBuffer.push_back(LogMessage(message, isError));
    if (logBuffer.size() > maxLines) logBuffer.pop_front();
    
    // For tagged messages, also add to MM if the tag matches activeMMTag
    if (message.rfind("[", 0) == 0) {
        size_t tagEnd = message.find("]");
        if (tagEnd != std::string::npos) {
            std::string tag = message.substr(1, tagEnd - 1);
            knownTags.insert(tag);
            if (tag == activeMMTag) {
                mmBuffer.push_back(LogMessage(message, isError));
                if (mmBuffer.size() > maxLines) mmBuffer.pop_front();
            }
        }
    }
}

void render(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    int offsetY = 0;
    int maxWidth = 580; // Use nearly the full width of the console panel (600px)
    int visibleHeight = 350; // Height of the console panel
    int maxVisibleLines = visibleHeight / lineHeight;
    
    // Calculate total rendered height for scrollbar
    std::vector<std::pair<std::string, bool>> allWrappedLines;
    for (const auto& logMsg : logBuffer) {
        try {
            auto wrappedLines = wrapText(logMsg.text, font, maxWidth);
            for (const auto& line : wrappedLines) {
                allWrappedLines.push_back({line, logMsg.isError});
            }
        } catch (const std::exception& e) {
            allWrappedLines.push_back({logMsg.text, logMsg.isError});
        }
    }
    
    // Calculate maximum scroll offset
    int totalLines = allWrappedLines.size();
    int maxScroll = (totalLines > maxVisibleLines) ? (totalLines - maxVisibleLines) : 0;
    if (logScrollOffset > maxScroll) logScrollOffset = maxScroll;
    if (logScrollOffset < 0) logScrollOffset = 0;
    
    // Render the scrollbar if needed
    if (totalLines > maxVisibleLines) {
        // Background of scrollbar
        SDL_Rect scrollBarBg = {x + 590, y, 10, visibleHeight};
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &scrollBarBg);
        
        // Scrollbar thumb
        float thumbRatio = static_cast<float>(maxVisibleLines) / totalLines;
        int thumbHeight = static_cast<int>(visibleHeight * thumbRatio);
        int thumbY = y + static_cast<int>((visibleHeight - thumbHeight) * 
                                        (static_cast<float>(logScrollOffset) / maxScroll));
        SDL_Rect scrollThumb = {x + 590, thumbY, 10, thumbHeight};
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &scrollThumb);
    }
    
    // Render visible wrapped lines with scroll offset applied
    int startLine = logScrollOffset;
    int endLine = std::min(startLine + maxVisibleLines, static_cast<int>(allWrappedLines.size()));
    
    for (int i = startLine; i < endLine; i++) {
        const auto& [line, isError] = allWrappedLines[i];
        // Use red color for error messages
        SDL_Color textColor = isError ? 
            SDL_Color{255, 100, 100, 255} : // Red for errors
            SDL_Color{255, 255, 255, 255};  // White for normal messages
            
        SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect destRect = {x + 10, y + offsetY, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            offsetY += lineHeight;
        }
    }
}

void renderMM(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    int offsetY = 0;
    int maxWidth = 580; // Use nearly the full width of the console panel (600px)
    int visibleHeight = 350; // Height of the console panel
    int maxVisibleLines = visibleHeight / lineHeight;
    
    // Calculate total rendered height for scrollbar
    std::vector<std::pair<std::string, bool>> allWrappedLines;
    for (const auto& logMsg : mmBuffer) {
        try {
            auto wrappedLines = wrapText(logMsg.text, font, maxWidth);
            for (const auto& line : wrappedLines) {
                allWrappedLines.push_back({line, logMsg.isError});
            }
        } catch (const std::exception& e) {
            allWrappedLines.push_back({logMsg.text, logMsg.isError});
        }
    }
    
    // Calculate maximum scroll offset
    int totalLines = allWrappedLines.size();
    int maxScroll = (totalLines > maxVisibleLines) ? (totalLines - maxVisibleLines) : 0;
    if (mmScrollOffset > maxScroll) mmScrollOffset = maxScroll;
    if (mmScrollOffset < 0) mmScrollOffset = 0;
    
    // Render the scrollbar if needed
    if (totalLines > maxVisibleLines) {
        // Background of scrollbar
        SDL_Rect scrollBarBg = {x + 590, y, 10, visibleHeight};
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &scrollBarBg);
        
        // Scrollbar thumb
        float thumbRatio = static_cast<float>(maxVisibleLines) / totalLines;
        int thumbHeight = static_cast<int>(visibleHeight * thumbRatio);
        int thumbY = y + static_cast<int>((visibleHeight - thumbHeight) * 
                                        (static_cast<float>(mmScrollOffset) / maxScroll));
        SDL_Rect scrollThumb = {x + 590, thumbY, 10, thumbHeight};
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &scrollThumb);
    }
    
    // Render visible wrapped lines with scroll offset applied
    int startLine = mmScrollOffset;
    int endLine = std::min(startLine + maxVisibleLines, static_cast<int>(allWrappedLines.size()));
    
    for (int i = startLine; i < endLine; i++) {
        const auto& [line, isError] = allWrappedLines[i];
        // Use red color for error messages, blue tint for MM messages
        SDL_Color textColor = isError ? 
            SDL_Color{255, 100, 100, 255} : // Red for errors
            SDL_Color{200, 200, 255, 255};  // Blue tint for normal MM messages
            
        SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect destRect = {x + 10, y + offsetY, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            offsetY += lineHeight;
        }
    }
}

// Handle mouse wheel events for scrolling
void handleMouseWheel(int x, int y, int wheelY) {
    // Determine which console panel was scrolled
    if (x < 600) { // Left console panel (main log)
        logScrollOffset -= wheelY;
    } else { // Right console panel (MM)
        mmScrollOffset -= wheelY;
    }
    
    // Bounds checking is done in the render functions
}

// Helper function to wrap text to a maximum width
std::vector<std::string> wrapText(const std::string& text, TTF_Font* font, int maxWidth) {
    std::vector<std::string> lines;
    std::string currentLine;
    std::istringstream iss(text);
    std::string word;
    
    int lineWidth = 0;
    while (iss >> word) {
        int wordWidth, wordHeight;
        TTF_SizeText(font, (currentLine + (currentLine.empty() ? "" : " ") + word).c_str(), &wordWidth, &wordHeight);
        
        if (lineWidth + wordWidth > maxWidth && !currentLine.empty()) {
            // Line would be too long, start a new line
            lines.push_back(currentLine);
            currentLine = word;
            TTF_SizeText(font, word.c_str(), &wordWidth, &wordHeight);
            lineWidth = wordWidth;
        } else {
            // Add to current line
            if (!currentLine.empty()) {
                currentLine += " ";
            }
            currentLine += word;
            lineWidth = wordWidth;
        }
    }
    
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    return lines;
}

} // namespace DebugConsole

// These old DebugStreamBuf and DebugCoutRedirect implementations can be removed
// The new implementations are now in debug_console.hpp

/*
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
*/
