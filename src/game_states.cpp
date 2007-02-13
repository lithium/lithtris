#include "Game.h"


namespace lithtris 
{

void Game::won_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleExitInput();
        clearScreen();
        displayText(p_menu_font, "You won!!!", 100,120, 255,255,255, 0,0,0);
        displayText(p_menu_font, "Quit Game? (y or n)", 100,140,  255,255,255, 0,0,0);
        SDL_UpdateRect(p_window, 0, 0, 0, 0);
        p_timer = SDL_GetTicks();
    }
}
void Game::lost_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleLoserInput();
        clearScreen();
        displayText(p_menu_font, "LOSER!", 100,120, 255,255,255, 0,0,0);
        displayText(p_menu_font, "Now you quit.", 100,140, 255,255,255, 0,0,0);
        SDL_UpdateRect(p_window, 0, 0, 0, 0);
        p_timer = SDL_GetTicks();
    }
}
void Game::menu_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleMenuInput();
        clearScreen();
        displayText(p_menu_font, "(S)tart game", 120,120, 255,255,255, 0,0,0);
        displayText(p_menu_font, "(Q)uit game", 120,150, 255,255,255, 0,0,0);
        SDL_UpdateRect(p_window, 0, 0, 0, 0);
        p_timer = SDL_GetTicks();
    }
}

void Game::exit_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleExitInput();
        clearScreen();
        displayText(p_menu_font, "Quit Game? (y or n)", 100, 150, 255,255,255, 0,0,0);
        SDL_UpdateRect(p_window,0,0,0,0);
        p_timer = SDL_GetTicks();
    }
}

void Game::play_state()
{
    static int force_down_counter = 0;
    static int slide_counter = SLIDE_TIME;

    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
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
        p_timer = SDL_GetTicks();
    }
}

void Game::pushState(StateFunction state_func)
{
    p_stateStack.push(state_func);
}

void Game::clearStates()
{
    while (!p_stateStack.empty()) 
        p_stateStack.pop();
}


};
