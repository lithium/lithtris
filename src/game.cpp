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
    p_skin_bitmap = 0;
    p_window = 0;
    p_rand_fd = -1;
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

    p_level = 1;
    p_lines = 0;
    p_blockSpeed = INITIAL_SPEED;
    adjustShadowBlock();
}

void Game::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    p_window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);
    p_timer = SDL_GetTicks();

    p_blocks_bitmap = SDL_LoadBMP("data/skins/blocks2.bmp");
    // set black as transparency color
    SDL_SetColorKey(p_blocks_bitmap, SDL_SRCCOLORKEY, SDL_MapRGB(p_blocks_bitmap->format, 0,0,0));

    p_skin_bitmap = SDL_LoadBMP("data/skins/tempest-skin.bmp");


    init_random();

    restart();
  
    pushState(&Game::exit_state);
    pushState(&Game::menu_state);

    TTF_Init();
    p_menu_font = TTF_OpenFont("verdana.ttf",14);
}
void Game::shutdown()
{
    TTF_CloseFont(p_menu_font);

    TTF_Quit();
    SDL_FreeSurface(p_blocks_bitmap);
    SDL_FreeSurface(p_skin_bitmap);
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

void Game::drawBackground()
{
    SDL_Rect src = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_BlitSurface(p_skin_bitmap, &src, p_window, &dest);
}
void Game::drawScore()
{
    static TTF_Font *score_font = TTF_OpenFont("arial.ttf",28);

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
        p_lines += num_lines;
        if (p_lines >= LINES_PER_LEVEL) {
            p_level++;
            checkWin();
            p_blockSpeed -= SPEED_CHANGE;
            p_lines %= LINES_PER_LEVEL;
        }
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


}

