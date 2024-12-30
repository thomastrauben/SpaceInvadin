#ifndef ALIEN_H
#define ALIEN_H

#include "SDL.h"

class Alien {
public:
    int x, y, w, h;
    bool active;

    Alien(int startX, int startY, int width, int height);
    void render(SDL_Renderer* renderer);
    void move(int dx);
};

#endif
#pragma once
