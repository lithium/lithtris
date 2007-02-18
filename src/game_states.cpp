#include "Game.h"


namespace lithtris 
{



/*
void Game::won_state()
{
    handleExitInput();
    clearScreen();
    displayText(p_menu_font, "You won!!!", 100,120, 255,255,255, 0,0,0);
    displayText(p_menu_font, "Quit Game? (y or n)", 100,140,  255,255,255, 0,0,0);
    SDL_UpdateRect(p_window, 0, 0, 0, 0);
    SDL_Delay(timeLeftInFrame());
}
void Game::lost_state()
{
    handleLoserInput();
    clearScreen();
    displayText(p_menu_font, "LOSER!", 100,120, 255,255,255, 0,0,0);
    displayText(p_menu_font, "Now you quit.", 100,140, 255,255,255, 0,0,0);
    SDL_UpdateRect(p_window, 0, 0, 0, 0);
    SDL_Delay(timeLeftInFrame());
}
void Game::menu_state()
{
    handleMenuInput();
    clearScreen();
    displayText(p_menu_font, "(S)tart game", 120,120, 255,255,255, 0,0,0);
    displayText(p_menu_font, "(Q)uit game", 120,150, 255,255,255, 0,0,0);
    SDL_UpdateRect(p_window, 0, 0, 0, 0);
    SDL_Delay(timeLeftInFrame());
}
*/
void Game::menu_state()
{
    handleMenuInput();
    clearScreen();
    drawMenu(p_which_menu);
    SDL_UpdateRect(p_window,0,0,0,0);
    SDL_Delay(timeLeftInFrame());
}

void Game::play_state()
{
    static int force_down_counter = 0;
    static int slide_counter = SLIDE_TIME;
    if (!p_playing_music) playNextMusicTrack();

    handlePlayInput();
    if (++force_down_counter >= p_blockSpeed)
    {
        if (!checkCollisions(p_focusBlock,Down)) {
            p_focusBlock->move(Down);
            force_down_counter = 0;
        }
    }

    if (checkCollisions(p_focusBlock,Down)) {
        slide_counter--;
    }
    else {
        slide_counter = SLIDE_TIME;
    }

    if (slide_counter == 0) {
        slide_counter = SLIDE_TIME;
        if (handleBottomCollision()) 
            return;
    }

    clearScreen();
    drawBackground();

    if (p_focusBlock) p_focusBlock->draw(p_window);
    if (p_holdBlock) p_holdBlock->draw(p_window);
    if (p_shadowBlock) p_shadowBlock->draw(p_window);
    int i;
    for (i=p_nextBlocks.size()-1; i>=0; i--)
    {
        p_nextBlocks[i].block->draw(p_window);
    }

    for (i=0; i < MAX_ROWS; i++) {
        for (int c=0; c < SQUARES_PER_ROW; c++) {
            if (p_pile[i][c]) {
                p_pile[i][c]->draw(p_window);
            }
        }
    }

    drawScore();

    SDL_UpdateRect(p_window,0,0,0,0);
    SDL_Delay(timeLeftInFrame());
}

void Game::keys_state()
{
}

void Game::quit_state()
{
}

void Game::restart_state()
{
}



};
