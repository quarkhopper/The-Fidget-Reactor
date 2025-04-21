// power_button.cpp
#include "power_button.hpp"
#include "../bus/pin_sim.hpp" // Updated include path for pin_sim.hpp
#include <SDL2/SDL.h>

PowerButton::PowerButton(int x, int y, int w, int h)
    : rect{x, y, w, h}, isOn(false), lastClickTime(0) {}

void PowerButton::handleClick(int mouseX, int mouseY) {
    Uint32 now = SDL_GetTicks();
    SDL_Point p{mouseX, mouseY};
    if (SDL_PointInRect(&p, &rect)) {
        // Simulate debounce: ignore clicks within 200 ms
        if (now - lastClickTime > 200) {
            isOn = !isOn;
            emitPinState("MASTER", isOn);
            lastClickTime = now;
        }
    }
}

void PowerButton::render(SDL_Renderer* renderer) {
    if (isOn) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for ON
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for OFF
    }
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
