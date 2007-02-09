#include "Game.h"
#include <time.h>

namespace lithtris
{

Game::Game()
{
    p_focusBlock = 0;
    p_holdBlock = 0;
    p_blocks_bitmap = 0;
    p_skin_bitmap = 0;
    p_window = 0;
    memset(p_pile, 0, sizeof(p_pile));
    init();
}
Game::~Game()
{
    shutdown();
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
    BlockType type = (BlockType)(rand() % NumBlockTypes);
    return new Block(x, y, p_blocks_bitmap, type);
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
}

void Game::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    p_window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);
    p_timer = SDL_GetTicks();

    p_blocks_bitmap = SDL_LoadBMP("data/skins/blocks.bmp");
    p_skin_bitmap = SDL_LoadBMP("data/skins/tempest-skin.bmp");

    srand(time(0));

    restart();
  
    pushState(&Game::exit_state);
    pushState(&Game::menu_state);

    TTF_Init();
}
void Game::shutdown()
{
    reset();

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

}

void Game::clearScreen()
{
    SDL_FillRect(p_window, 0,0);
}

void Game::displayText(const char *text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB, const char *font_name)
{
    static TTF_Font *font = TTF_OpenFont(font_name,size);
    SDL_Color foreground = {fR,fG,fB};
    SDL_Color background = {bR,bG,bB};
    SDL_Surface *tmp = TTF_RenderText_Shaded(font, text, foreground, background);
    SDL_Rect dest = {x,y,0,0};

    SDL_BlitSurface(tmp, NULL, p_window, &dest);
    SDL_FreeSurface(tmp);
}


void Game::handleBottomCollision()
{
    addBlockToPile(p_focusBlock);
    delete p_focusBlock;

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

    //checkLoss();
    nextFocusBlock();
}
void Game::addBlockToPile(Block *block)
{
    int i;
    Square **squares = block->squares();
    for (i=0; i < 4; i++) {
        SDL_Rect r = getRowCol(squares[i]->centerX(), squares[i]->centerY());
        p_pile[r.x][r.y] = squares[i];
    }
}

void Game::nextFocusBlock()
{
    shiftNextBlocks();
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
}


}

