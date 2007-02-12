#include "Square.h"


namespace lithtris
{

Square::Square()
{}

Square::~Square()
{}

Square::Square(int x, int y, SDL_Surface *bitmap, BlockType type, bool is_shadow)
{
    p_centerX = x;
    p_centerY = y;
    p_bitmap = bitmap;
    p_blockType = type;
    p_isShadow = is_shadow;
}

void Square::draw(SDL_Surface *window, bool is_shadow)
{
    int x;
    int y = 0;
    switch (p_blockType) {
        case SquareBlock: x = YELLOW_X; break;
        case TBlock: x = PURPLE_X; break;
        case LineBlock: x = TEAL_X; break;
        case LBlock: x = ORANGE_X; break;
        case JBlock: x = BLUE_X; break;
        case SBlock: x = RED_X; break;
        case ZBlock: x = GREEN_X; break;
    }
    if (is_shadow || p_isShadow) {
        y = 15;
    }
    SDL_Rect source = {x, y, SQUARE_MEDIAN*2, SQUARE_MEDIAN*2};
    SDL_Rect destination = {p_centerX - SQUARE_MEDIAN, p_centerY - SQUARE_MEDIAN, SQUARE_MEDIAN * 2, SQUARE_MEDIAN *2};
    SDL_BlitSurface(p_bitmap, &source, window, &destination);
}

void Square::move(Direction dir)
{
    switch (dir)
    {
    case Left:
        p_centerX -= SQUARE_MEDIAN * 2;
        break;
    case Right:
        p_centerX += SQUARE_MEDIAN * 2;
        break;
    case Down:
        p_centerY += SQUARE_MEDIAN * 2;
        break;
    default:
        return;
    }
}

};
