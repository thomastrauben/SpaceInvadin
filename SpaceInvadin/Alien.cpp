#include "Alien.h"

Alien::Alien(int startX, int startY, int width, int height)
    : x(startX), y(startY), w(width), h(height), active(true) {
}

void Alien::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect alienRect = { x, y, w, h };
        SDL_RenderFillRect(renderer, &alienRect);
    }
}

void Alien::move(int dx) {
    x += dx;
}
