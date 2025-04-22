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

// Simple button class for console toolbar
class ConsoleButton {
public:
    ConsoleButton(const std::string& label, int x, int y, int width, int height)
        : label(label), bounds{x, y, width, height}, isHovered(false), isPressed(false) {}
    
    bool handleMouseMove(int mouseX, int mouseY) {
        bool wasHovered = isHovered;
        isHovered = (mouseX >= bounds.x && mouseX < bounds.x + bounds.w &&
                    mouseY >= bounds.y && mouseY < bounds.y + bounds.h);
        return wasHovered != isHovered; // Return true if hover state changed
    }
    
    bool handleMouseDown(int mouseX, int mouseY) {
        if (isHovered) {
            isPressed = true;
            return true;
        }
        return false;
    }
    
    bool handleMouseUp(int mouseX, int mouseY) {
        bool wasPressed = isPressed;
        isPressed = false;
        return wasPressed && isHovered; // Return true if button was clicked (pressed and released while hovering)
    }
    
    void render(SDL_Renderer* renderer, TTF_Font* font) {
        // Draw button background based on state
        SDL_Color bgColor = {50, 50, 50, 255}; // Default
        if (isPressed) {
            bgColor = {30, 30, 100, 255}; // Pressed
        } else if (isHovered) {
            bgColor = {70, 70, 90, 255}; // Hover
        }
        
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &bounds);
        
        // Draw button border
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawRect(renderer, &bounds);
        
        // Draw button text
        if (font) {
            SDL_Color textColor = {220, 220, 220, 255};
            SDL_Surface* surface = TTF_RenderText_Blended(font, label.c_str(), textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                
                // Center text in button
                SDL_Rect textRect = {
                    bounds.x + (bounds.w - surface->w) / 2,
                    bounds.y + (bounds.h - surface->h) / 2,
                    surface->w,
                    surface->h
                };
                
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }
        }
    }
    
    void setPosition(int x, int y) {
        bounds.x = x;
        bounds.y = y;
    }
    
private:
    std::string label;
    SDL_Rect bounds;
    bool isHovered;
    bool isPressed;
};

// Container class to manage text display with automatic wrapping
class TextContainer {
public:
    TextContainer(int x, int y, int width, int height)
        : bounds{x, y, width, height}, scrollOffset(0), 
          copyButton("Copy", x + width - 70, y + height - 30, 60, 25) {}
    
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
    
    // Handle mouse events for buttons
    bool handleMouseMove(int mouseX, int mouseY) {
        return copyButton.handleMouseMove(mouseX, mouseY);
    }
    
    bool handleMouseDown(int mouseX, int mouseY) {
        return copyButton.handleMouseDown(mouseX, mouseY);
    }
    
    bool handleMouseUp(int mouseX, int mouseY) {
        if (copyButton.handleMouseUp(mouseX, mouseY)) {
            copyToClipboard();
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
        
        // Draw container background
        SDL_Rect bgRect = {bounds.x, bounds.y, bounds.w, bounds.h};
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderFillRect(renderer, &bgRect);
        
        // Calculate visible area - adjusted to leave room for toolbar
        int textAreaWidth = bounds.w - 20; // 10px margin on each side
        int toolbarHeight = 30;
        int visibleHeight = bounds.h - toolbarHeight;
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
        int endLine = startLine + maxVisibleLines;
        if (endLine > static_cast<int>(cachedWrappedLines.size())) {
            endLine = static_cast<int>(cachedWrappedLines.size());
        }
        
        int offsetY = 0;
        for (int i = startLine; i < endLine; i++) {
            // Use traditional pair access instead of structured binding
            const std::string& line = cachedWrappedLines[i].first;
            bool isError = cachedWrappedLines[i].second;
            
            // Select color based on message type
            SDL_Color textColor;
            if (isError) {
                textColor.r = 255;
                textColor.g = 100;
                textColor.b = 100;
                textColor.a = 255;
            } else {
                textColor = defaultColor;
            }
                
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
        
        // Draw toolbar background
        SDL_Rect toolbarRect = {bounds.x, bounds.y + bounds.h - toolbarHeight, bounds.w, toolbarHeight};
        SDL_SetRenderDrawColor(renderer, 40, 40, 45, 255);
        SDL_RenderFillRect(renderer, &toolbarRect);
        
        // Draw separator line
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderDrawLine(renderer, bounds.x, bounds.y + bounds.h - toolbarHeight, 
                          bounds.x + bounds.w, bounds.y + bounds.h - toolbarHeight);
        
        // Render button
        copyButton.render(renderer, font);
    }
    
    // Set new bounds for the container (for resizing)
    void setBounds(int x, int y, int width, int height) {
        bounds = {x, y, width, height};
        // Update button position
        copyButton.setPosition(x + width - 70, y + height - 30);
        // Invalidate cached wrapped lines when resizing
        cachedWrappedLines.clear();
    }
    
    // Get all text content for clipboard
    std::string getAllText() const {
        std::ostringstream oss;
        for (const auto& msg : messages) {
            oss << msg.text << "\n";
        }
        return oss.str();
    }
    
private:
    static const int LINE_HEIGHT = 18;
    static const size_t MAX_MESSAGES = 100;
    
    SDL_Rect bounds;
    int scrollOffset;
    std::deque<LogMessage> messages;
    std::vector<std::pair<std::string, bool>> cachedWrappedLines;
    ConsoleButton copyButton;
    
    // Copy console content to clipboard
    void copyToClipboard() {
        std::string content = getAllText();
        
        #ifdef _WIN32
        // Open clipboard
        if (!OpenClipboard(nullptr)) return;
        
        // Empty clipboard
        EmptyClipboard();
        
        // Allocate global memory for text
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, content.size() + 1);
        if (!hMem) {
            CloseClipboard();
            return;
        }
        
        // Lock memory and copy text
        char* pMem = (char*)GlobalLock(hMem);
        memcpy(pMem, content.c_str(), content.size() + 1);
        GlobalUnlock(hMem);
        
        // Set clipboard data and close
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
        
        std::cout << "Console content copied to clipboard" << std::endl;
        #else
        // For non-Windows platforms, print a message for now
        std::cout << "Clipboard functionality not implemented for this platform" << std::endl;
        #endif
    }
    
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
        if (thumbHeight < 20) {
            thumbHeight = 20; // Minimum thumb size
        }
        
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
    SDL_Color defaultTextColor;
    defaultTextColor.r = 255;
    defaultTextColor.g = 255;
    defaultTextColor.b = 255;
    defaultTextColor.a = 255; // White for normal messages
    mainConsole.render(renderer, font, defaultTextColor);
}

void renderMM(SDL_Renderer* renderer, int x, int y) {
    if (!font) return;
    // Use the MM console container
    SDL_Color defaultTextColor;
    defaultTextColor.r = 200;
    defaultTextColor.g = 200;
    defaultTextColor.b = 255;
    defaultTextColor.a = 255; // Blue tint for MM messages
    mmConsole.render(renderer, font, defaultTextColor);
}

// Handle mouse wheel events for scrolling
void handleMouseWheel(int x, int y, int wheelY) {
    // Use container's built-in hit testing and scrolling
    mainConsole.handleScroll(x, y, wheelY);
    mmConsole.handleScroll(x, y, wheelY);
}

// Add mouse event handlers for the buttons
bool handleMouseMove(int x, int y) {
    bool handled = false;
    handled |= mainConsole.handleMouseMove(x, y);
    handled |= mmConsole.handleMouseMove(x, y);
    return handled;
}

bool handleMouseDown(int x, int y) {
    bool handled = false;
    handled |= mainConsole.handleMouseDown(x, y);
    handled |= mmConsole.handleMouseDown(x, y);
    return handled;
}

bool handleMouseUp(int x, int y) {
    bool handled = false;
    handled |= mainConsole.handleMouseUp(x, y);
    handled |= mmConsole.handleMouseUp(x, y);
    return handled;
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
