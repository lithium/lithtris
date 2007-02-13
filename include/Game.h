#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "enums.h"
#include "SDL.h"
#include "SDL_main.h"
#include "SDL_ttf.h"
#include "Square.h"
#include "Block.h"

#include <string>
#include <vector>
#include <stack>

namespace lithtris
{



class Game
{
public:
    Game();
    ~Game();

    void run();

protected:
    typedef void (Game::*StateFunction)();

    /* game_collisions.cpp*/
    bool checkCollisions(Square *square, Direction dir) { return checkWallCollisions(square,dir) || checkPileCollisions(square,dir); }
    bool checkCollisions(Block *block, Direction dir) { return checkWallCollisions(block,dir) || checkPileCollisions(block,dir); }
    bool checkWallCollisions(Square *square, Direction dir);
    bool checkPileCollisions(Square *square, Direction dir);
    bool checkPileCollisions(Block *block, Direction dir);
    bool checkWallCollisions(Block *block, Direction dir);
    bool checkRotationCollisions(Block *block, Direction dir); 
    bool checkSquareOutside(Square *square,Direction dir=Left);
    bool checkBlockOutside(Block *block,Direction dir=Left);


    bool checkWin();
    bool checkLoss(Block *block);
    int checkCompletedLines();

    /* game.cpp */
    void init();
    void shutdown();
    void reset(StateFunction statePointer=0);
    void restart();

    bool handleBottomCollision();
    bool addBlockToPile(Block *block);
    bool hardDropFocusBlock();
    void nextFocusBlock();
    void holdFocusBlock();
    void shiftNextBlocks();
    Block *getRandomBlock(int x = BLOCK_START_X, int y = BLOCK_START_Y);
    SDL_Rect getRowCol(int x, int y);
    void adjustShadowBlock();
    void updateLines();



    void drawBackground();
    void drawScore();
    void clearScreen();
    void displayText(TTF_Font *font, const char *text, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB);

    /* game_inputs.cpp */
    void handleMenuInput();
    void handlePlayInput();
    void handleExitInput();
    void handleLoserInput();

    /* game_states.cpp */

    void pushState(StateFunction state_func);
    void clearStates();

    void menu_state();
    void play_state();
    void exit_state();
    void won_state();
    void lost_state();

private:
    typedef struct nextblock_dat {
        Block *block;
        SDL_Rect rect;
    } nextblock_t;

    Block *p_focusBlock;
    Block *p_holdBlock;
    Block *p_shadowBlock;
    std::vector<nextblock_t> p_nextBlocks;

    Square *p_pile[MAX_ROWS][SQUARES_PER_ROW];
    int p_level;
    int p_lines;
    int p_blockSpeed;

    TTF_Font *p_menu_font;


    SDL_Surface *p_blocks_bitmap;
    SDL_Surface *p_skin_bitmap;

    SDL_Surface *p_window;
    SDL_Event p_event;
    int p_timer;
    std::stack<StateFunction> p_stateStack;
};

}

#endif
