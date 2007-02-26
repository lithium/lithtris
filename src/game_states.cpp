#include "Game.h"


namespace lithtris 
{

void Game::start_state()
{
    handleStartInput();
    clearScreen();
    drawMenu(p_which_menu);
    SDL_UpdateRect(p_window,0,0,0,0);
    SDL_Delay(timeLeftInFrame());
}

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
        case ScoreToggle: p_showscore_on = !p_showscore_on; break;
        case ShadowToggle: p_shadow_on = !p_shadow_on; break;
        case HoldToggle: p_hold_on = !p_hold_on; break;
        case InfspinToggle: p_infspin_on = !p_infspin_on; break;
        case CascadeToggle: 
            p_cascade_on = !p_cascade_on; 
            p_which_menu = StartMenu; 
            p_stateStack.pop();
            return;
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


    if (p_transition == LineTransition) {
        if (--line_transition_counter) {
            if (line_sur) {
                alpha += alpha_inc;
                SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
            }
        }
        else {
            p_transition = NoTransition;
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
                if (!p_cascade_on) {
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
            if (p_cascade_on) {
                if (checkCascades(it->first-1)) {
                    pushState(&Game::cascade_state);
                    return;
                }
            }
            nextFocusBlock();
        }
    }
    else
    if (p_transition == PlainTransition) {
        if (--plain_transition_counter) {
        }
        else {
            p_transition = NoTransition;
            plain_transition_counter = PLAIN_TIME;
            nextFocusBlock();
        }
    }
    else 
    if (p_focusBlock)
    {
        force_down_counter++;
        if (force_down_counter >= p_blockSpeed)
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

    if (p_shadow_on && p_shadowBlock) p_shadowBlock->draw(p_window);
    if (p_focusBlock) p_focusBlock->draw(p_window);
    if (p_holdBlock) p_holdBlock->draw(p_window);
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
    if (p_showscore_on && p_transition == LineTransition) 
        drawLineTransition();

    SDL_UpdateRect(p_window,0,0,0,0);

    SDL_Delay(timeLeftInFrame());
}

bool Game::checkCascades(int start_row)
{
    bool ret = false;
    for (int c=0; c < SQUARES_PER_ROW; c++) {
        bool col_falling = false;
        for (int r=start_row; r >= 0; r--) {
            if (p_pile[r][c] == 0)  {
                col_falling = true;
            }
            else if (col_falling) {
                // check to see if its attached to same colors
                if (false) {
                else {
                    ret = true;
                    p_cascading_cols[c] = start_row;
                    goto check_cascades_continue_outer;
                }
            }
        }
        p_cascading_cols[c] = -1;
check_cascades_continue_outer:
        ;
    }
    return ret;
}

void Game::cascade_state()
{
#define FALL_TIME 0.25*FRAMES_PER_SECOND
    static int fall_counter = FALL_TIME;
    static float line_transition_counter = LINE_TIME;
    static int alpha = 128;
    static SDL_Surface *line_sur = 0;
    static float alpha_inc = (128 / LINE_TIME);
    int i;
    int c;


    if (p_transition == LineTransition) {
        if (--line_transition_counter) {
            if (line_sur) {
                alpha += alpha_inc;
                SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
            }
        }
        else {
            p_transition = NoTransition;
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
            }
        }
    }

    if ((p_completed_rows.size() > 0) && !line_sur) {
        line_sur = SDL_CreateRGBSurface(p_window->flags|SDL_SRCALPHA, PLAYAREA_W, SQUARE_MEDIAN*2, p_window->format->BitsPerPixel,
            p_window->format->Rmask, p_window->format->Gmask, p_window->format->Bmask, p_window->format->Amask); 
        SDL_FillRect(line_sur, NULL, SDL_MapRGB(line_sur->format, 255,255,255));
        SDL_SetAlpha(line_sur, SDL_SRCALPHA, (Uint8)(alpha));
    }

    clearScreen();
    drawBackground();

    if (p_holdBlock) p_holdBlock->draw(p_window);
    for (i=p_nextBlocks.size()-1; i>=0; i--)
    {
        p_nextBlocks[i].block->draw(p_window);
    }



    if (--fall_counter < 0) {
        //adjust cascade blocks
        for (c=0; c < SQUARES_PER_ROW; c++) {
            for (i=p_cascading_cols[c]-1; i > 0; i--) {
                if (p_pile[i][c] && !p_pile[i+1][c]) {
                    p_pile[i][c]->move(Down);
                    p_pile[i+1][c] = p_pile[i][c];
                    p_pile[i][c] = 0;
                }
            }
        }
        fall_counter = FALL_TIME;
    }


    for (i=0; i < MAX_ROWS; i++) {
        for (c=0; c < SQUARES_PER_ROW; c++) {

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
    if (p_showscore_on && p_transition == LineTransition)  {
        drawLineTransition();
    }
    else 
    if (!checkCascades(MAX_ROWS-1))  {
        /* WTF: this is a mimic of updateLines() */
        int num_lines = checkCompletedLines();
        if (num_lines > 0) { // cascade cleared more lines
           updateLines(false);
        }
        else { // cascade finished
           updateLines(true);
           p_stateStack.pop();
        }
    }

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
