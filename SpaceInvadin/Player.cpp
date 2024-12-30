#include "Player.h"

Player::Player(int startX, int startY, int width, int height, int moveSpeed)
    : x(startX), y(startY), w(width), h(height), speed(moveSpeed), shooting(false) {
}

void Player::moveLeft() {
    if (x > 0) {
        x -= speed;
    }
}

void Player::moveRight() {
    if (x + w < 800) { // Assuming screen width is 800
        x += speed;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect playerRect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &playerRect);
}
