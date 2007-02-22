#include "Game.h"

#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <iostream>

namespace lithtris
{

Game::Game()
{
    p_focusBlock = 0;
    p_holdBlock = 0;
    p_shadowBlock = 0;
    p_blocks_bitmap = 0;
    p_levels_bitmap = 0;
    p_window = 0;
    p_rand_fd = -1;
    p_has_audio = false;
    p_music_paused = false;
    p_hold_locked = false;
    p_line_message = 0;
    p_last_tetris = false;
    p_menu_item = 0;
    p_b2b = false;
    p_combo = 0;
    p_cascade = 1;

    p_music_on = true;
    p_hold_on = true;
    p_shadow_on = true;
    p_cascade_on = false;
    p_infspin_on = true;
    p_showscore_on = true;

    memset(p_pile, 0, sizeof(p_pile));
    init();
}
Game::~Game()
{
    shutdown();
}


void Game::init_random()
{
    if (p_rand_fd != -1) return;

//    p_rand_fd = open("/dev/urandom", O_RDONLY);

    if (p_rand_fd == -1) { // fallback to rand()
        srand(time(0));
    }
}

int Game::get_random(int min, int max)
{
    int ret;
    int byte;
    if (p_rand_fd != -1) {
        read(p_rand_fd, &byte, sizeof(int));
    }
    else { // fallback to rand()
        byte = rand();
    }

    ret = (byte % (max - min)) + min;
    return ret;
}

void Game::run()
{
    while (!p_stateStack.empty())
    {
        StateFunction func = p_stateStack.top();
        if (func)
            (*this.*func)();
    }
}

void Game::shuffle_bag(BlockType *bag,int bag_size)
{
    for (int i=0; i < bag_size; i++) {
        int j = get_random(0,bag_size - i);
        BlockType t = bag[j];
        bag[j] = bag[i];
        bag[i] = t;
    }
}

Block *Game::getRandomBlock(int x, int y)
{
    static BlockType bag[7];
    static int bag_len = -1;

    if (bag_len <= 0) {
/*
        static BlockType tmp_bag[7] = {TBlock, SquareBlock, ZBlock, SBlock, LineBlock, JBlock};
        for (int i=0; i < 7; i++) { bag[i] = tmp_bag[i]; } // fill bag
        bag_len = 7;
        */
        for (int i=0; i < 7; i++) { bag[i] = (BlockType)i; } // fill bag
        bag_len = 7;
        shuffle_bag(bag,7);
    }
    BlockType type = bag[--bag_len];

    return new Block(x, y, p_blocks_bitmap, type);
}

void Game::adjustShadowBlock()
{
    if (p_shadowBlock) delete p_shadowBlock;
    p_shadowBlock = p_focusBlock->getRotatedCopy(InvalidDirection);
    p_shadowBlock->setIsShadow(true);
    
    while (! checkCollisions(p_shadowBlock, Down)) {
        p_shadowBlock->move(Down);
    }
}


void Game::restart(int level)
{
    reset();
    p_match_start = time(0);
    p_focusBlock = getRandomBlock(BLOCK_START_X, BLOCK_START_Y);

    // WTF: hehe
    {
        nextblock_t tmp = {0,{NEXTAREA1_X + NEXTAREA1_W/2, NEXTAREA1_Y + NEXTAREA1_H/2, NEXTAREA1_W, NEXTAREA1_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA2_X + NEXTAREA2_W/2, NEXTAREA2_Y + NEXTAREA2_H/2, NEXTAREA2_W, NEXTAREA2_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA3_X + NEXTAREA3_W/2, NEXTAREA3_Y + NEXTAREA3_H/2, NEXTAREA3_W, NEXTAREA3_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA4_X + NEXTAREA4_W/2, NEXTAREA4_Y + NEXTAREA4_H/2, NEXTAREA4_W, NEXTAREA4_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    p_holdBlock = 0;

    p_level = 20;
    p_lines = 0;
    p_score = 0;
    p_music_paused = false;
    p_playing_music = false; 
    p_draw_state.empty();
    p_draw_state.push( NoTransition);
    p_blockSpeed = INITIAL_SPEED-(p_level*SPEED_CHANGE);
    p_hold_locked = false;
    p_last_tetris = false;
    p_line_message = 0;

    adjustShadowBlock();
}

void Game::pauseMusic()
{
    p_music_paused = true;
    p_playing_music = false;
    Mix_PauseMusic();
}

void Game::haltMusic()
{
    Mix_HaltMusic();
    p_playing_music = false;
}
void Game::playNextMusicTrack() 
{
    if (!p_music_on) return;

    if (p_music_paused) {
        p_music_paused = false;
        Mix_ResumeMusic();
    }
    else {
        int n = ((p_level+1) / 2)-1;
        if (n < 0) n = 0;
        Mix_FadeInMusic(p_music[n], -1, 1500);
    }
    p_playing_music = true;
}

static SDL_Surface *open_with_transp(const char *filename, int r=0,int g=0,int b=0)
{
    SDL_Surface *s = SDL_LoadBMP(filename);
    SDL_SetColorKey(s, SDL_SRCCOLORKEY, SDL_MapRGB(s->format, r,g,b));
    return s;
}


void Game::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);
    p_window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);

    if (Mix_OpenAudio(48000,AUDIO_S16, 2, 4096) != -1) {
        p_has_audio = true;
        Mix_SetMusicCMD(getenv("MUSIC_CMD"));
    }

    p_keymap[MoveLeftKey] = SDLK_LEFT;
    p_keymap[MoveRightKey] = SDLK_RIGHT;
    p_keymap[SoftDropKey] = SDLK_DOWN;
    p_keymap[HardDropKey] = SDLK_UP;
    p_keymap[RotLeftKey] = SDLK_z;
    p_keymap[RotRightKey] = SDLK_x;
    p_keymap[HoldKey] = SDLK_SPACE;

    p_blocks_bitmap = open_with_transp("data/blocks.bmp");

    p_levels_bitmap = SDL_LoadBMP("data/levels.bmp");

    std::string fn(getenv("HOME"));
    p_score_filename.assign(getenv("HOME"));
    if (!fn.empty()) {
        p_score_filename.append("/.lithtris");
        mkdir(p_score_filename.c_str(), 0755);
        p_score_filename.append("/hiscore.dat");
    }
    else {
        p_score_filename.assign("/usr/local/share/lithtris/hiscore.dat");
    }
    read_scores(p_score_filename.c_str(), p_hiscore);

    p_music[0] = Mix_LoadMUS("data/music/track01.mp3");
    p_music[1] = Mix_LoadMUS("data/music/track02.mp3");
    p_music[2] = Mix_LoadMUS("data/music/track03.mp3");
    p_music[3] = Mix_LoadMUS("data/music/track04.mp3");
    p_music[4] = Mix_LoadMUS("data/music/track05.mp3");
    p_music[5] = Mix_LoadMUS("data/music/track06.mp3");
    p_music[6] = Mix_LoadMUS("data/music/track07.mp3");
    p_music[7] = Mix_LoadMUS("data/music/track08.mp3");
    p_music[8] = Mix_LoadMUS("data/music/track09.mp3");
    p_music[9] = Mix_LoadMUS("data/music/track10.mp3");


    p_menu_titles[MainMenu] = open_with_transp("data/menu/lithtris.bmp");
    {
        menuitem_t tmp = {"Play", &Game::play_state, InvalidMenu};
        p_menu[MainMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Options", &Game::menu_state, OptionsMenu};
        p_menu[MainMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Scores", &Game::menu_state, ScoresMenu};
        p_menu[MainMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Exit", &Game::menu_state, ExitMenu};
        p_menu[MainMenu].push_back(tmp);
    }

    p_menu_titles[PauseMenu] = open_with_transp("data/menu/lithtris.bmp");
    {
        menuitem_t tmp = {"Resume", &Game::play_state, InvalidMenu};
        p_menu[PauseMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Restart", &Game::restart_state, InvalidMenu};
        p_menu[PauseMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Quit", &Game::stopplaying_state, InvalidMenu};
        p_menu[PauseMenu].push_back(tmp);
    }

    p_menu_titles[OptionsMenu] = open_with_transp("data/menu/options.bmp");
    {
        menuitem_t tmp = {"Keys", &Game::menu_state, KeysMenu};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Music: ", &Game::toggle_state, MusicToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Cascade: ", &Game::toggle_state, CascadeToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Show Score: ", &Game::toggle_state, ScoreToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Shadow: ", &Game::toggle_state, ShadowToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Hold: ", &Game::toggle_state, HoldToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Inf. Spin: ", &Game::toggle_state, InfspinToggle};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Back", &Game::menu_state, MainMenu};
        p_menu[OptionsMenu].push_back(tmp);
    }


    p_menu_titles[ScoresMenu] = open_with_transp("data/menu/scores.bmp");
    {
        menuitem_t tmp = {"Back", &Game::menu_state, MainMenu};
        p_menu[ScoresMenu].push_back(tmp);
    }

    p_menu_titles[KeysMenu] = open_with_transp("data/menu/options.bmp");
    {
        menuitem_t tmp = {"Move Left: ", &Game::keys_state, MoveLeftToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Move Right: ", &Game::keys_state, MoveRightToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Move Down: ", &Game::keys_state, SoftDropToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Hard Drop: ", &Game::keys_state, HardDropToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Spin Left: ", &Game::keys_state, SpinLeftToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Spin Right: ", &Game::keys_state, SpinRightToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Hold: ", &Game::keys_state, HoldKeyToggle};
        p_menu[KeysMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Back", &Game::menu_state, MainMenu};
        p_menu[KeysMenu].push_back(tmp);
    }

    p_menu_titles[WinnerMenu] = open_with_transp("data/menu/winner.bmp");
    p_menu_titles[LoserMenu] = open_with_transp("data/menu/loser.bmp");
    {
        menuitem_t tmp = {"Restart", &Game::restart_state, InvalidMenu};
        p_menu[WinnerMenu].push_back(tmp);
        p_menu[LoserMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Quit", &Game::menu_state, MainMenu};
        p_menu[WinnerMenu].push_back(tmp);
        p_menu[LoserMenu].push_back(tmp);
    }


    {
        menuitem_t tmp = {"Yes, Exit", &Game::quit_state, InvalidMenu};
        p_menu[ExitMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Back", &Game::menu_state, MainMenu};
        p_menu[ExitMenu].push_back(tmp);
    }

    p_draw_state.push(NoTransition);



    p_spin_ticks = SDL_GetTicks();
    init_random();

    restart();
  
    p_which_menu = MainMenu;
    pushState(&Game::menu_state);

    TTF_Init();
    p_menu_font = TTF_OpenFont("data/arial.ttf",14);
}
void Game::shutdown()
{
    Mix_HaltMusic();
    TTF_CloseFont(p_menu_font);

    for (int i=0; i < 10; i++) {
        Mix_FreeMusic(p_music[i]);
    }

    TTF_Quit();
    SDL_FreeSurface(p_blocks_bitmap);
    SDL_FreeSurface(p_levels_bitmap);
    SDL_FreeSurface(p_window);
    SDL_Quit();
}

void Game::reset(StateFunction statePointer)
{
    int i;
    if (p_focusBlock) delete(p_focusBlock);
    if (p_holdBlock) delete(p_holdBlock);

    for (i=0; i < MAX_ROWS; i++) {
        for (int c = 0; c < SQUARES_PER_ROW; c++) {
            delete p_pile[i][c];
        }
    }
    memset(p_pile,0, sizeof(p_pile));

    for (i=p_nextBlocks.size()-1; i >= 0; i--) {
        delete(p_nextBlocks[i].block);
    }
    p_nextBlocks.clear();

    p_lines = 0;
    p_level = 1;
    p_score = 0;

    clearStates();

    if (statePointer) 
    {
        pushState(statePointer);
    }
}


void Game::drawKeysPrompt()
{
    displayText(p_menu_font, "Press key", WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 255,255,255,0,0,0);
}


void Game::drawMenu(MenuId which)
{
    SDL_Surface *surface = p_menu_titles[which];

    if (surface) {
        SDL_Rect src = {0,0,MENU_TITLE_W,MENU_TITLE_H};
        SDL_Rect dest = {MENU_TITLE_X, MENU_TITLE_Y, MENU_TITLE_W, MENU_TITLE_H};
        SDL_BlitSurface(surface, &src, p_window, &dest);
    }
    
    if (p_menu[which].size() < 1) return;

    std::vector<menuitem_t>::iterator it;
    int x = MENU_TITLE_X;
    int y = MENU_TITLE_Y + MENU_TITLE_H + MENU_TITLE_Y;
    int i = 0;
    for (it=p_menu[which].begin(); it != p_menu[which].end(); it++, i++) {
        int r = 255; int g = 255; int b = 255;
        if (i == p_menu_item) {
            r = 0; g = 0; b = 255;
        }
        SDL_Rect rect = displayText(p_menu_font, it->label, x, y, r,g,b, 0,0,0, true, 255, Left);
        switch (it->menu) {
        case MusicToggle:
            displayText(p_menu_font, p_music_on ? "On" : "Off", rect.x+rect.w, y, p_music_on?128:r,g,b, 0,0,0,true,255,Left);
            break;
        case CascadeToggle:
            displayText(p_menu_font, p_cascade_on ? "On" : "Off", rect.x+rect.w, y, p_cascade_on?128:r,g,b, 0,0,0,true,255,Left);
            break;
        case ScoreToggle:
            displayText(p_menu_font, p_showscore_on ? "On" : "Off", rect.x+rect.w, y, p_showscore_on?128:r,g,b, 0,0,0,true,255,Left);
            break;
        case ShadowToggle:
            displayText(p_menu_font, p_shadow_on ? "On" : "Off", rect.x+rect.w, y, p_shadow_on?128:r,g,b, 0,0,0,true,255,Left);
            break;
        case HoldToggle:
            displayText(p_menu_font, p_hold_on ? "On" : "Off", rect.x+rect.w, y, p_hold_on?128:r,g,b, 0,0,0,true,255,Left);
            break;
        case InfspinToggle:
            displayText(p_menu_font, p_infspin_on ? "On" : "Off", rect.x+rect.w, y, p_infspin_on?128:r,g,b, 0,0,0,true,255,Left);
            break;

        case MoveLeftToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[MoveLeftKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case MoveRightToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[MoveRightKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case SoftDropToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[SoftDropKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case HardDropToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[HardDropKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case SpinLeftToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[RotLeftKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case SpinRightToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[RotRightKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        case HoldKeyToggle:
            displayText(p_menu_font, SDL_GetKeyName(p_keymap[HoldKey]), rect.x+rect.w, y, 128,g,b, 0,0,0,true,255,Left);
            break;
        }
        y += 20;
    }

    if (which == ScoresMenu) {
        std::stringstream ss;
        ss << "Highest Score: " << p_hiscore.highest_score;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
        ss.str("");
        ss << "Highest Level: " << p_hiscore.highest_level;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
        ss.str("");
        ss << "Longest Combo: " << p_hiscore.highest_combo;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
        ss.str("");
        ss << "Highest Line Score: " << p_hiscore.highest_line_score;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
        ss.str("");
        ss << "Longest Game: " << (p_hiscore.longest_time / 60) << ":" << p_hiscore.longest_time % 60;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
        ss.str("");
        ss << "Most Lines: " << p_hiscore.most_lines;
        displayText(p_menu_font, ss.str().c_str(), x, y += 20, 255,255,255, 0,0,0,true,255,Left);
    }
}



SDL_Rect Game::backgroundRectFromLevel(int level)
{
    int x = level > 10 ? WINDOW_WIDTH : 0;
    int y = ((level-1) % 10) * WINDOW_HEIGHT;
    SDL_Rect ret = {x,y,WINDOW_WIDTH,WINDOW_HEIGHT};
    return ret;
}

void Game::drawPlayScreen()
{
}

void Game::drawBackground()
{
    if (p_draw_state.top() == LevelTransition) {
        drawBlendedBackground(1.5);
        return;
    }
    SDL_Rect src = backgroundRectFromLevel(p_level);
    SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_BlitSurface(p_levels_bitmap, &src, p_window, &dest);
}

void Game::drawBlendedBackground(float fade_secs)
{
    SDL_Rect old_src = backgroundRectFromLevel(p_level);
    SDL_Rect new_src = backgroundRectFromLevel(p_level+1);
    SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};

    static SDL_Surface *old_sur = 0;
    static SDL_Surface *new_sur = 0;
    static float alpha=255;
    float alpha_inc = 256 / (fade_secs * FRAMES_PER_SECOND); 
   
    if (old_sur == 0) {
        old_sur = SDL_CreateRGBSurface(p_levels_bitmap->flags|SDL_SRCALPHA, WINDOW_WIDTH, WINDOW_HEIGHT, p_levels_bitmap->format->BitsPerPixel,
            p_levels_bitmap->format->Rmask, p_levels_bitmap->format->Gmask, p_levels_bitmap->format->Bmask, p_levels_bitmap->format->Amask); 
        SDL_BlitSurface(p_levels_bitmap, &old_src, old_sur, &dest);

        new_sur = SDL_CreateRGBSurface(p_levels_bitmap->flags|SDL_SRCALPHA, WINDOW_WIDTH, WINDOW_HEIGHT, p_levels_bitmap->format->BitsPerPixel,
            p_levels_bitmap->format->Rmask, p_levels_bitmap->format->Gmask, p_levels_bitmap->format->Bmask, p_levels_bitmap->format->Amask); 
        SDL_BlitSurface(p_levels_bitmap, &new_src, new_sur, &dest);
        alpha = 255;
    }

    SDL_SetAlpha(old_sur, SDL_SRCALPHA, (Uint8)(alpha));
    SDL_SetAlpha(new_sur, SDL_SRCALPHA, (Uint8)(255-alpha));
    SDL_BlitSurface(new_sur, &dest, p_window, &dest);
    SDL_BlitSurface(old_sur, &dest, p_window, &dest);

    alpha -= alpha_inc;
    if (alpha < 0) { /* blending is overwith */
        SDL_FreeSurface(old_sur);
        SDL_FreeSurface(new_sur);
        old_sur = new_sur = 0;
        p_draw_state.pop();
        if (++p_level > p_hiscore.highest_level) 
            p_hiscore.highest_level = p_level;
        if (p_level % 2) {
            playNextMusicTrack();
        }
    }

}


void Game::drawLineTransition()
{
    static TTF_Font *transition_font = TTF_OpenFont("data/arial.ttf",64);
    if (p_line_message) {
        std::stringstream ss;
        if (p_b2b) 
            ss << "B2B ";
        ss << p_line_message;
        if (p_combo > 1) 
             ss << " +" << p_combo << " ";
        displayText(transition_font, ss.str().c_str(), WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 255,255,255, 0,0,0, true, p_line_alpha -= 256/LINE_TIME);

    }
}

void Game::drawScore()
{
    static TTF_Font *score_font = TTF_OpenFont("data/arial.ttf",14);
    static TTF_Font *level_font = TTF_OpenFont("data/arial.ttf",28);

    char lvl[5];
    char lns[5];
    char sco1[25],sco2[25],sco3[25];
    sprintf(lvl, "%u", p_level);
    sprintf(lns, "%u", p_lines);

    sprintf(sco1,  "  Score: %u", p_score);
    sprintf(sco2, "  Combo: +%u", p_combo);
    sprintf(sco3, "Cascade: x%u", p_cascade);
    displayText(level_font, lvl, LEVELAREA_X + LEVELAREA_W/2, LEVELAREA_Y + LEVELAREA_H/2, 255,255,255, 0,0,0);
    displayText(level_font, lns, LINESAREA_X + LEVELAREA_W/2, LINESAREA_Y + LINESAREA_H/2, 255,255,255, 0,0,0);
    if (p_showscore_on) {
        displayText(score_font, sco1, PLAYAREA_X+PLAYAREA_W + 14, WINDOW_HEIGHT-42, 255,255,255,0,0,0, false, 255, Left);
        displayText(score_font, sco2, PLAYAREA_X+PLAYAREA_W + 14, WINDOW_HEIGHT-28, 255,255,255,0,0,0, false, 255, Left);
        if (p_cascade_on)
            displayText(score_font, sco3, PLAYAREA_X+PLAYAREA_W + 14, WINDOW_HEIGHT-14, 255,255,255,0,0,0, false, 255, Left);
    }
}

void Game::clearScreen()
{
    SDL_FillRect(p_window, 0,0);
}

SDL_Rect Game::displayText(TTF_Font *font, const char *text, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB, bool transparent, int alpha, Direction align)
{
    SDL_Rect r = {0,0,0,0};
    if (!font) return r;
    SDL_Color foreground = {fR,fG,fB};
    SDL_Color background = {bR,bG,bB};
    SDL_Surface *tmp = TTF_RenderText_Shaded(font, text, foreground, background);

    if (align == Down) { // center align
        x -= tmp->w/2;
        y -= tmp->h/2;
    }
    else { // left align
    }
    SDL_Rect dest = {x,y,0,0};
    r.x = x;
    r.y = y;
    r.w = tmp->w;
    r.h = tmp->h;

    if (transparent)
        SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, bR,bG,bB));

    SDL_SetAlpha(tmp, SDL_SRCALPHA, (Uint8)(alpha));
    SDL_BlitSurface(tmp, NULL, p_window, &dest);
    SDL_FreeSurface(tmp);
    return r;
}

int Game::calcScore(int nlines, int level, int combo, bool b2b, int cascade)
{
    return ((nlines-1)*2 + combo + (b2b?5:0) ) * 100 * cascade;
}

void Game::updateLines()
{
    int num_lines = checkCompletedLines();

    if (num_lines > 0) {
        if (((p_lines % LINES_PER_LEVEL) + num_lines) >= LINES_PER_LEVEL) {
            p_blockSpeed -= SPEED_CHANGE;
            p_draw_state.push(LevelTransition);
            if (!(p_level % 2)) {
                Mix_FadeOutMusic(1500);
            }
        }
        p_draw_state.push(LineTransition);
        p_focusBlock = 0;
        p_shadowBlock = 0;
        p_lines += num_lines;


        p_line_alpha = 255;
        switch (num_lines) {
            case 1: p_line_message = "Single"; break;
            case 2: p_line_message = "Double"; break;
            case 3: p_line_message = "Triple"; break;
            case 4: p_line_message = "Tetris"; break;
        }
        p_b2b = p_last_tetris && (num_lines == 4);
        p_last_tetris = (num_lines == 4);
        int line_score = calcScore(num_lines, p_level, ++p_combo, p_b2b);
        p_score += line_score;
        if (line_score > p_hiscore.highest_line_score) 
            p_hiscore.highest_line_score = line_score;
        if (p_combo > p_hiscore.highest_combo)
            p_hiscore.highest_combo = p_combo;
        if (num_lines > p_hiscore.most_lines)
            p_hiscore.most_lines = num_lines;
    }
    else {
        p_draw_state.push(PlainTransition);
        p_line_message = 0;
        p_combo = 0;
    }
    checkWin();
}


bool Game::handleBottomCollision()
{
    if (addBlockToPile(p_focusBlock))
        return true; // loss condition
    delete p_focusBlock;
    delete p_shadowBlock;
    p_focusBlock = 0;
    p_shadowBlock = 0;

    updateLines();
    return true;
}
bool Game::addBlockToPile(Block *block)
{
    int i;
    Square **squares = block->squares();
    for (i=0; i < 4; i++) {
        SDL_Rect r = getRowCol(squares[i]->centerX(), squares[i]->centerY());
        p_pile[r.x][r.y] = squares[i];
    }
    return checkLoss(block);
}

void Game::nextFocusBlock()
{
    shiftNextBlocks();
    adjustShadowBlock();
    p_hold_locked = false;
}

void Game::shiftNextBlocks()
{
    p_focusBlock = p_nextBlocks[0].block;
    p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_blocks_bitmap);
    for (int i=0; i<3; i++)
    {
        p_nextBlocks[i].block = p_nextBlocks[i+1].block;
        p_nextBlocks[i].block->setupSquares(p_nextBlocks[i].rect.x, p_nextBlocks[i].rect.y, p_blocks_bitmap);
    }
    p_nextBlocks[3].block = getRandomBlock(p_nextBlocks[3].rect.x, p_nextBlocks[3].rect.y);
}

void Game::holdFocusBlock()
{
    Block *temp = p_holdBlock;

    if (p_hold_locked) return;

    p_holdBlock = p_focusBlock;
    p_holdBlock->setupSquares(HOLDAREA_X+HOLDAREA_W/2, HOLDAREA_Y+HOLDAREA_H/2, p_blocks_bitmap);
    if (temp) {
        p_focusBlock = temp;
        p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_blocks_bitmap);
    }
    else {
        shiftNextBlocks();
    }
    adjustShadowBlock();
    p_hold_locked = true;
}

bool Game::hardDropFocusBlock()
{

    p_shadowBlock->setIsShadow(false);
    if (addBlockToPile(p_shadowBlock))
        return true;
    delete p_focusBlock;
    delete p_shadowBlock;
    p_focusBlock = 0;
    p_shadowBlock = 0;
    updateLines();
    return false;
}

unsigned int Game::timeLeftInFrame()
{
    static unsigned int next_time = 0;
    unsigned int now = SDL_GetTicks();

    if (next_time <= now) {
        next_time = now + FRAME_RATE;
        return(0);
    }
    return (next_time-now);
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


bool Game::write_scores(const char *filename, hiscores_t &scores)
{
    if (!filename) return false;
    std::ofstream ofs(filename, std::ios::trunc);
    if (ofs.bad() || ofs.fail()) { return false; }

    ofs << p_music_on << ":"
        << p_hold_on << ":"
        << p_shadow_on << ":"
        << p_cascade_on << ":"
        << p_infspin_on  << ":"
        << p_showscore_on << ":"
        << scores.highest_level << ":"
        << scores.highest_score << ":"
        << scores.highest_combo << ":"
        << scores.highest_line_score << ":"
        << scores.longest_time << ":"
        << scores.most_lines << ":"
        << scores.most_cascade << ":"
        << (int)p_keymap[MoveLeftKey] << ":"
        << (int)p_keymap[MoveRightKey] << ":"
        << (int)p_keymap[SoftDropKey] << ":"
        << (int)p_keymap[HardDropKey] << ":"
        << (int)p_keymap[RotLeftKey] << ":"
        << (int)p_keymap[RotRightKey] << ":"
        << (int)p_keymap[HoldKey] << ":"
        ;
    ofs.close();
    return true;
}

bool Game::read_scores(const char *filename, hiscores_t &scores)
{
    memset(&scores, 0, sizeof(hiscores_t));
    if (!filename) return false;
    std::ifstream ifs(filename, std::ios::in);
    if (ifs.bad() || ifs.fail()) { return false; }

    char c;

    ifs >> p_music_on >> c
        >> p_hold_on >> c
        >> p_shadow_on >> c
        >> p_cascade_on >> c
        >> p_infspin_on  >> c
        >> p_showscore_on >> c
        >> scores.highest_level >> c
        >> scores.highest_score >> c
        >> scores.highest_combo >> c
        >> scores.highest_line_score >> c
        >> scores.longest_time >> c
        >> scores.most_lines >> c
        >> scores.most_cascade >> c
        ;
    int t;
    ifs >> t >> c; p_keymap[MoveLeftKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[MoveRightKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[SoftDropKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[HardDropKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[RotLeftKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[RotRightKey] = (SDLKey)t;
    ifs >> t >> c; p_keymap[HoldKey] = (SDLKey)t;
    ifs.close();
    return true;
}


}

