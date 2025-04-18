// power_button.hpp
#pragma once
#include <SDL2/SDL.h>

class PowerButton {
public:
    PowerButton(int x, int y, int w, int h);
    void handleClick(int mouseX, int mouseY);
    void render(SDL_Renderer* renderer);

private:
    SDL_Rect rect;
    bool isOn;
    Uint32 lastClickTime;
};
