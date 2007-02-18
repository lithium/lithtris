#include "Game.h"
#include <time.h>
#include <sys/time.h>

namespace lithtris {
#define pekksym p_event.key.keysym.sym

void Game::handleMenuInput()
{
    if ( SDL_PollEvent(&p_event) )
    {
        if (p_event.type == SDL_QUIT) {
            clearStates();
            return;
        }

        if (p_event.type != SDL_KEYDOWN) return;
        if (pekksym == SDLK_ESCAPE) {
            p_stateStack.pop();
            return;
        }

        if ((pekksym == SDLK_DOWN) || (pekksym == p_keymap[SoftDropKey]) || (pekksym == p_keymap[MoveLeftKey])) {
            if (p_menu_item >= p_menu[p_which_menu].size()) 
                p_menu_item = 0;
            else 
                p_menu_item++;
            return;
        }
        if ((pekksym == SDLK_UP) || (pekksym == p_keymap[HardDropKey]) || (pekksym == p_keymap[MoveRightKey])) {
            if (--p_menu_item < 0) {
                p_menu_item = p_menu[p_which_menu].size();
            }
            return;
        }

        if ((pekksym == SDLK_RETURN) || (pekksym == p_keymap[RotLeftKey]) || (pekksym == p_keymap[RotRightKey]) || (pekksym == p_keymap[HoldKey])) {
            if (p_menu_item >= p_menu[p_which_menu].size()) return;
            const menuitem_t &mit = p_menu[p_which_menu].at(p_menu_item);
            p_which_menu = mit.menu;
            pushState(mit.state);
            return;
        }
    }
}
/*
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
void Game::handleLoserInput()
{
    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT) {
            clearStates();
            return;
        }

        if (p_event.type == SDL_KEYDOWN)  {
            clearStates();
            return;
        }
    }

}*/

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
            if (pekksym == SDLK_ESCAPE) {
                haltMusic();
                p_which_menu = MainMenu;
                p_stateStack.pop();
                return;
            }

            if (pekksym == p_keymap[RotLeftKey]) {
                if (!checkRotationCollisions(p_focusBlock, Left)) {
                    p_focusBlock->rotate(Left);
                    adjustShadowBlock();
                }
            }
            else
            if (pekksym == p_keymap[RotRightKey]) {
                if (!checkRotationCollisions(p_focusBlock, Right)) {
                    p_focusBlock->rotate(Right);
                    adjustShadowBlock();
                }
            }
            else
            if (pekksym == p_keymap[HardDropKey]) {
                if (hardDropFocusBlock())
                    return;
            }
            else
            if (pekksym == p_keymap[HoldKey]) {
                holdFocusBlock();
            }
            else
            if (pekksym == p_keymap[MoveLeftKey]) 
                left_pressed = true;
            else
            if (pekksym == p_keymap[MoveRightKey])
                right_pressed = true;
            else
            if (pekksym == p_keymap[SoftDropKey])
                down_pressed = true;
        }
        else
        if (p_event.type == SDL_KEYUP) {
            if (pekksym == p_keymap[MoveLeftKey])
                left_pressed = false;
            else
            if (pekksym == p_keymap[MoveRightKey])
                right_pressed = false;
            else
            if (pekksym == p_keymap[SoftDropKey])
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

#undef pekksym 
};
