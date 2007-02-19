#include "Game.h"


namespace lithtris 
{

void Game::keys_state()
{
    handleKeysInput();
    clearScreen();
    drawKeysPrompt();
    SDL_UpdateRect(p_window,0,0,0,0);
    SDL_Delay(timeLeftInFrame());
}

void Game::toggle_state()
{
    switch (p_which_menu) {
        case MusicToggle: p_music_on = !p_music_on; break;
        case CascadeToggle: p_cascade_on = !p_cascade_on; break;
        case ScoreToggle: p_showscore_on = !p_showscore_on; break;
        case ShadowToggle: p_shadow_on = !p_shadow_on; break;
        case HoldToggle: p_hold_on = !p_hold_on; break;
        case InfspinToggle: p_infspin_on = !p_infspin_on; break;
    }
    p_which_menu = OptionsMenu;
    p_stateStack.pop();
}

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
    static float line_transition_counter = LINE_TIME;
    static float plain_transition_counter = PLAIN_TIME;
    if (!p_playing_music) playNextMusicTrack();


    if (p_draw_state.top() == LineTransition) {
        if (--line_transition_counter) {
        }
        else {
            p_draw_state.pop();
            line_transition_counter = LINE_TIME;
            nextFocusBlock();
        }
    }
    else
    if (p_draw_state.top() == PlainTransition) {
        if (--plain_transition_counter) {
        }
        else {
            p_draw_state.top() = NoTransition;
            plain_transition_counter = PLAIN_TIME;
            nextFocusBlock();
        }
    }
    else 
    if (p_focusBlock)
    {

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
            if (!p_infspin_on || (SDL_GetTicks() - p_spin_ticks >= INF_SPIN_MS)) {
                if (handleBottomCollision()) 
                    return;
            }
        }

        handlePlayInput();
    }

    clearScreen();
    drawBackground();

    if (p_focusBlock) p_focusBlock->draw(p_window);
    if (p_holdBlock) p_holdBlock->draw(p_window);
    if (p_shadow_on && p_shadowBlock) p_shadowBlock->draw(p_window);
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
    if (p_showscore_on && (p_draw_state.top() == LineTransition))
        drawLineTransition();

    SDL_UpdateRect(p_window,0,0,0,0);
    SDL_Delay(timeLeftInFrame());
}


void Game::quit_state()
{
    clearStates();
}

void Game::restart_state()
{
    restart();
    pushState(&Game::play_state);
}

void Game::stopplaying_state()
{
    p_which_menu = MainMenu;
    restart();
    pushState(&Game::menu_state);
}



};
