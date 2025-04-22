// main_window.cpp

#include <windows.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>  // Added for std::ifstream
#include <cstdlib>
#include "power_button.hpp"
#include "../bus/pin_sim.hpp" // Updated include path for pin_sim.hpp
#include "debug_console.hpp" // Updated include path for debug_console.hpp
#include "../config/config_helper.hpp" // Include ConfigHelper for configuration handling

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Launches an external .exe without opening a popup
bool launchProcess(const char* path) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    BOOL success = CreateProcessA(
        nullptr,                // Application name
        (LPSTR)path,            // Command line
        nullptr, nullptr,       // Process & thread attributes
        FALSE,                  // Inherit handles
        CREATE_NO_WINDOW,       // Creation flags
        nullptr, nullptr,       // Environment, current directory
        &si, &pi                // STARTUPINFO and PROCESS_INFORMATION
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cout << "[BOOT] Failed to launch: " << path << std::endl;
    }

    return success;
}

// Ensure clean exit on errors
void cleanExit(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    DebugConsole::shutdown();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize debug console and redirect std::cout FIRST
    DebugCoutRedirect redirect;
    DebugConsole::init("C:/Windows/Fonts/consola.ttf", 14);
    DebugConsole::setActiveMMTag("BOOT");

    // Now initialize pipes from configuration file - AFTER the redirect is set up
    nlohmann::json config;
    try {
        std::cout << "[Main] Loading configuration file..." << std::endl;
        std::ifstream configFile("simulation_config.json");
        if (!configFile.is_open()) {
            std::cerr << "Failed to open simulation_config.json" << std::endl;
            cleanExit(nullptr, nullptr);
            return 1;
        }
        configFile >> config;
        
        // Initialize all named pipes from controller_pipes and phy_pipes sections
        std::cout << "[Main] Initializing named pipes..." << std::endl;
        ConfigHelper::initializePipes(config);
        
        // Set up PinSim instances for the debug UI
        if (config.contains("wiring") && config["wiring"].contains("debug_ui")) {
            std::cout << "[Main] Setting up PinSim wiring for debug UI..." << std::endl;
            std::unordered_map<std::string, PinSim> pinSims;
            ConfigHelper::setupPinSimWiring(config["wiring"]["debug_ui"], pinSims);
            
            // Store the PinSim instances for use in the UI
            // For example, associate them with UI buttons
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to set up configuration: " << e.what() << std::endl;
        cleanExit(nullptr, nullptr);
        return 1;
    }

    // Boot messages
    std::cout << "[BOOT] Bootup succeeded" << std::endl;

    // Load UI window
    SDL_Window* window = SDL_CreateWindow("Fidget Reactor UI",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL Create Window Error: " << SDL_GetError() << std::endl;
        cleanExit(nullptr, nullptr);
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL Create Renderer Error: " << SDL_GetError() << std::endl;
        cleanExit(window, nullptr);
        return 1;
    }

    PowerButton masterButton(350, 50, 40, 40); // MASTER in main panel

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                masterButton.handleClick(mouseX, mouseY);
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                // Get the mouse position for determining which panel was scrolled
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                
                // Pass the mouse wheel event to the debug console
                DebugConsole::handleMouseWheel(mouseX, mouseY, event.wheel.y);
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Panels
        SDL_Rect testPanel = {0, 0, 300, 450};
        SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
        SDL_RenderFillRect(renderer, &testPanel);

        SDL_Rect debugConsole = {0, 450, 600, 350};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &debugConsole);

        SDL_Rect mmPanel = {600, 450, 600, 350};
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderFillRect(renderer, &mmPanel);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 600, 450, 600, 800);

        SDL_Rect mainPanel = {300, 0, 900, 450};
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &mainPanel);

        masterButton.render(renderer);

        DebugConsole::render(renderer, 0, 450);
        DebugConsole::renderMM(renderer, 600, 450);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    cleanExit(window, renderer);
    return 0;
}
