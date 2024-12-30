#include "Bullet.h"

Bullet::Bullet(int startX, int startY, int width, int height)
    : x(startX), y(startY), w(width), h(height), active(true) {
}

void Bullet::move() {
    y -= 10; // Move bullet upwards
    if (y < 0) {
        active = false; // Deactivate bullet if it goes off-screen
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White bullet
        SDL_Rect bulletRect = { x, y, w, h };
        SDL_RenderFillRect(renderer, &bulletRect);
    }
}
