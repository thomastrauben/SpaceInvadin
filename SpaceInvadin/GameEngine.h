#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Player.h"
#include "Alien.h"
#include "Bullet.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include "SDL.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool initialize();
    void run();
    void cleanup();

private:
    void processInput();
    void update();
    void render();
    void resetAliens();
    void alienFire();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    bool spacePressed;

    Player player;
    std::vector<Alien> aliens;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> alienBullets;

    int playerHealth;
    int level;
    int alienSpeed;
    int alienDirection;
};

#endif
