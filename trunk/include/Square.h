#ifndef SQUARE_H
#define SQUARE_H

#include "enums.h"
#include "defines.h"
#include "SDL.h"
#include "SDL_main.h"

namespace lithtris 
{

class Square
{
public:
    Square();
    Square(int x, int y, SDL_Surface *bitmap, BlockType type, bool is_shadow = false);
    ~Square();

    void draw(SDL_Surface *window, bool is_shadow = false);
    void move(Direction dir);

    bool isShadow() { return p_isShadow; }
    void setIsShadow(bool b) { p_isShadow = b; }

    int centerX() { return p_centerX; }
    int centerY() { return p_centerY; }
    void setCenterX(int x) { p_centerX = x; }
    void setCenterY(int y) { p_centerY = y; }
private:
    int p_centerX;
    int p_centerY;

    bool p_isShadow;
    
    enum BlockType p_blockType;
    SDL_Surface *p_bitmap;
};


};
#endif
