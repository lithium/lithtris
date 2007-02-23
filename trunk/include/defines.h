#ifndef DEFINES_H
#define DEFINES_H

#define WINDOW_CAPTION "lithtris"

#define INF_SPIN_MS 400

#define KEY_ROTATE_LEFT     SDLK_z
#define KEY_ROTATE_RIGHT    SDLK_x
#define KEY_MOVE_LEFT       SDLK_LEFT
#define KEY_MOVE_RIGHT      SDLK_RIGHT
#define KEY_SOFT_DROP       SDLK_DOWN
#define KEY_HARD_DROP       SDLK_UP
#define KEY_HOLD            SDLK_SPACE

#define KEYPRESS_DELAY 60


#define FRAMES_PER_SECOND 60
#define FRAME_RATE        1000/FRAMES_PER_SECOND

#define LINE_TIME FRAMES_PER_SECOND*0.5
#define PLAIN_TIME FRAMES_PER_SECOND*0.5


#define MAX_LEVEL       20
#define LINES_PER_LEVEL 10


#define INITIAL_SPEED   1.2*FRAMES_PER_SECOND
#define SPEED_CHANGE    INITIAL_SPEED / MAX_LEVEL

#define SLIDE_TIME      1.0*FRAMES_PER_SECOND


#define MAX_ROWS 20
#define SQUARES_PER_ROW 10
#define SQUARE_MEDIAN    7  //distance from center of square to sides


#define MENU_TITLE_X    100
#define MENU_TITLE_Y    20
#define MENU_TITLE_W 300
#define MENU_TITLE_H 100

// offsets,widths for skin images
#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 310

// falling block area 
#define PLAYAREA_X 110
#define PLAYAREA_Y 20
#define PLAYAREA_W SQUARE_MEDIAN*2*SQUARES_PER_ROW   //140
#define PLAYAREA_H SQUARE_MEDIAN*2*MAX_ROWS          //280

#define BLOCK_START_X  PLAYAREA_X + (PLAYAREA_W/2)
#define BLOCK_START_Y  PLAYAREA_Y+SQUARE_MEDIAN*2


// where the hold peice is displayed
#define HOLDAREA_X 27
#define HOLDAREA_Y 30
#define HOLDAREA_W 56
#define HOLDAREA_H 56

// where the current line in level
#define LINESAREA_X 31
#define LINESAREA_Y 160
#define LINESAREA_W 48
#define LINESAREA_H 48

// where the current level is shown
#define LEVELAREA_X 31
#define LEVELAREA_Y 244
#define LEVELAREA_W 48
#define LEVELAREA_H 48

// region for each next block
#define NEXTAREA1_X 278
#define NEXTAREA1_Y 30
#define NEXTAREA1_W 56
#define NEXTAREA1_H 56

#define NEXTAREA2_X 348
#define NEXTAREA2_Y 30
#define NEXTAREA2_W 56
#define NEXTAREA2_H 56

#define NEXTAREA3_X 418
#define NEXTAREA3_Y 30
#define NEXTAREA3_W 56
#define NEXTAREA3_H 56

#define NEXTAREA4_X 418
#define NEXTAREA4_Y 100
#define NEXTAREA4_W 56
#define NEXTAREA4_H 56

// offsets into blocks.bmp
#define BLUE_X  0
#define GREEN_X 15
#define RED_X 30
#define TEAL_X 45
#define ORANGE_X 60
#define YELLOW_X 75
#define PURPLE_X 90

#endif
