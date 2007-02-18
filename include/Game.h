#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "enums.h"
#include "SDL.h"
#include "SDL_main.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "Square.h"
#include "Block.h"

#include <string>
#include <vector>
#include <stack>
#include <map>

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

    void pushState(StateFunction state_func);
    void clearStates();

    void haltMusic();
    void playNextMusicTrack();
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

    void init_random();
    int get_random(int min,int max);

    unsigned int timeLeftInFrame();


    void drawBackground();
    void drawScore();
    void clearScreen();
    void drawMenu(MenuId which);
    void displayText(TTF_Font *font, const char *text, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB);

    /* game_inputs.cpp */
    void handleMenuInput();
    void handlePlayInput();
    void handleKeysInput();

    /* game_states.cpp */


    void menu_state();
    void play_state();
    void keys_state();
    void quit_state();
    void restart_state();


private:
    bool p_has_audio;
    int p_rand_fd;

    std::map<KeyId, SDLKey> p_keymap;

    MenuId p_which_menu;
    int p_menu_item;

    typedef struct menuitem_dat {
        const char *label;
        StateFunction state;
        MenuId menu;
    } menuitem_t;
    std::map<MenuId, std::vector<menuitem_t> > p_menu;
    std::map<MenuId, SDL_Surface *> p_menu_titles;
    TTF_Font *p_menu_font;

    Mix_Music *p_music[10];
    bool p_playing_music;

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


    SDL_Surface *p_blocks_bitmap;
    SDL_Surface *p_levels_bitmap;

    SDL_Surface *p_window;
    SDL_Event p_event;
    std::stack<StateFunction> p_stateStack;
};

}

#endif
