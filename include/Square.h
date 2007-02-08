#ifndef SQUARE_H
#define SQUARE_H

#include "enums.h"
#include "defines.h"

namespace lithtris 
{

class Square
{
public:
    Square();
    Square(int x, int y, SDL_Surface *bitmap, BlockType type);
    ~Square();

    void draw(SDL_Surface *window);
    void move(Direction dir);

    int centerX() { return p_centerX; }
    int centerY() { return p_cetnerY; }
    void setCenterX(int x) { p_centerX = x; }
    void setCenterY(int y) { p_centerY = y; }
private:
    int p_centerX;
    int p_centerY;
    
    enum BlockType p_blockType;
    SDL_Surface *p_bitmap;
};


};
#endif
