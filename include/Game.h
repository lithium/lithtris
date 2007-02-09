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

    void run();

protected:
    /* game_collisions.cpp*/
    bool checkEntityCollisions(Square *square, Direction dir);
    bool checkWallCollisions(Square *square, Direction dir);
    bool checkEntityCollisions(Block *block, Direction dir);
    bool checkWallCollisions(Block *block, Direction dir);
    bool checkRotationCollisions(Block *block); 
    bool checkWin();
    bool checkLoss();
    int checkCompletedLines();

    /* game.cpp */
    void init();
    void shutdown();
    void reset(statePointer);

    void handleBottomCollision();
    void changeFocusBlock();
    void holdFocusBlock();
    void drawBackground();
    void clearScreen();
    void displayText(std::string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB);

    /* game_inputs.cpp */
    void handleMenuInput();
    void handlePlayInput();
    void handleExitInput();

    /* game_states.cpp */
    typedef state_dat {
        void (*statePointer)();
    } state_t;
    void menu_state();
    void play_state();
    void exit_state();
    void won_state();
    void lost_state();

private:
    Block *p_focusBlock;
    Block *p_nextBlock;
    Block *p_holdBlock;

    std::vector<Square*> p_oldSquares;
    int p_level;
    int p_score; // number of lines cleared
    int p_blockSpeed;

    SDL_Surface *p_bitmap;
    SDL_Surface *p_window;
    SDL_Event p_event;
    int p_timer;
    std::stack<state_t> p_stateStack;
};

}

#endif
