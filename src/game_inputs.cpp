#include "Game.h"
#include <time.h>
#include <sys/time.h>

namespace lithtris {

void Game::handleMenuInput()
{
    if ( SDL_PollEvent(&p_event) )
    {
        if (p_event.type == SDL_QUIT) {
            clearStates();
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
                pushState(&Game::play_state);
                return;
            }
        }
    }
}
void Game::handleExitInput()
{
    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT) {
            clearStates();
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
                restart();
                pushState(&Game::play_state);
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
    static Uint32 tick = 0;

    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT) {
            clearStates();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE) {
                p_stateStack.pop();
                return;
            }

            if (p_event.key.keysym.sym == KEY_ROTATE_LEFT) {
                if (!checkRotationCollisions(p_focusBlock, Left)) {
                    p_focusBlock->rotate(Left);
                    adjustShadowBlock();
                }
            }
            else
            if (p_event.key.keysym.sym == KEY_ROTATE_RIGHT) {
                if (!checkRotationCollisions(p_focusBlock, Right)) {
                    p_focusBlock->rotate(Right);
                    adjustShadowBlock();
                }
            }
            else
            if (p_event.key.keysym.sym == KEY_HARD_DROP) {
                hardDropFocusBlock();
            }
            else
            if (p_event.key.keysym.sym == KEY_HOLD) {
                holdFocusBlock();
            }
            else
            if (p_event.key.keysym.sym == KEY_MOVE_LEFT) 
                left_pressed = true;
            else
            if (p_event.key.keysym.sym == KEY_MOVE_RIGHT) 
                right_pressed = true;
            else
            if (p_event.key.keysym.sym == KEY_SOFT_DROP) 
                down_pressed = true;
        }
        else
        if (p_event.type == SDL_KEYUP) {
            if (p_event.key.keysym.sym == KEY_MOVE_LEFT) 
                left_pressed = false;
            else
            if (p_event.key.keysym.sym == KEY_MOVE_RIGHT) 
                right_pressed = false;
            else
            if (p_event.key.keysym.sym == KEY_SOFT_DROP) 
                down_pressed = false;
        }
    }
    Direction dir;
    if (down_pressed) dir = Down;
    else if (left_pressed) dir = Left;
    else if (right_pressed) dir = Right;
    else return;
   
    if (SDL_GetTicks() - KEYPRESS_DELAY >= tick)  {
        tick = SDL_GetTicks();
        if (!checkCollisions(p_focusBlock, dir)) {
            p_focusBlock->move(dir);
            adjustShadowBlock();
        }
    }
}

};
