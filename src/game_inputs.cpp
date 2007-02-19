#include "Game.h"
#include <time.h>
#include <sys/time.h>

namespace lithtris {
#define pekksym p_event.key.keysym.sym

void Game::handleKeysInput() {
    if ( SDL_PollEvent(&p_event) )
    {
        if (p_event.type == SDL_QUIT) {
            clearStates();
            return;
        }
        if (p_event.type != SDL_KEYDOWN) return;

        if (pekksym != SDLK_ESCAPE) {
            std::map<KeyId,SDLKey>::iterator it;
            bool used = false;
            for (it=p_keymap.begin(); it!=p_keymap.end(); it++) {
                if (pekksym == it->second) {
                    used = true; break;
                }
            }

            if (!used)
            switch (p_which_menu) {
                case MoveLeftToggle: p_keymap[MoveLeftKey] = pekksym; break;
                case MoveRightToggle: p_keymap[MoveRightKey] = pekksym; break;
                case SoftDropToggle: p_keymap[SoftDropKey] = pekksym; break;
                case HardDropToggle: p_keymap[HardDropKey] = pekksym; break;
                case SpinLeftToggle: p_keymap[RotLeftKey] = pekksym; break;
                case SpinRightToggle: p_keymap[RotRightKey] = pekksym; break;
                case HoldKeyToggle: p_keymap[HoldKey] = pekksym; break;
            }
        }

        p_which_menu = KeysMenu;
        p_stateStack.pop();
    }
}

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
            p_which_menu = ExitMenu;
            clearStates();
            pushState(&Game::menu_state);
            return;
        }

        if ((pekksym == SDLK_RETURN) || (pekksym == p_keymap[RotLeftKey]) || (pekksym == p_keymap[RotRightKey]) || (pekksym == p_keymap[HoldKey])) {
            if (p_menu_item >= p_menu[p_which_menu].size()) return;
            const menuitem_t &mit = p_menu[p_which_menu].at(p_menu_item);
            p_which_menu = mit.menu;
            if (mit.menu < NumMenuIds) p_menu_item = 0; // hack
            pushState(mit.state);
            return;
        }

        if ((pekksym == SDLK_DOWN) || (pekksym == p_keymap[SoftDropKey]) || (pekksym == p_keymap[MoveLeftKey])) {
            if (p_menu_item >= p_menu[p_which_menu].size()-1) 
                p_menu_item = 0;
            else 
                p_menu_item++;
            return;
        }
        if ((pekksym == SDLK_UP) || (pekksym == p_keymap[HardDropKey]) || (pekksym == p_keymap[MoveRightKey])) {
            if (--p_menu_item < 0) {
                p_menu_item = p_menu[p_which_menu].size()-1;
            }
            return;
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
            if (pekksym == SDLK_ESCAPE) {
                pauseMusic();
                p_which_menu = PauseMenu;
                pushState(&Game::menu_state);
                return;
            }

            if (pekksym == p_keymap[RotLeftKey]) {
                if (p_focusBlock->type() == SquareBlock) return; // HACK
                p_spin_ticks = SDL_GetTicks();
                if (!checkRotationCollisions(p_focusBlock, Left)) {
                    p_focusBlock->rotate(Left);
                    adjustShadowBlock();
                }
            }
            else
            if (pekksym == p_keymap[RotRightKey]) {
                    p_spin_ticks = SDL_GetTicks();
                if (!checkRotationCollisions(p_focusBlock, Right)) {
                    p_focusBlock->rotate(Right);
                    adjustShadowBlock();
                }
            }
            else
            if (pekksym == p_keymap[HardDropKey]) {
                int nrows = (p_shadowBlock->p_centerY - p_focusBlock->p_centerY) / (SQUARE_MEDIAN*2);
                p_score += nrows*2;
                if (hardDropFocusBlock())
                    return;
            }
            else
            if (pekksym == p_keymap[HoldKey]) {
                if (p_hold_on) 
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
            if (dir == Down) {
                p_score += 1;
            }
            adjustShadowBlock();
        }
    }
}

#undef pekksym 
};
