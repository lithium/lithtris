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
    static int alpha = 128;
    static SDL_Surface *line_sur = 0;
    static float alpha_inc = (128 / LINE_TIME);


    if (!p_playing_music) playNextMusicTrack();


    if (p_draw_state.top() == LineTransition) {
        if (--line_transition_counter) {
            if (line_sur) {
                alpha += alpha_inc;
                SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
            }
        }
        else {
            p_draw_state.pop();
            line_transition_counter = LINE_TIME;
            alpha = 128;
            SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
            //remove 
            std::map<int,bool>::iterator it;
            for (it = p_completed_rows.begin(); it != p_completed_rows.end(); it++) {
                if (!it->second) continue;
                int row; int c;
                for (c = 0; c < SQUARES_PER_ROW; c++) {
                    delete p_pile[it->first][c];
                    p_pile[it->first][c] = 0;
                }
                p_completed_rows[it->first] = false;
                
                //apply gravity
                if (!p_cascade_on) { // naive gravity
                    for (row=it->first - 1; row >= 0; row--) {
                        for (c=0; c < SQUARES_PER_ROW; c++) {
                            if (p_pile[row][c]) {
                                p_pile[row][c]->move(Down);
                                p_pile[row+1][c] = p_pile[row][c];
                                p_pile[row][c] = 0;
                            }
                        }
                    }
                }
            }
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

    if (p_holdBlock) p_holdBlock->draw(p_window);
    if (p_shadow_on && p_shadowBlock) p_shadowBlock->draw(p_window);
    if (p_focusBlock) p_focusBlock->draw(p_window);
    int i;
    for (i=p_nextBlocks.size()-1; i>=0; i--)
    {
        p_nextBlocks[i].block->draw(p_window);
    }

    if ((p_completed_rows.size() > 0) && !line_sur) {
        line_sur = SDL_CreateRGBSurface(p_window->flags|SDL_SRCALPHA, PLAYAREA_W, SQUARE_MEDIAN*2, p_window->format->BitsPerPixel,
            p_window->format->Rmask, p_window->format->Gmask, p_window->format->Bmask, p_window->format->Amask); 
        SDL_FillRect(line_sur, NULL, SDL_MapRGB(line_sur->format, 255,255,255));
        SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
    }

    for (i=0; i < MAX_ROWS; i++) {
        for (int c=0; c < SQUARES_PER_ROW; c++) {
            if (p_pile[i][c]) {
                p_pile[i][c]->draw(p_window);
            }
        }
        if (line_sur && p_completed_rows[i]) { // draw a flashing completed row
            SDL_Rect dest = {PLAYAREA_X, PLAYAREA_Y+(SQUARE_MEDIAN*2*i), PLAYAREA_W, SQUARE_MEDIAN*2};
            SDL_BlitSurface(line_sur, NULL, p_window, &dest);
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
    write_scores(p_score_filename.c_str(), p_hiscore);
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
    write_scores(p_score_filename.c_str(), p_hiscore);
}



};
