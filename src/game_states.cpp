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
            if (!checkWallCollisions(p_focusBlock, Down) && !checkEntityCollisions(p_focusBlock, Down)) {
                p_focusBlock->move(Down);
                force_down_counter = 0;
            }
        }

        if (checkWallCollisions(p_focusBlock, Down) || checkEntityCollsions(p_focusBlock, Down) )
        {
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
        p_nextBlock->draw(p_window);

        for (int i=0; i < p_oldSquares.size(); i++)
        {
            p_oldSquares[i]->draw(p_window);
        }

        /* XXX draw level texts */


        SDL_UpdateRect(p_window,0,0,0,0);
        p_timer = SDL_GetTicks();
    }
}


};
