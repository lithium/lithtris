#ifndef DEFINES_H
#define DEFINES_H


#define FRAMES_PER_SECOND 30
#define FRAME_RATE        1000/FRAMES_PER_SECOND


#define MAX_LEVEL       20
#define LINES_PER_LEVEL 10


#define INITIAL_SPEED   70
#define SPEED_CHANGE    3 

#define SLIDE_TIME      15


#define MAX_ROWS 20
#define SQUARES_PER_ROW 10
#define SQUARE_MEDIAN    7  //distance from center of square to sides


// offsets,widths for skin images
#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 310

// falling block area 
#define PLAYAREA_X 110
#define PLAYAREA_Y 20
#define PLAYAREA_W SQUARE_MEDIAN*2*SQUARES_PER_ROW   //140
#define PLAYAREA_H SQUARE_MEDIAN*2*MAX_ROWS          //280

// where the hold peice is displayed
#define HOLDAREA_X 27
#define HOLDAREA_Y 30
#define HOLDAREA_W 56
#define HOLDAREA_H 56

// where the current line in level
#define SCOREAREA_X 31
#define SCOREAREA_Y 160
#define SCOREAREA_W 48
#define SCOREAREA_H 48

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
