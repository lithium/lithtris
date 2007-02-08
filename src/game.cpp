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


void Game::play()
{
    static int force_down_counter = 0;
    static int slide_counter = 0;

    while (!p_stateStack.empty()) {
        if (p_stateStack.top().statePointer)
            p_stateStack.top().statePointer();
    }
}


/* protected */
void Game::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    p_window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);
    p_timer = SDL_GetTicks();
    p_bitmap = SDL_LoadBMP("data/blocks.bmp");

    state_t state;
    state.statePointer = &Game::exit;
    p_stateStack.push(state);

    state.statePointer = &Game::menu;
    p_stateStack.push(state);
    TTF_Init();


    srand(time(0));
    BlockType type = rand() % NumBlockTypes;
    p_focusBlock = new Block(BLOCK_START_X, BLOCK_START_Y, p_bitmap, type);
    fillNextBlocks(3);
}
void Game::shutdown()
{
    int i;
    delete(p_focusBlock);
    for (i=0; i < p_nextBlocks.size(); i++) {
        delete(p_nextBlocks[i]);
    }
    p_nextBlocks.erase(p_nextBlocks.begin(),p_nextBlocks.end());
    for (i=0; i < p_oldSquares; i++) {
        delete(p_oldSquares[i]);
    }
    p_oldSquares.erase(p_oldSquares.begin(),p_oldSquares.end());

    TTF_Quit();
    SDL_FreeSurface(p_bitmap);
    SDL_FreeSurface(p_window);
    SDL_Quit();
}

Game::fillNextBlocks(int num_next_blocks)
{
    while (p_nextBlocks.size() < p_num_next_blocks)
    {
        BlockType type = rand() % NumBlockTypes;
        Block *t = new Block(NEXT_BLOCK_RECT_X, NEXT_BLOCK_RECT_Y, p_bitmap, type);
        p_nextBlocks.push_back(t);
    }
}


void Game::drawBackground()
{
    SDL_Rect src = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_BlitSurface(p_bitmap, &src, p_window, &src);
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

void Game::handleMenuInput()
{
    if ( SDL_PollEvent(&p_event) )
    {
        if (p_event.type == SDL_QUIT) {
            while (!p_stateStack.empty())
                p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE) {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_q) {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_s) {
                state_t tmp;
                tmp.statePointer = this::*play;
                p_stateStack.push(tmp);
                return;
            }
        }
    }
}
void Game::handleExitInput()
{
    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT)
        {
            while (!p_stateStack.empty()) p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE)
            {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_y)
            {
                p_stateStack.pop();
                return;
            }
            if (p_event.key.keysym.sym == SDLK_n)
            {
                state_t tmp;
                tmp.statePointer = this::*play;
                p_stateStack.push(tmp);
                return;
            }
        }
    }

}

void Game::handlePlayInput()
{
    if ( SDL_PollEvent(&p_event)) {
        if (p_event.type == SDL_QUIT)
        {
            while (!p_stateStack.empty()) p_stateStack.pop();
            return;
        }

        if (p_event.type == SDL_KEYDOWN) {
            if (p_event.key.keysym.sym == SDLK_ESCAPE)
            {
                p_stateStack.pop();
                return;
            }
        }
    }
}


/* states */
void Game::menu()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleMenuInput();
        clearScreen();
        displayText("(S)tart game", 120, 120, 12, 255, 255, 255, 0, 0, 0);
        displayText("(Q)uit game", 120, 150, 12, 255, 255, 255, 0, 0, 0);
        SDL_UpdateRect(p_window, 0, 0, 0, 0);
        p_timer = SDL_GetTicks();
    }
}

void Game::exit()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handleExitInput();
        clearScreen();
        displayText("Quit Game? (y or n)", 100, 150, 255, 255,255, 0,0,0);
        SDL_UpdateRect(p_window,0,0,0,0);
        p_timer = SDL_GetTicks();
    }
}

void Game::play()
{
    if ((SDL_GetTicks() - p_timer) >= FRAME_RATE)
    {
        handlePlayInput();
        clearScreen();
        drawBackground();
        SDL_UpdateRect(p_window,0,0,0,0);
        p_timer = SDL_GetTicks();
    }
}

}

