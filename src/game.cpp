#include "Game.h"
#include <time.h>

namespace lithtris
{

Game::Game()
{
    memset(p_pile,0, sizeof(p_pile));
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
        const state_t &top = p_stateStack.top();
        if (top.statePointer)
            this.*top.statePointer();
    }
}

Block *Game::getRandomBlock(int x, int y)
{
    BlockType type = rand() % NumBlockTypes;
    return new Block(x, y, p_blocks_bitmap, type);
}


void Game::restart()
{
    reset();
    p_focusBlock = getRandomBlock(BLOCK_START_X, BLOCK_START_Y);

    // WTF: hehe
    {
        nextblock_t tmp = {NEXTAREA1_X, NEXTAREA1_Y, NEXTAREA1_W, NEXTAREA1_H};
        tmp.block = getRandomBLock(tmp.x, tmp.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {NEXTAREA2_X, NEXTAREA2_Y, NEXTAREA2_W, NEXTAREA2_H};
        tmp.block = getRandomBLock(tmp.x, tmp.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {NEXTAREA3_X, NEXTAREA3_Y, NEXTAREA3_W, NEXTAREA3_H};
        tmp.block = getRandomBLock(tmp.x, tmp.y);
        p_nextBlocks.push_back(tmp);
    }
    {
        nextblock_t tmp = {NEXTAREA4_X, NEXTAREA4_Y, NEXTAREA4_W, NEXTAREA4_H};
        tmp.block = getRandomBLock(tmp.x, tmp.y);
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

    for (int r=0; r < MAX_ROWS; r++) {
        for (int c=0; c < SQUARES_PER_ROW; c++) {
            if (p_pile[r][c]) delete(p_pile[r][c]);
        }
    }
    memset(p_pile,0, sizeof(p_pile));

    for (i=0; i < p_nextBlocks.size(); i++) {
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

void Game::displayText(std::string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB, const char *font_name="arial.ttf")
{
    static TTF_Font *font = TTF_OpenFont(fontname,size);
    SDL_Color foreground = {fR,fG,fB};
    SDL_Color background = {bR,bG,bB};
    SDL_Surface *tmp = TTF_RenderText_Shaded(font, text.c_str(), foreground, background);
    SDL_Rect dest = {x,y,0,0}

    SDL_BlitSurface(tmp, NULL, p_window, &dest);
    SDL_FreeSurface(tmp);
}


void Game::handleBottomCollision()
{
    int num_lines = checkCompletedLines();

    if (num_lines > 0) {
        p_lines += num_lines;
        if (p_lines >= LINES_PER_LEVEL) {
            p_level++;
            checkWin();
            p_focusBlockSpeed -= SPEED_CHANGE;
            p_lines %= LINES_PER_LEVEL;
        }
    }

    checkLoss();
    nextFocusBlock();
}

void Game::addBlockToPile(Block *block)
{
    int i;
    Square **squares = block->getSquares();
    for (i=0; i < 4; i++) {
        SDL_Rect r = getRowCol(squares[i]->centerX(), squares[i]->centerY());
        p_pile[r.x][r.y] = squares[i];
    }
}

void Game::nextFocusBlock()
{
    addBlockToPile(p_focusBlock);
    delete p_focusBlock;

    shiftNextBlocks();
}

void Game::shiftNextBlocks()
{
    p_focusBlock = p_nextBlock[0].block;
    p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_blocks_bitmap);
    for (i=0; i<3; i++)
    {
        p_nextBlock[i].block = p_nextBlock[i+1].block;
        p_nextBlock[i].block->setupSquares(p_nextBlock[i].rect.x, p_nextBlock[i].rect.y, p_blocks_bitmap)
    }
    p_nextBlock[3].block = getRandomBlock(p_nextBlock[3].rect.x, p_nextBlock[3].rect.y, p_blocks_bitmap);
}

void Game::holdFocusBlock()
{
    Block *temp = p_holdBlock;

    p_holdBlock = pFocusBlock;
    if (temp) {
        p_focusBlock = temp;
        p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_blocks_bitmap);
    }
    else {
        shiftNextBlocks();
    }
}


}

