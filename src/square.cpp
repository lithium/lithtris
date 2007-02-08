#include "Square.h"

namespace lithris
{

Square::Square()
{}

Square::~Square()
{}

Square(int x, int y, SDL_Surface *bitmap, BlockType type)
{
    p_centerX = x;
    p_centerY = y;
    p_bitmap = bitmap;
    p_blockType = type;
}

void draw(SDL_Surface *window)
{
    int x,y;
    switch (p_blockType) {
        case SquareBlock:
            x = YELLOW_SQUARE_X;
            y = YELLOW_SQUARE_Y;
            break;
        case TBlock:
            x = PURPLE_SQUARE_X;
            y = PURPLE_SQUARE_Y;
            break;
        case LineBlock:
            x = TEAL_SQUARE_X;
            y = TEAL_SQUARE_Y;
            break;
        case LBlock:
            x = BLUE_SQUARE_X;
            y = BLUE_SQUARE_Y;
            break;
        case JBlock:
            x = ORANGE_SQUARE_X;
            y = ORANGE_SQUARE_Y;
            break;
        case SBlock:
            x = RED_SQUARE_X;
            y = RED_SQUARE_Y;
            break;
        case ZBlock:
            x = GREEN_SQUARE_X;
            y = GREEN_SQUARE_Y;
            break;
    }
    SDL_Rect source = {x, y, SQUARE_MERIDIAN*2, SQUARE_MERIDIAN*2};
    SDL_Rect destination = {p_centerX - SQUARE_MEDIAN, p_centerY - SQUARE_MEDIAN, SQUARE_MEDIAN * 2, SQUARE_MEDIAN *2};
    SDL_BlitSurface(p_bitmap, &source, window, &destination);
}

void move(Direction dir)
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
