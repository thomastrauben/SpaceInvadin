#include "GameEngine.h"

int main(int argc, char* argv[]) {
    GameEngine gameEngine;
    if (gameEngine.initialize()) {
        gameEngine.run();
    }
    gameEngine.cleanup();
    return 0;
}
