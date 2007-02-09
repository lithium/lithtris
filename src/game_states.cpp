#include "Game.h"


namespace lithtris 
{

void Game::won_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleExitInput();
        clearScreen();
        displayText("You won!!!", 100, 120, 12, 255,255,255,0,0,0);
        displayText("Quit Game? (y or n)", 100, 140, 12, 255,255,255, 0,0,0);
        SDL_UpdateRect(p_window, 0, 0, 0, 0);
        p_timer = SDL_GetTicks();
    }
}
void Game::lost_state()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleExitInput();
        clearScreen();
        displayText("LOSER", 100, 120, 12, 255,255,255,0,0,0);
        displayText("Quit Game? (y or n)", 100, 140, 12, 255,255,255, 0,0,0);
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
        displayText("(S)tart game", 120, 120, 12, 255, 255, 255, 0, 0, 0);
        displayText("(Q)uit game", 120, 150, 12, 255, 255, 255, 0, 0, 0);
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
        displayText("Quit Game? (y or n)", 100, 150, 255, 255,255, 0,0,0);
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
        if (++force_down_counter >= p_focusBlockSpeed)
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
            handleBottomCollision();
        }

        clearScreen();
        drawBackground();

        p_focusBlock->draw(p_window);
        int i;
        for (i=0; i < p_nextBlocks.size(); i++)
        {
            p_nextBlocks[i].block->draw(p_window);
        }

        for (int r=0; r < MAX_ROWS; r++) {
            for (int c=0; c < SQUARES_PER_ROW; c++) {
                p_pile[r][c]->draw(p_window);
            }
        }

        drawScore();

        SDL_UpdateRect(p_window,0,0,0,0);
        p_timer = SDL_GetTicks();
    }
}

void Game::pushState(StateFunction state_func)
{
    state_t tmp;
    tmp.statePointer = state_func;
    p_stateStack.push(tmp);
}

void Game::clearStates()
{
    while (!p_stateStack.empty()) 
        p_stateStack.pop();
}


};
