#ifndef BULLET_H
#define BULLET_H

#include "SDL.h"

class Bullet {
public:
    int x, y, w, h;
    bool active;

    Bullet(int startX, int startY, int width, int height);
    void move();
    void render(SDL_Renderer* renderer);
};

#endif
#pragma once
