#include "Game.h"

namespace lithtris {

void Game::handleMenuInput()
{
    if ( SDL_PollEvent(&p_event) )
    {
        if (p_event.type == SDL_QUIT) {
            while (!p_stateStack.empty())
                p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE) {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_q) {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_s) {
                state_t tmp;
                tmp.statePointer = this::*play;
                p_stateStack.push(tmp);
                return;
            }
        }
    }
}
void Game::handleExitInput()
{
    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT)
        {
            while (!p_stateStack.empty()) p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE)
            {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_y)
            {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_n)
            {
                state_t tmp;
                tmp.statePointer = this::*play;
                p_stateStack.push(tmp);
                return;
            }
        }
    }

}

void Game::handlePlayInput()
{
    static bool down_pressed = false;
    static bool left_pressed = false;
    static bool right_pressed = false;

    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT) {
            while (!p_stateStack.empty()) p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE) {
                p_stateStack.pop();
                return;
            }

            if (p_event.key.keysym.sym == SDLK_UP) {
                if (!checkRotationCollisions(p_focusBlock))
                    p_focusBlock->rotate(Left);
            }
            else
            if (p_event.key.keysym.sym == SDLK_LEFT) 
                left_pressed = true;
            else
            if (p_event.key.keysym.sym == SDLK_RIGHT) 
                right_pressed = true;
            else
            if (p_event.key.keysym.sym == SDLK_DOWN) 
                down_pressed = true;
        }
        if (p_event.type == SDL_KEYUP) {
            if (p_event.key.keysym.sym == SDLK_LEFT) 
                left_pressed = false;
            else
            if (p_event.key.keysym.sym == SDLK_RIGHT) 
                right_pressed = false;
            else
            if (p_event.key.keysym.sym == SDLK_DOWN) 
                down_pressed = false;
        }
    }
    Direction dir;
    if (down_pressed) dir = Down;
    else if (left_pressed) dir = Left;
    else if (right_pressed) dir = Right;

    if (!checkWallCollisions(p_focusBlock, dir) && !checkEntityCollisions(p_focusBlock,dir))
        p_focusBlock->move(dir);

}

};
