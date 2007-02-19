#include "Block.h"
#include "defines.h"

#include <string.h>
namespace lithtris 
{

Block::Block()
{}

Block::~Block()
{}

Block::Block(int x, int y, SDL_Surface *bitmap, BlockType type)
{
    p_centerX = x-SQUARE_MEDIAN;
    p_centerY = y-SQUARE_MEDIAN;
    p_type = type;
    p_bitmap = bitmap;
    p_isShadow = false;
    memset(p_squares, 0, 4*sizeof(Square *));

    setupSquares(x, y, bitmap);
}

Block::Block(int x, int y, SDL_Surface *bitmap, BlockType type, int square_xys[8])
{
    p_centerX = x;
    p_centerY = y;
    p_type = type;
    p_bitmap = bitmap;
    p_isShadow = false;
    for (int i=0; i < 4; i++) {
        p_squares[i] = new Square(square_xys[i*2],square_xys[i*2+1], bitmap, type);
    }
}


void Block::draw(SDL_Surface *window)
{
    for (int i=0; i < 4; i++)
    {
        if (p_squares[i]) {
            p_squares[i]->draw(window, p_isShadow);
        }
    }
}

void Block::move(Direction dir)
{
    switch(dir) {
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
    for (int i=0; i < 4; i++)
    {
        p_squares[i]->move(dir);
    }
}

void Block::rotate(Direction dir)
{
    int x1,y1,x2,y2;

    for (int i=0; i < 4; i++)
    {
        //translate to origin
        x1 = p_squares[i]->centerX() - p_centerX;
        y1 = p_squares[i]->centerY() - p_centerY;

        //rotate and untranslate
        if (dir == Right) {
            x2 = -y1 + p_centerX;
            y2 = x1 + p_centerY;
        }
        else 
        if (dir == Left) {
            x2 = y1 + p_centerX;
            y2 = -x1 + p_centerY;
        }

        p_squares[i]->setCenterX(x2);
        p_squares[i]->setCenterY(y2);
    }
}
    


/* protected */
void Block::setupSquares(int x, int y, SDL_Surface *bitmap)
{
    p_centerX = x;
    p_centerY = y;
    for(int i=0; i < 4; i++) if (p_squares[i]) delete p_squares[i];

    switch(p_type) {
        case SquareBlock:
            // [0][2]
            // [1][3]
            p_squares[0] = new Square(x - SQUARE_MEDIAN*2, y - SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[1] = new Square(x - SQUARE_MEDIAN*2, y, bitmap, p_type);
            p_squares[2] = new Square(x, y - SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x, y, bitmap, p_type);
            break;
        case TBlock:
            //    [0]
            // [1][2][3]
            p_squares[0] = new Square(x, y, bitmap, p_type);
            p_squares[1] = new Square(x - SQUARE_MEDIAN*2, y + SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[2] = new Square(x, y + SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x + SQUARE_MEDIAN*2, y + SQUARE_MEDIAN*2, bitmap, p_type);
            break;
        case LineBlock:
            //    [0][1][2][3]
            p_squares[0] = new Square(x - SQUARE_MEDIAN*4,   y, bitmap, p_type);
            p_squares[1] = new Square(x - SQUARE_MEDIAN*2,   y, bitmap, p_type);
            p_squares[2] = new Square(x, y, bitmap, p_type);
            p_squares[3] = new Square(x + SQUARE_MEDIAN*2, y, bitmap, p_type);
            break;
        case LBlock:
            // [0]
            // [1]
            // [2][3]
            p_squares[0] = new Square(x - SQUARE_MEDIAN*2, y - SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[1] = new Square(x - SQUARE_MEDIAN*2, y, bitmap, p_type);
            p_squares[2] = new Square(x - SQUARE_MEDIAN*2, y + SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x, y + SQUARE_MEDIAN*2, bitmap, p_type);
            break;
        case JBlock:
            //    [0]
            //    [1]
            // [2][3]
            p_squares[0] = new Square(x, y - SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[1] = new Square(x, y, bitmap, p_type);
            p_squares[2] = new Square(x - SQUARE_MEDIAN*2, y + SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x, y + SQUARE_MEDIAN*2, bitmap, p_type);
            break;
        case SBlock:
            //    [0][1]
            // [2][3]
            p_squares[0] = new Square(x, y, bitmap, p_type);
            p_squares[1] = new Square(x + SQUARE_MEDIAN*2, y, bitmap, p_type);
            p_squares[2] = new Square(x - SQUARE_MEDIAN*2, y+SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x, y + SQUARE_MEDIAN*2, bitmap, p_type);
            break;
        case ZBlock:
            // [0][1]
            //    [2][3]
            p_squares[0] = new Square(x - SQUARE_MEDIAN*2, y, bitmap, p_type);
            p_squares[1] = new Square(x, y, bitmap, p_type);
            p_squares[2] = new Square(x, y + SQUARE_MEDIAN*2, bitmap, p_type);
            p_squares[3] = new Square(x + SQUARE_MEDIAN*2, y + SQUARE_MEDIAN*2, bitmap, p_type);
            break;
    }
}

Block *Block::getRotatedCopy(Direction dir)
{
    int *tmp = getRotatedXY(dir);
    Block *ret = new Block(p_centerX, p_centerY, p_bitmap, p_type, tmp);
    delete tmp;
    return ret;
}

int *Block::getRotatedXY(Direction dir)
{
    int *ret = new int[8];
    int x1,y1,x2,y2;

    for (int i=0; i < 4; i++)
    {
        x1 = p_squares[i]->centerX() - p_centerX;
        y1 = p_squares[i]->centerY() - p_centerY;

        if (dir == Right) {
            x2 = (-y1) + p_centerX;
            y2 = (x1) + p_centerY;
        }
        else
        if (dir == Left) {
            x2 = y1 + p_centerX;
            y2 = (-x1) + p_centerY; 
        }
        else {
            x2 = x1 + p_centerX;
            y2 = y1 + p_centerY;
        }

        ret[i*2] = x2;
        ret[i*2+1] = y2;
    }

    return ret;
}

};
