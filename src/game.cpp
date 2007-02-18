#include "Game.h"

#include <time.h>
#include <unistd.h>
#include <fcntl.h>

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

Block *Game::getRandomBlock(int x, int y)
{
    BlockType type = (BlockType)(get_random(0,NumBlockTypes));
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


void Game::restart()
{
    reset();
    p_focusBlock = getRandomBlock(BLOCK_START_X, BLOCK_START_Y);

    // WTF: hehe
    {
        nextblock_t tmp = {0,{NEXTAREA1_X + NEXTAREA1_W/2 - SQUARE_MEDIAN, NEXTAREA1_Y + NEXTAREA1_H/2, NEXTAREA1_W, NEXTAREA1_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA2_X + NEXTAREA2_W/2 - SQUARE_MEDIAN, NEXTAREA2_Y + NEXTAREA2_H/2, NEXTAREA2_W, NEXTAREA2_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA3_X + NEXTAREA3_W/2 - SQUARE_MEDIAN, NEXTAREA3_Y + NEXTAREA3_H/2, NEXTAREA3_W, NEXTAREA3_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {0,{NEXTAREA4_X + NEXTAREA4_W/2 - SQUARE_MEDIAN, NEXTAREA4_Y + NEXTAREA4_H/2, NEXTAREA4_W, NEXTAREA4_H}};
        tmp.block = getRandomBlock(tmp.rect.x, tmp.rect.y);
        p_nextBlocks.push_back(tmp);
    }
    p_holdBlock = 0;

    p_level = 8;
    p_lines = 78;
    p_blockSpeed = INITIAL_SPEED;
    adjustShadowBlock();
}

void Game::haltMusic()
{
    Mix_HaltMusic();
    p_playing_music = false;
}
void Game::playNextMusicTrack() 
{
    int n = ((p_level+1) / 2)-1;
    Mix_PlayMusic(p_music[n], -1);
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
    p_playing_music = false;

    p_blocks_bitmap = open_with_transp("data/blocks.bmp");

    p_levels_bitmap = SDL_LoadBMP("data/levels.bmp");

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

    p_keymap[MoveLeftKey] = SDLK_LEFT;
    p_keymap[MoveRightKey] = SDLK_RIGHT;
    p_keymap[SoftDropKey] = SDLK_DOWN;
    p_keymap[HardDropKey] = SDLK_UP;
    p_keymap[RotLeftKey] = SDLK_z;
    p_keymap[RotRightKey] = SDLK_x;
    p_keymap[HoldKey] = SDLK_SPACE;

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
        menuitem_t tmp = {"Quit", &Game::menu_state, ExitMenu};
        p_menu[MainMenu].push_back(tmp);
    }

    p_menu_titles[OptionsMenu] = open_with_transp("data/menu/options.bmp");
    {
        menuitem_t tmp = {"Keys", &Game::keys_state, KeysMenu};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Shadow: ", &Game::menu_state, KeysMenu};
        p_menu[OptionsMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"Hold: ", &Game::menu_state, KeysMenu};
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

//    p_menu_titles[KeysMenu] = open_with_transp("data/menu/keys.bmp");
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
        menuitem_t tmp = {"Quit", &Game::menu_state, ExitMenu};
        p_menu[WinnerMenu].push_back(tmp);
        p_menu[LoserMenu].push_back(tmp);
    }

    {
        menuitem_t tmp = {"Yes", &Game::quit_state, InvalidMenu};
        p_menu[ExitMenu].push_back(tmp);
    }
    {
        menuitem_t tmp = {"No", &Game::menu_state, MainMenu};
        p_menu[ExitMenu].push_back(tmp);
    }






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

    clearStates();

    if (statePointer) 
    {
        pushState(statePointer);
    }
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
        displayText(p_menu_font, it->label, x, y, r,g,b, 0,0,0);
        y += 20;
    }
}

void Game::drawBackground()
{
    int x = p_level > 10 ? WINDOW_WIDTH : 0;
    int y = ((p_level-1) % 10) * WINDOW_HEIGHT;
    SDL_Rect src = {x,y,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_BlitSurface(p_levels_bitmap, &src, p_window, &dest);
    printf("level %d @ (%d,%d)\n",p_level,x,y);
}
void Game::drawScore()
{
    static TTF_Font *score_font = TTF_OpenFont("data/arial.ttf",28);

    char lvl[3];
    char lns[3];
    sprintf(lvl, "%u", p_level);
    sprintf(lns, "%u", p_lines);
    displayText(score_font, lvl, LEVELAREA_X + LEVELAREA_W/4, LEVELAREA_Y + LEVELAREA_H/4, 255,255,255, 0,0,0);
    displayText(score_font, lns, LINESAREA_X + LEVELAREA_W/4, LINESAREA_Y + LINESAREA_H/4, 255,255,255, 0,0,0);
}

void Game::clearScreen()
{
    SDL_FillRect(p_window, 0,0);
}

void Game::displayText(TTF_Font *font, const char *text, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB)
{
    if (!font) return;
    SDL_Color foreground = {fR,fG,fB};
    SDL_Color background = {bR,bG,bB};
    SDL_Surface *tmp = TTF_RenderText_Shaded(font, text, foreground, background);
    SDL_Rect dest = {x,y,0,0};

    SDL_BlitSurface(tmp, NULL, p_window, &dest);
    SDL_FreeSurface(tmp);
}

void Game::updateLines()
{
    int num_lines = checkCompletedLines();

    if (num_lines > 0) {
        if (((p_lines % LINES_PER_LEVEL) + num_lines) >= LINES_PER_LEVEL) {
            p_level++;
            if (checkWin()) return;
            if (p_level % 2) {
                playNextMusicTrack();
            }
            p_blockSpeed -= SPEED_CHANGE;
        }
        p_lines += num_lines;
    }

}


bool Game::handleBottomCollision()
{
    if (addBlockToPile(p_focusBlock))
        return true; // loss condition
    delete p_focusBlock;

    updateLines();
    nextFocusBlock();
    return false;
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

    p_holdBlock = p_focusBlock;
    p_holdBlock->setupSquares(HOLDAREA_X+HOLDAREA_W/2-SQUARE_MEDIAN, HOLDAREA_Y+HOLDAREA_H/2, p_blocks_bitmap);
    if (temp) {
        p_focusBlock = temp;
        p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_blocks_bitmap);
    }
    else {
        shiftNextBlocks();
    }
    adjustShadowBlock();
}

bool Game::hardDropFocusBlock()
{
    p_shadowBlock->setIsShadow(false);
    if (addBlockToPile(p_shadowBlock))
        return true;;
    updateLines();
    delete p_focusBlock;
    nextFocusBlock();
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

}

