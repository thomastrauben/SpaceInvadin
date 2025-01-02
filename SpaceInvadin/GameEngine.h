#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Player.h"
#include "Alien.h"
#include "Bullet.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "SDL.h"
#include "SDL_ttf.h"

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
    void welcomeScreen();
    void renderText(const std::string& message, const SDL_Color& color, int x, int y);
    void showHelpScreen();
    bool confirmExit();

    void saveGameState(const std::string& filename);
    bool loadGameState(const std::string& filename);

    void loadHighScore(const std::string& filename);
    void saveHighScore(const std::string& filename);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    bool spacePressed;
    bool showHelp;
    bool exitRequested;

    Player player;
    std::vector<Alien> aliens;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> alienBullets;

    int playerHealth;
    int level;
    int alienSpeed;
    int alienDirection;
    int score;      
    int highScore;

    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
};

#endif
