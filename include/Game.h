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


    void finishMatch();
    bool checkWin();
    bool checkLoss(Block *block);
    int checkCompletedLines();

    /* game.cpp */
    void init();
    void shutdown();
    void reset(StateFunction statePointer=0);
    void restart(int level=1);

    void pushState(StateFunction state_func);
    void clearStates();

    void pauseMusic();
    void haltMusic();
    void playNextMusicTrack();
    bool handleBottomCollision();
    bool addBlockToPile(Block *block);
    bool hardDropFocusBlock();
    void nextFocusBlock();
    void holdFocusBlock();
    void shiftNextBlocks();
    void shuffle_bag(BlockType*,int);
    Block *getRandomBlock(int x = BLOCK_START_X, int y = BLOCK_START_Y);
    SDL_Rect getRowCol(int x, int y);
    void adjustShadowBlock();
    void updateLines();
    int calcScore(int nlines, int level, int combo=1, bool b2b=false, int cacasde=1);

    void init_random();
    int get_random(int min,int max);

    unsigned int timeLeftInFrame();


    SDL_Rect backgroundRectFromLevel(int level);

    void drawPlayScreen();
    void drawKeysPrompt();
    void drawBackground();
    void drawBlendedBackground(float fade_secs);
    void drawScore();
    void drawLineTransition();
    void clearScreen();
    void drawMenu(MenuId which);
    SDL_Rect displayText(TTF_Font *font, const char *text, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB, bool transparent=true, int alpha=255, Direction align=Down);

    /* game_inputs.cpp */
    int checkWallKick(Block *block, Direction dir);
    void handleMenuInput();
    void handlePlayInput();
    void handleKeysInput();

    /* game_states.cpp */


    void menu_state();
    void play_state();
    void keys_state();
    void quit_state();
    void restart_state();
    void toggle_state();
    void stopplaying_state();


    typedef struct hiscore_dat {
        int highest_level;
        int highest_score;
        int highest_combo;
        int highest_line_score;
        int longest_time; // in seconds
        int most_lines;
        int most_cascade;
    } hiscores_t;

    bool write_scores(const char *filename, hiscores_t &scores);
    bool read_scores(const char *filename, hiscores_t &scores);
private:
    std::string p_score_filename;
    hiscores_t p_hiscore;
    int p_match_start;
    int p_match_end;

    bool p_has_audio;
    int p_rand_fd;

    std::map<KeyId, SDLKey> p_keymap;

    std::stack<DrawState> p_draw_state;

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
    bool p_music_paused;

    bool p_music_on;
    bool p_hold_on;
    bool p_shadow_on;
    bool p_cascade_on;
    bool p_infspin_on;
    bool p_showscore_on;

    Uint32 p_spin_ticks;

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
    int p_score;

    int p_blockSpeed;
    int p_hold_locked;
    bool p_last_tetris;
    bool p_b2b;
    int p_combo;
    int p_cascade;

    const char *p_line_message;
    int p_line_alpha;

    std::map<int,bool> p_completed_rows;

    SDL_Surface *p_blocks_bitmap;
    SDL_Surface *p_levels_bitmap;

    SDL_Surface *p_window;
    SDL_Event p_event;
    std::stack<StateFunction> p_stateStack;
};

}

#endif
