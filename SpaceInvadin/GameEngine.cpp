#include "GameEngine.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

GameEngine::GameEngine()
    : window(nullptr), renderer(nullptr), running(true), gameOver(false),
    spacePressed(false), showHelp(false), exitRequested(false),
    player(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 20, 5),
    playerHealth(3), level(1), alienSpeed(1), alienDirection(1) {
    std::srand(std::time(nullptr));
}

GameEngine::~GameEngine() {}

bool GameEngine::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize: %s", TTF_GetError());
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
    if (!loadGameState("save.txt")) {
        resetAliens();
    }

    score = 0; 
    loadHighScore("highscore.txt"); 


   // resetAliens();
    return true;
}

void GameEngine::run() {
    welcomeScreen();
    while (running) {
        processInput();

        if (showHelp) {
            showHelpScreen(); 
            SDL_Delay(16);      
            continue;         
        }

        if (!gameOver && !showHelp) {
            update();
        }

        if (gameOver) {
            if (score > highScore) {
                highScore = score;
                saveHighScore("highscore.txt");
            }
        }

        render();
        SDL_Delay(16);
    }
}


void GameEngine::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
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
            if (event.key.keysym.sym == SDLK_q) {
                showHelp = !showHelp; 
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                if (confirmExit()) {
                    running = false;
                }
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
    for (auto& bullet : playerBullets) {
        bullet.move();
    }
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [](const Bullet& b) { return !b.active; }),
        playerBullets.end());

    for (auto& bullet : alienBullets) {
        bullet.y += 4;
        if (bullet.y > SCREEN_HEIGHT) {
            bullet.active = false;
        }
    }
    alienBullets.erase(std::remove_if(alienBullets.begin(), alienBullets.end(),
        [](const Bullet& b) { return !b.active; }),
        alienBullets.end());

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

    alienFire();

    for (auto& bullet : playerBullets) {
        for (auto& alien : aliens) {
            if (bullet.active && alien.active &&
                bullet.x < alien.x + alien.w && bullet.x + bullet.w > alien.x &&
                bullet.y < alien.y + alien.h && bullet.y + bullet.h > alien.y) {
                bullet.active = false;
                alien.active = false;
                score += 10;
            }
        }
    }

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

    if (std::all_of(aliens.begin(), aliens.end(), [](const Alien& a) { return !a.active; })) {
        level++;
        alienSpeed++;
        resetAliens();
    }
}

void GameEngine::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameOver) {
        SDL_Color red = { 255, 0, 0, 255 };
        renderText("Game Over!", red, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50);
        renderText("Your Score: " + std::to_string(score), red, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2);
        renderText("High Score: " + std::to_string(highScore), red, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50);
    }
    else if (showHelp) {
        showHelpScreen();
    }
    else {
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

        for (int i = 0; i < playerHealth; ++i) {
            SDL_Rect healthBar = { 10, 50 + (i * 20), 10, 10 };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &healthBar);
        }

        SDL_Color white = { 255, 255, 255, 255 };
        renderText("Level: " + std::to_string(level), white, 10, 10);
    }

    SDL_RenderPresent(renderer);
}


void GameEngine::resetAliens() {
    aliens.clear();
    int rows = (level <= 2) ? 3 : (level <= 4) ? 5 : 6;
    int speedIncrement = (level <= 2) ? 0 : (level <= 4) ? 1 : 2;

    alienSpeed = 1 + speedIncrement;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < 5; ++j) {
            aliens.emplace_back(j * 100 + 50, i * 50 + 50, 40, 20);
        }
    }
}

void GameEngine::alienFire() {
    if (std::rand() % 100 < 5) {
        int shooterIndex = std::rand() % aliens.size();
        if (aliens[shooterIndex].active) {
            alienBullets.emplace_back(aliens[shooterIndex].x + aliens[shooterIndex].w / 2 - 2, aliens[shooterIndex].y + aliens[shooterIndex].h, 5, 10);
        }
    }
}

void GameEngine::welcomeScreen() {
    bool inWelcomeScreen = true;

    SDL_Color white = { 255, 255, 255, 255 };
    while (inWelcomeScreen) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                inWelcomeScreen = false;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                inWelcomeScreen = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderText("Press Enter to Start", white, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 50);
        SDL_RenderPresent(renderer);
    }
}

void GameEngine::showHelpScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = { 255, 255, 255, 255 };
    renderText("Help Screen", white, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 150);
    renderText("Arrow Keys: Move", white, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100);
    renderText("Space: Shoot", white, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 70);
    renderText("Press F1/Esc (my f1 key doesnt work) to Resume", white, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40);

    SDL_RenderPresent(renderer);
}


//bool GameEngine::confirmExit() {
  //  SDL_Color white = { 255, 255, 255, 255 };
    //renderText("Are you sure you want to quit?", white, SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50);
    //renderText("Press Y to Quit, N to Resume", white, SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2);
    //return true;
//}

void GameEngine::renderText(const std::string& message, const SDL_Color& color, int x, int y) {
    TTF_Font* font = TTF_OpenFont("res/arial.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        TTF_CloseFont(font);
        SDL_Log("Failed to create surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);

    if (texture) {
        SDL_Rect textRect = { x, y, 300, 50 };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
    }
    else {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
    }
}

void GameEngine::saveGameState(const std::string& filename) {
    std::ofstream saveFile(filename);
    if (!saveFile) {
        SDL_Log("Failed to open save file: %s", filename.c_str());
        return;
    }

    saveFile << "Player " << player.x << " " << player.y << " " << playerHealth << "\n";

    saveFile << "Level " << level << "\n";
    saveFile << "AlienSpeed " << alienSpeed << "\n";
    saveFile << "AlienDirection " << alienDirection << "\n";

    saveFile << "Aliens " << aliens.size() << "\n";
    for (const auto& alien : aliens) {
        saveFile << alien.x << " " << alien.y << " " << alien.active << "\n";
    }

    saveFile.close();
    SDL_Log("Game state saved to %s", filename.c_str());
}

bool GameEngine::confirmExit() {
    SDL_Color white = { 255, 255, 255, 255 };
    renderText("Exit? Press Y to save and exit, N to cancel", white, SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_y) {
                    saveGameState("save.txt"); // Save game state before exiting
                    return true;
                }
                if (event.key.keysym.sym == SDLK_n) {
                    return false;
                }
            }
        }
    }
}

bool GameEngine::loadGameState(const std::string& filename) {
    std::ifstream loadFile(filename);
    if (!loadFile) {
        SDL_Log("Failed to open save file: %s", filename.c_str());
        return false;
    }

    std::string line, label;
    int alienCount;

    loadFile >> label >> player.x >> player.y >> playerHealth;

    loadFile >> label >> level;
    loadFile >> label >> alienSpeed;
    loadFile >> label >> alienDirection;

    loadFile >> label >> alienCount;
    aliens.clear();
    for (int i = 0; i < alienCount; ++i) {
        int x, y, active;
        loadFile >> x >> y >> active;
        aliens.emplace_back(x, y, 40, 20);
        aliens.back().active = active;
    }

    loadFile.close();
    SDL_Log("Game state loaded from %s", filename.c_str());
    return true;
}

void GameEngine::loadHighScore(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    else {
        highScore = 0;
    }
}

void GameEngine::saveHighScore(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

