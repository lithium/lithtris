#include "Game.h"

namespace lithtris
{

bool Game::checkEntityCollisions(Square *square, Direction dir)
{}

bool Game::checkWallCollisions(Square *square, Direction dir)
{}

bool Game::checkEntityCollisions(Block *block, Direction dir)
{}

bool Game::checkWallCollisions(Block *block, Direction dir)
{}

bool Game::checkRotationCollisions(Block *block)
{}
 
int Game::checkCompletedLines()
{}

bool Game::checkWin()
{
    if (p_level >= NUM_LEVELS) {
        reset(won_state);
    }
}

bool Game::checkLoss()
{
    if (checkEntityCollisions(p_focusBlock, Down)) {
        reset(lost_state); 
    }
}


}
