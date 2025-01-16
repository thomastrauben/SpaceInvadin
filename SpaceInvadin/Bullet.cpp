#include "Bullet.h"
//tworzy obiekt pocisku i jego stan poruszania
Bullet::Bullet(int startX, int startY, int width, int height)
    : x(startX), y(startY), w(width), h(height), active(true) {
}

void Bullet::move() {
    y -= 10; // porusza pocisk w  góre
    if (y < 0) {
        active = false; // unicestwia pociski poza mapą
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // kolor biały
        SDL_Rect bulletRect = { x, y, w, h };
        SDL_RenderFillRect(renderer, &bulletRect);
    }
}
