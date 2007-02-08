#ifndef ENUMS_H
#define ENUMS_H

namespace lithtris
{

enum BlockType
{
    Invalid=0,
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
    Invalid=0,
    Left,
    Right,
    Down
};

};

#endif
