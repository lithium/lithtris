#ifndef ENUMS_H
#define ENUMS_H

namespace lithtris
{

enum BlockType
{
    SquareBlock,
    TBlock,
    LineBlock,
    LBlock,
    JBlock,
    SBlock,
    ZBlock,
    NumBlockTypes,
};

enum Direction
{
    InvalidDirection=0,
    Left,
    Right,
    Down
};

};

#endif
