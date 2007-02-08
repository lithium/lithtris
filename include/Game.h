#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "enums.h"
#include "SDL.h"
#include "SDLmain.h"
#include "SDL_TTF.h"

namespace lithtris
{


class Game
{
public:
    Game();
    ~Game();

    void run() { play(); }

protected:

    bool checkEntityCollisions(Square *square, Direction dir);
    bool checkWallCollisions(Square *square, Direction dir);
    bool checkEntityCollisions(Block *block, Direction dir);
    bool checkWallCollisions(Block *block, Direction dir);
    bool checkRotationCollisions(Block *block); 
    bool checkWin();
    bool checkLoss();

    void handleBottomCollision();
    void nextFocusBlock();
    int checkCompletedLines();

    void fillNextBlocks();

    void init();
    void shutdown();
    void win();
    void lose();

    void drawBackground();
    void clearScreen();
    void displayText(std::string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB);
    void handleMenuInput();
    void handlePlayInput();
    void handleExitInput();

    typedef state_dat {
        void (*statePointer)();
    } state_t;
/*states*/
    void menu();
    void play();
    void exit();

private:
    Block *p_focusBlock;
    std::vector<Block*> p_nextBlocks;

    std::vector<Square*> p_oldSquares;
    int p_level;
    int p_blockSpeed;
    int p_num_next_blocks;

    SDL_Surface *p_bitmap;
    SDL_Surface *p_window;
    SDL_Event p_event;
    int p_timer;
    std::stack<state_t> p_stateStack;
};

}

#endif
