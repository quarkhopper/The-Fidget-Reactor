// debug_console.cpp
#include "debug_console.hpp"
#include <deque>
#include <string>
#include <unordered_set>
#include <sstream>

namespace DebugConsole {

// Forward declaration moved to the top
struct LogMessage {
    std::string text;
    bool isError;
    
    LogMessage(const std::string& text, bool isError = false) 
        : text(text), isError(isError) {}
};

// Container class to manage text display with automatic wrapping
class TextContainer {
public:
    TextContainer(int x, int y, int width, int height)
        : bounds{x, y, width, height}, scrollOffset(0) {}
    
    // Add a message to the container
    void addMessage(const std::string& text, bool isError) {
        messages.push_back(LogMessage(text, isError));
        if (messages.size() > MAX_MESSAGES) {
            messages.pop_front();
        }
        // Invalidate cached wrapped lines when adding new messages
        cachedWrappedLines.clear();
    }
    
    // Clear all messages
    void clear() {
        messages.clear();
        cachedWrappedLines.clear();
        scrollOffset = 0;
    }
    
    // Handle scroll events within this container
    bool handleScroll(int mouseX, int mouseY, int scrollAmount) {
        if (mouseX >= bounds.x && mouseX < bounds.x + bounds.w &&
            mouseY >= bounds.y && mouseY < bounds.y + bounds.h) {
            scrollOffset -= scrollAmount;
            // Bounds checking done in render
            return true;
        }
        return false;
    }
    
    // Render the container contents with wrapping
    void render(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& defaultColor) {
        if (!font) return;
        
        // Calculate or use cached wrapped lines
        if (cachedWrappedLines.empty()) {
            calculateWrappedLines(font);
        }
        
        // Calculate visible area
        int textAreaWidth = bounds.w - 20; // 10px margin on each side
        int visibleHeight = bounds.h;
        int maxVisibleLines = visibleHeight / LINE_HEIGHT;
        
        // Calculate max scroll
        int totalLines = cachedWrappedLines.size();
        int maxScroll = (totalLines > maxVisibleLines) ? (totalLines - maxVisibleLines) : 0;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
        if (scrollOffset < 0) scrollOffset = 0;
        
        // Render scrollbar if needed
        if (totalLines > maxVisibleLines) {
            renderScrollbar(renderer, totalLines, maxVisibleLines);
        }
        
        // Render visible lines
        int startLine = scrollOffset;
        int endLine = std::min(startLine + maxVisibleLines, static_cast<int>(cachedWrappedLines.size()));
        
        int offsetY = 0;
        for (int i = startLine; i < endLine; i++) {
            const auto& [line, isError] = cachedWrappedLines[i];
            
            // Select color based on message type
            SDL_Color textColor = isError ? 
                SDL_Color{255, 100, 100, 255} : // Red for errors
                defaultColor;                   // Default color for normal messages
                
            SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect destRect = {bounds.x + 10, bounds.y + offsetY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &destRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
                offsetY += LINE_HEIGHT;
            }
        }
    }
    
    // Set new bounds for the container (for resizing)
    void setBounds(int x, int y, int width, int height) {
        bounds = {x, y, width, height};
        // Invalidate cached wrapped lines when resizing
        cachedWrappedLines.clear();
    }
    
private:
    static const int LINE_HEIGHT = 18;
    static const size_t MAX_MESSAGES = 100;
    
    SDL_Rect bounds;
    int scrollOffset;
    std::deque<LogMessage> messages;
    std::vector<std::pair<std::string, bool>> cachedWrappedLines;
    
    // Calculate wrapped lines for all messages
    void calculateWrappedLines(TTF_Font* font) {
        int textAreaWidth = bounds.w - 20; // 10px margin on each side
        
        for (const auto& msg : messages) {
            auto wrappedLines = wrapTextProperly(msg.text, font, textAreaWidth);
            for (const auto& line : wrappedLines) {
                cachedWrappedLines.push_back(std::make_pair(line, msg.isError));
            }
        }
    }
    
    // Render scrollbar for the container
    void renderScrollbar(SDL_Renderer* renderer, int totalLines, int visibleLines) {
        // Background of scrollbar
        SDL_Rect scrollBarBg = {bounds.x + bounds.w - 10, bounds.y, 10, bounds.h};
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &scrollBarBg);
        
        // Scrollbar thumb
        float thumbRatio = static_cast<float>(visibleLines) / totalLines;
        int thumbHeight = static_cast<int>(bounds.h * thumbRatio);
        thumbHeight = std::max(thumbHeight, 20); // Minimum thumb size
        
        int maxScroll = totalLines - visibleLines;
        float scrollRatio = maxScroll > 0 ? static_cast<float>(scrollOffset) / maxScroll : 0;
        int thumbY = bounds.y + static_cast<int>((bounds.h - thumbHeight) * scrollRatio);
        
        SDL_Rect scrollThumb = {bounds.x + bounds.w - 10, thumbY, 10, thumbHeight};
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &scrollThumb);
    }
    
    // Improved text wrapping function that accurately tracks line width
    std::vector<std::string> wrapTextProperly(const std::string& text, TTF_Font* font, int maxWidth) {
        std::vector<std::string> lines;
        std::string currentLine;
        std::istringstream iss(text);
        std::string word;
        
        int currentLineWidth = 0;
        
        while (iss >> word) {
            // Get width of word with space if needed
            int spaceWidth = 0;
            if (!currentLine.empty()) {
                TTF_SizeText(font, " ", &spaceWidth, nullptr);
            }
            
            int wordWidth = 0;
            TTF_SizeText(font, word.c_str(), &wordWidth, nullptr);
            
            // Check if adding this word would exceed max width
            if (currentLineWidth + spaceWidth + wordWidth > maxWidth && !currentLine.empty()) {
                // Line would be too long, start a new line
                lines.push_back(currentLine);
                currentLine = word;
                TTF_SizeText(font, currentLine.c_str(), &currentLineWidth, nullptr);
            } else {
                // Add to current line
                if (!currentLine.empty()) {
                    currentLine += " ";
                    currentLineWidth += spaceWidth;
                }
                currentLine += word;
                // Recalculate the actual width of the current line (more accurate than just adding wordWidth)
                TTF_SizeText(font, currentLine.c_str(), &currentLineWidth, nullptr);
            }
        }
        
        // Add the last line if not empty
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
        
        return lines;
    }
};

// Replace individual buffers with containers
static TextContainer mainConsole(0, 450, 600, 350);
static TextContainer mmConsole(600, 450, 600, 350);
static const size_t maxLines = 100;
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
    mainConsole.addMessage(message, isError);
    
    // For tagged messages, also add to MM if the tag matches activeMMTag
    if (message.rfind("[", 0) == 0) {
        size_t tagEnd = message.find("]");
        if (tagEnd != std::string::npos) {
            std::string tag = message.substr(1, tagEnd - 1);
            knownTags.insert(tag);
            if (tag == activeMMTag) {
                mmConsole.addMessage(message, isError);
            }
        }
    }
}

void render(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    // Use the main console container
    SDL_Color defaultTextColor = {255, 255, 255, 255}; // White for normal messages
    mainConsole.render(renderer, font, defaultTextColor);
}

void renderMM(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    // Use the MM console container
    SDL_Color defaultTextColor = {200, 200, 255, 255}; // Blue tint for MM messages
    mmConsole.render(renderer, font, defaultTextColor);
}

// Handle mouse wheel events for scrolling
void handleMouseWheel(int x, int y, int wheelY) {
    // Use container's built-in hit testing and scrolling
    mainConsole.handleScroll(x, y, wheelY);
    mmConsole.handleScroll(x, y, wheelY);
}

// Add function to support dynamic resizing of console panels
void resizeConsoles(int mainX, int mainY, int mainWidth, int mainHeight,
                    int mmX, int mmY, int mmWidth, int mmHeight) {
    mainConsole.setBounds(mainX, mainY, mainWidth, mainHeight);
    mmConsole.setBounds(mmX, mmY, mmWidth, mmHeight);
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
