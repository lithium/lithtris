#include "Game.h"

namespace lithtris
{

SDL_Rect Game::getRowCol(int x, int y)
{
    int row_height = SQUARE_MEDIAN*2;
    int bottom = PLAYAREA_Y+PLAYAREA_H;
    int top = bottom - (MAX_ROWS * row_height);
    int left = PLAYAREA_X;
    SDL_Rect ret = {(y - top) / row_height, (x - left) / row_height, row_height,row_height};
    return ret;
}

bool Game::checkWallCollisions(Square *square, Direction dir)
{
    if (!square) return true;
    int distance = SQUARE_MEDIAN*2;
    int x = square->centerX();
    int y = square->centerY();
    switch (dir) 
    {
        case Down:
            if ( y + distance >= PLAYAREA_Y+PLAYAREA_H ) return true;
            break;
        case Left:
            if ( x - distance < PLAYAREA_X ) return true;
            break;
        case Right:
            if ( x + distance >= PLAYAREA_X+PLAYAREA_W ) return true;
            break;
    }
    return false;
}

bool Game::checkSquareOutside(Square *square, Direction dir)
{
    int x = square->centerX();
    int y = square->centerY();

    if (dir == Left || dir == Right) {
        if ((x < PLAYAREA_X) || (x > PLAYAREA_X+PLAYAREA_W))
            return true;
    }
    if (dir == Down) { 
        if (( y-SQUARE_MEDIAN <= PLAYAREA_Y))
            return true;
    }
    return false;
}
bool Game::checkBlockOutside(Block *block, Direction dir)
{
    Square **sqs = block->squares();
    for (int i=0; i < 4; i++) {
        if (checkSquareOutside(sqs[i],dir)) {
            return true;
        }
    }
    return false;
}

bool Game::checkPileCollisions(Square *square, Direction dir)
{
    int distance = SQUARE_MEDIAN*2;
    int x = square->centerX();
    int y = square->centerY();

    /* calc where square would end up after move */
    switch (dir) 
    {
        case Down: y += distance; break;
        case Left: x -= distance; break;
        case Right: x += distance; break;
    }

    /* determine row,col check if a block is in pile there */
    SDL_Rect r = getRowCol(x,y);
    if (p_pile[r.x][r.y]) return true;

    return false;
}
bool Game::checkWallCollisions(Block *block, Direction dir)
{
    Square **sqs = block->squares();
    for (int i=0; i < 4; i ++) {
        //printf("Sq[%u] = %d <= %d\n",i,sqs[i]->centerY(), PLAYAREA_Y+PLAYAREA_H);
        if (checkWallCollisions(sqs[i], dir))
            return true;
    }
    //printf("\n");
    return false;
}
bool Game::checkPileCollisions(Block *block, Direction dir)
{
    Square **sqs = block->squares();
    for (int i=0; i < 4; i ++) {
        if (checkPileCollisions(sqs[i], dir))
            return true;
    }
    return false;
}

bool Game::checkRotationCollisions(Block *block, Direction dir)
{
    Block *rot = block->getRotatedCopy(dir);
    bool ret;

    ret = checkBlockOutside(rot);
    if (ret) 
        goto check_rotation_collisions_exit;

    ret = checkCollisions(rot, dir);

check_rotation_collisions_exit:
    delete rot;
    return ret;
}
 
int Game::checkCompletedLines()
{
    int num_lines = 0;
    int i,c,row;

    for (i = 0; i < MAX_ROWS; i++) {
        for (c = 0; c < SQUARES_PER_ROW; c++) {
            if (!p_pile[i][c])  { // found an empty square, goto next row
                goto continue_outer;
            }
        }
        // we found no empty squares, a complete line!
        num_lines++;

        //erase the row
        for (c = 0; c < SQUARES_PER_ROW; c++) {
            delete p_pile[i][c];
            p_pile[i][c] = 0;
        }

        //move everything above it down one row
        for (row=i-1; row >= 0; row--) {
            for (c=0; c < SQUARES_PER_ROW; c++) {
                if (p_pile[row][c]) {
                    p_pile[row][c]->move(Down);
                    p_pile[row+1][c] = p_pile[row][c];
                    p_pile[row][c] = 0;
                }
            }
        }
continue_outer:
        ;
    }

    return num_lines;
}

bool Game::checkWin()
{
    if (p_level >= MAX_LEVEL) {
        p_which_menu = WinnerMenu;
        reset(&Game::menu_state);
        return true;
    }
    return false;
}

bool Game::checkLoss(Block *block)
{
    if (checkBlockOutside(block, Down)) {
        haltMusic();
        restart();
        p_which_menu = LoserMenu;
        pushState(&Game::menu_state);
        return true;
    }
    /*
    if (checkPileCollisions(p_focusBlock, Down)) {
        reset(&Game::lost_state); 
    }
    */
    return false;
}


}
