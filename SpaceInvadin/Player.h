#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"

class Player {
public:
    int x, y, w, h;
    int speed;
    bool shooting;

    Player(int startX, int startY, int width, int height, int moveSpeed);
    void moveLeft();
    void moveRight();
    void render(SDL_Renderer* renderer);
};

#endif
#pragma once
