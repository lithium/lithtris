#ifndef BLOCK_H
#define BLOCK_H

#include "SDL.h"
#include "enums.h"
#include "defines.h"
#include "Square.h"

namespace lithtris
{


class Block {
public:
    Block();
    ~Block();
    Block(int x, int y, SDL_Surface *bitmap, BlockType type);

    void draw(SDL_Surface *window);
    void move(Direction dir);
    void rotate(Direction dir);
    
    Square **squares() { return p_squares; }
    Block *getRotatedCopy(Direction dir);
    void setupSquares(int x, int y, SDL_Surface *bitmap);
private:
    int p_centerX;
    int p_centerY;
    BlockType p_type;
    Square *p_squares[4];
};

};


#endif
