// main_window.cpp
#include <SDL2/SDL.h>
#include <iostream>
#include "power_button.hpp"
#include "pin_sim.hpp"
#include "debug_console.hpp"
#include "controller_manager.hpp"
ControllerManager controller;

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    DebugCoutRedirect redirect;
    DebugConsole::init("C:/Windows/Fonts/consola.ttf", 14);
    DebugConsole::setActiveMMTag("PC1");
    std::cout << "[boot] Debug console initialized" << std::endl;
    std::cout << "[PC1] MM logging active for PC1" << std::endl;
    std::cout << "This line should not appear in MM (untagged)" << std::endl;
    std::cout << "[PC2] This line will be buffered but not shown" << std::endl;

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

        controller.tick(0); // or pass frame count

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // --- TEST INPUTS PANEL ---
        SDL_Rect testPanel = {0, 0, 300, 450};
        SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255); // Dark Blue
        SDL_RenderFillRect(renderer, &testPanel);

        // --- DEBUG CONSOLE ---
        SDL_Rect debugConsole = {0, 450, 600, 350};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark Gray
        SDL_RenderFillRect(renderer, &debugConsole);

        // --- MULTIPURPOSE MONITOR ---
        SDL_Rect mmPanel = {600, 450, 600, 350};
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Very Dark Gray
        SDL_RenderFillRect(renderer, &mmPanel);

        // --- SPLITTER ---
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        SDL_RenderDrawLine(renderer, 600, 450, 600, 800);

        // --- MAIN PANEL SIMULATION ZONE ---
        SDL_Rect mainPanel = {300, 0, 900, 450};
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &mainPanel);

        masterButton.render(renderer);

        // --- DEBUG CONSOLE TEXT ---
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