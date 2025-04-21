// main_window.cpp

#include <windows.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include "power_button.hpp"
#include "../bus/pin_sim.hpp" // Updated include path for pin_sim.hpp
#include "debug_console.hpp" // Updated include path for debug_console.hpp

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

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize debug console and redirect std::cout
    DebugCoutRedirect redirect;
    DebugConsole::init("C:/Windows/Fonts/consola.ttf", 14);
    DebugConsole::setActiveMMTag("BOOT");

    // Boot messages
    std::cout << "[BOOT] Fidget Reactor UI initializing..." << std::endl;
    std::cout << "[BOOT] Launching ESP32 simulation..." << std::endl;
    launchProcess("build\\Debug\\controller_runner.exe");

    std::cout << "[BOOT] Launching PHC controller: phc_a..." << std::endl;
    launchProcess("build\\Debug\\phc.exe config\\components\\phc_a.json");

    // Load UI window
    SDL_Window* window = SDL_CreateWindow("Fidget Reactor UI",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL Create Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL Create Renderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
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

    DebugConsole::shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
