#ifndef ENUMS_H
#define ENUMS_H

namespace lithtris
{

enum DrawState {
    NoTransition = 0,
    PlainTransition, // new piece no lines cleared
    LineTransition,  // lines cleared
    LevelTransition, // level changed
    NumDrawStates,
};

enum MenuId
{
    InvalidMenu=0,
    MainMenu,       
                    // Play
                    // Options
                    // Scores
                    // Quit
    PauseMenu,
                    // Resume
                    // Keys
                    // Quit    
    OptionsMenu,    
                    // Keys
                    // Shadow: on
                    // Hold: on
                    // Back
    ScoresMenu,
    KeysMenu,   
                    // Move L:
                    // Move R:
                    // Rotate L:
                    // Rotate R:
                    // Move down:
                    // Hard drop:
                    // Soft drop:
                    // Hold:
                    // Back
    ExitMenu,
                    // yes
                    // no
    WinnerMenu,
                    // restart
                    // quit
    LoserMenu,
                    // restart
                    // quit
    NumMenuIds,
    // hack for toggle shit
    MusicToggle,
    CascadeToggle,
    ScoreToggle,
    ShadowToggle,
    HoldToggle,
    InfspinToggle,
    MoveLeftToggle,
    MoveRightToggle,
    SoftDropToggle,
    HardDropToggle,
    SpinLeftToggle,
    SpinRightToggle,
    HoldKeyToggle,
};

enum KeyId
{
    InvalidKey=0,
    MoveLeftKey,
    MoveRightKey,
    SoftDropKey,
    HardDropKey,
    RotLeftKey,
    RotRightKey,
    HoldKey,
    NumKeyIds,
};

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
