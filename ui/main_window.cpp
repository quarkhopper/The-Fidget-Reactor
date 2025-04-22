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

// Add a logging helper with absolute path for debugging
std::ofstream debugLogFile("C:/Users/quark/OneDrive/Documents/GitHub/The-Fidget-Reactor/debug_log.txt", std::ios::out | std::ios::trunc);
void logToFile(const std::string& message) {
    if (debugLogFile.is_open()) {
        debugLogFile << message << std::endl;
        debugLogFile.flush();
    }
}


// BREAKPOINT #1: You can set a breakpoint here at program start
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
    // BREAKPOINT #2: This is an ideal place for a breakpoint
    std::cout << "DEBUG: Program starting - breakpoint opportunity" << std::endl;
    
    // Log immediately at program start
    logToFile("Program started");
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        logToFile("SDL Init Error: " + std::string(SDL_GetError()));
        return 1;
    }
    std::cout << "DEBUG: SDL initialized successfully" << std::endl;

    // TEMPORARY DEBUGGING: Disable debug console redirection
    std::cout << "Starting program - debug console redirection disabled for debugging" << std::endl;
    
    // Initialize debug console and redirect std::cout FIRST
    DebugCoutRedirect redirect;
    bool debugConsoleInitialized = false;
    try {
        // BREAKPOINT #3: Set a breakpoint before debug console initialization
        std::cout << "DEBUG: About to initialize debug console" << std::endl;
        debugConsoleInitialized = DebugConsole::init("C:/Windows/Fonts/consola.ttf", 14);
        if (!debugConsoleInitialized) {
            std::cerr << "Warning: Failed to initialize debug console - font loading error" << std::endl;
            std::cerr << "Continuing without debug console..." << std::endl;
            logToFile("Warning: Failed to initialize debug console - font loading error");
            logToFile("Continuing without debug console...");
            // Continue execution even if debug console fails
        } else {
            std::cout << "DEBUG: Debug console initialized successfully" << std::endl;
            DebugConsole::setActiveMMTag("BOOT");
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception during debug console initialization: " << e.what() << std::endl;
        logToFile("ERROR: Exception during debug console initialization: " + std::string(e.what()));
        // Continue without debug console
    }
    
    // Now initialize pipes from configuration file - AFTER the redirect is set up
    nlohmann::json config;
    try {
        // BREAKPOINT #4: Set a breakpoint before config loading
        std::cout << "DEBUG: About to load configuration file" << std::endl;
        logToFile("[Main] Loading configuration file...");
        std::ifstream configFile("simulation_config.json");
        if (!configFile.is_open()) {
            std::cerr << "Failed to open simulation_config.json" << std::endl;
            logToFile("Failed to open simulation_config.json");
            if (debugConsoleInitialized) {
                DebugConsole::shutdown(); // This calls TTF_Quit() internally
            }
            SDL_Quit();
            return 1;
        }
        configFile >> config;
        std::cout << "DEBUG: Configuration file loaded successfully" << std::endl;
        
        // Initialize all named pipes from controller_pipes and phy_pipes sections
        std::cout << "[Main] Initializing named pipes..." << std::endl;
        logToFile("[Main] Initializing named pipes...");
        ConfigHelper::initializePipes(config);
        
        // Set up PinSim instances for the debug UI
        if (config.contains("wiring") && config["wiring"].contains("debug_ui")) {
            std::cout << "[Main] Setting up PinSim wiring for debug UI..." << std::endl;
            logToFile("[Main] Setting up PinSim wiring for debug UI...");
            std::unordered_map<std::string, PinSim> pinSims;
            ConfigHelper::setupPinSimWiring(config["wiring"]["debug_ui"], pinSims);
            
            // Store the PinSim instances for use in the UI
            // For example, associate them with UI buttons
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to set up configuration: " << e.what() << std::endl;
        logToFile("Failed to set up configuration: " + std::string(e.what()));
        if (debugConsoleInitialized) {
            DebugConsole::shutdown(); // This calls TTF_Quit() internally
        }
        SDL_Quit();
        return 1;
    }

    // Boot messages
    std::cout << "[BOOT] Bootup succeeded" << std::endl;
    logToFile("[BOOT] Bootup succeeded");

    // BREAKPOINT #5: Set a breakpoint before window creation
    std::cout << "DEBUG: About to create window" << std::endl;
    // Load UI window
    SDL_Window* window = SDL_CreateWindow("Fidget Reactor UI",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL Create Window Error: " << SDL_GetError() << std::endl;
        logToFile("SDL Create Window Error: " + std::string(SDL_GetError()));
        if (debugConsoleInitialized) {
            DebugConsole::shutdown(); // This calls TTF_Quit() internally
        }
        SDL_Quit();
        return 1;
    }
    std::cout << "DEBUG: Window created successfully" << std::endl;
    std::cout << "Window created successfully" << std::endl;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL Create Renderer Error: " << SDL_GetError() << std::endl;
        logToFile("SDL Create Renderer Error: " + std::string(SDL_GetError()));
        SDL_DestroyWindow(window);
        if (debugConsoleInitialized) {
            DebugConsole::shutdown(); // This calls TTF_Quit() internally
        }
        SDL_Quit();
        return 1;
    }
    std::cout << "Renderer created successfully" << std::endl;

    PowerButton masterButton(350, 50, 40, 40); // MASTER in main panel

    // BREAKPOINT #6: Set a breakpoint before entering the main loop
    std::cout << "DEBUG: About to enter main loop" << std::endl;
    bool running = true;
    SDL_Event event;
    
    std::cout << "Entering main loop" << std::endl;
    
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
                
                // Only handle mouse wheel if debugConsole is initialized
                if (debugConsoleInitialized) {
                    // Pass the mouse wheel event to the debug console
                    DebugConsole::handleMouseWheel(mouseX, mouseY, event.wheel.y);
                }
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

        // Only render debug console if it's initialized
        if (debugConsoleInitialized) {
            DebugConsole::render(renderer, 0, 450);
            DebugConsole::renderMM(renderer, 600, 450);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    cleanExit(window, renderer);
    return 0;
}
