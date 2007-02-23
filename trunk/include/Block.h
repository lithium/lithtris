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
    Block(int x, int y, SDL_Surface *bitmap, BlockType type, int xy_squares[8]);

    void draw(SDL_Surface *window);
    void move(Direction dir);
    void rotate(Direction dir);
    
    Square **squares() { return p_squares; }
    Block *getRotatedCopy(Direction dir);
    int *getRotatedXY(Direction dir);
    void setupSquares(int x, int y, SDL_Surface *bitmap);

    bool isShadow() { return p_isShadow; }
    void setIsShadow(bool b) { p_isShadow = b; }
    int p_centerX;
    int p_centerY;

    BlockType type() { return p_type; }
private:
    bool p_isShadow;
    BlockType p_type;
    SDL_Surface *p_bitmap;
    Square *p_squares[4];
};

};


#endif
