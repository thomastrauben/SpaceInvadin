#include "GameEngine.h"
#include <algorithm>
#include <iostream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

GameEngine::GameEngine()
    : window(nullptr), renderer(nullptr), running(true), gameOver(false),
    spacePressed(false), player(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 20, 5),
    playerHealth(3), level(1), alienSpeed(1), alienDirection(1) {
    std::srand(std::time(nullptr)); // Seed random number generator
}

GameEngine::~GameEngine() {}

bool GameEngine::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Space Invader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    resetAliens();
    return true;
}

void GameEngine::run() {
    while (running) {
        processInput();
        if (!gameOver) {
            update();
        }
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void GameEngine::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameEngine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (!gameOver && event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_LEFT) {
                player.moveLeft();
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                player.moveRight();
            }
            if (event.key.keysym.sym == SDLK_SPACE && !spacePressed) {
                playerBullets.emplace_back(player.x + player.w / 2 - 5, player.y - 10, 5, 10);
                spacePressed = true;
            }
        }

        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                spacePressed = false;
            }
        }
    }
}

void GameEngine::update() {
    // Player bullets
    for (auto& bullet : playerBullets) {
        bullet.move();
    }
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [](const Bullet& b) { return !b.active; }),
        playerBullets.end());

    // Alien bullets
    for (auto& bullet : alienBullets) {
        bullet.y += 4; // Alien bullets move downward
        if (bullet.y > SCREEN_HEIGHT) {
            bullet.active = false;
        }
    }
    alienBullets.erase(std::remove_if(alienBullets.begin(), alienBullets.end(),
        [](const Bullet& b) { return !b.active; }),
        alienBullets.end());

    // Alien movement
    bool changeDirection = false;
    for (auto& alien : aliens) {
        alien.move(alienDirection * alienSpeed);
        if (alien.x <= 0 || alien.x + alien.w >= SCREEN_WIDTH) {
            changeDirection = true;
        }
    }

    if (changeDirection) {
        alienDirection *= -1;
        for (auto& alien : aliens) {
            alien.y += 10;
            if (alien.active && alien.y + alien.h >= player.y && alien.x < player.x + player.w && alien.x + alien.w > player.x) {
                gameOver = true;
            }
        }
    }

    // Alien shooting
    alienFire();

    // Collision detection: player bullets vs aliens
    for (auto& bullet : playerBullets) {
        for (auto& alien : aliens) {
            if (bullet.active && alien.active &&
                bullet.x < alien.x + alien.w && bullet.x + bullet.w > alien.x &&
                bullet.y < alien.y + alien.h && bullet.y + bullet.h > alien.y) {
                bullet.active = false;
                alien.active = false;
            }
        }
    }

    // Collision detection: alien bullets vs player
    for (auto& bullet : alienBullets) {
        if (bullet.active && bullet.x < player.x + player.w && bullet.x + bullet.w > player.x &&
            bullet.y < player.y + player.h && bullet.y + bullet.h > player.y) {
            bullet.active = false;
            playerHealth--;
            if (playerHealth <= 0) {
                gameOver = true;
            }
        }
    }

    // Check if all aliens are killed
    if (std::all_of(aliens.begin(), aliens.end(), [](const Alien& a) { return !a.active; })) {
        level++;
        alienSpeed++;
        resetAliens();
    }
}

void GameEngine::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (!gameOver) {
        player.render(renderer);

        for (auto& alien : aliens) {
            alien.render(renderer);
        }

        for (auto& bullet : playerBullets) {
            bullet.render(renderer);
        }

        for (auto& bullet : alienBullets) {
            bullet.render(renderer);
        }

        // Render player health (vertical bars)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < playerHealth; ++i) {
            SDL_Rect healthBar = { 10 + (i * 15), 50, 10, 100 }; // Vertical bars for health
            SDL_RenderFillRect(renderer, &healthBar);
        }

        // Render level indicator (vertical bars)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < level; ++i) {
            SDL_Rect levelBar = { 10 + (i * 15), 10, 10, 30 }; // Vertical bars for difficulty
            SDL_RenderFillRect(renderer, &levelBar);
        }
    }
    else {
        // Game over screen
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect gameOverRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100 };
        SDL_RenderFillRect(renderer, &gameOverRect);
    }

    SDL_RenderPresent(renderer);
}

void GameEngine::resetAliens() {
    aliens.clear();
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            aliens.emplace_back(i * 100 + 50, j * 50 + 50, 40, 20);
        }
    }
}

void GameEngine::alienFire() {
    if (std::rand() % 100 < 5) { // 5% chance per frame
        int shooterIndex = std::rand() % aliens.size();
        if (aliens[shooterIndex].active) {
            alienBullets.emplace_back(aliens[shooterIndex].x + aliens[shooterIndex].w / 2 - 2, aliens[shooterIndex].y + aliens[shooterIndex].h, 5, 10);
        }
    }
}
