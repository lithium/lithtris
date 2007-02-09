#include "Game.h"
#include <time.h>

namespace lithtris
{

Game::Game()
{
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
            top.statePointer();
    }
}



void Game::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    p_window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);
    p_timer = SDL_GetTicks();
    p_bitmap = SDL_LoadBMP("data/lithtris.bmp");

    
    srand(time(0));
    BlockType type = rand() % NumBlockTypes;
    p_focusBlock = new Block(BLOCK_START_X, BLOCK_START_Y, p_bitmap, type);
    type = rand() % NumBlockTypes;
    p_nextBlock = new Block(NEXT_BLOCK_X, NEXT_BLOCK_Y, p_bitmap, type);
    p_holdBlock = 0;

    p_level = 1;
    p_score = 0;
    p_blockSpeed = INITIAL_SPEED;
    
    state_t state;
    state.statePointer = &Game::exit_state;
    p_stateStack.push(state);
    state.statePointer = &Game::menu_state;
    p_stateStack.push(state);
    TTF_Init();
}
void Game::shutdown()
{
    if (p_focusBlock) delete(p_focusBlock);
    if (p_nextBlock) delete(p_nextBlock);
    if (p_holdBlock) delete(p_holdBlock);

    for (int i=0; i < p_oldSquares.size(); i++) {
        delete(p_oldSquares[i]);
    }
    p_oldSquares.erase(p_oldSquares.begin(),p_oldSquares.end());

    TTF_Quit();
    SDL_FreeSurface(p_bitmap);
    SDL_FreeSurface(p_window);
    SDL_Quit();
}

void Game::reset(statePointer)
{
    for (int i=0; i < p_oldSquares.size(); i++)
    {
        delete p_oldSquares[i];
    }
    p_oldSquares.clear();
    p_score = 0;
    p_level = 1;
    while (!p_stateStack.empty()) p_stateStack.pop();

    state_t tmp;
    tmp.statePointer = statePointer;
    p_stateStack.push(tmp);
}

void Game::drawBackground()
{
    SDL_Rect src = {LEVEL_BACK_X,LEVEL_BACK_Y,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_BlitSurface(p_bitmap, &src, p_window, &dest);
}

void Game::clearScreen()
{
    SDL_FillRect(p_window, 0,0);
}

void Game::displayText(std::string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB)
{
    static TTF_Font *font = TTF_OpenFont("arial.ttf",size);
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
        int to_next_level = p_score % LINES_PER_LEVEL;
        if (num_lines >= p_score % LINES_PER_LEVEL) { // increase level
            p_level++;
            checkWin();
            p_focusBlockSpeed -= SPEED_CHANGE;
        }
        p_score += num_lines;
    }

    checkLoss();
    changeFocusBlock();
}

void Game::changeFocusBlock()
{
    Square **squares = p_focusBlock->getSquares();

    for (int i=0; i <4; i++)
        p_oldSquares.push_back(squares[i]);

    delete p_focusBlock;
    p_focusBlock = p_nextBlock;
    p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_bitmap);
    p_nextBlock = new Block(NEXT_BLOCK_X, NEXT_BLOCK_Y, p_bitmap, (BlockType)(rand()%NumBlockTypes));
}

void Game::holdFocusBlock()
{
    Block *temp = p_holdBlock;

    p_holdBlock = pFocusBlock;
    if (temp) {
        p_focusBlock = temp;
    }
    else {
        p_focusBlock = p_nextBlock;
        p_focusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y, p_bitmap);
        p_nextBlock = new Block(NEXT_BLOCK_X, NEXT_BLOCK_Y, p_bitmap, (BlockType)(rand()%NumBlockTypes));
    }
}


}

