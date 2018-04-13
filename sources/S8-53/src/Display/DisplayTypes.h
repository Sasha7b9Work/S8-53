#pragma once


//#include "defines.h"


#define SCREEN_WIDTH            320
#define SCREEN_WIDTH_IN_BYTES   (320 / 8)
#define SCREEN_HEIGHT           240
#define SCREEN_HEIGHT_IN_BYTES  (240 / 8)
#define SIZE_BUFFER_FOR_SCREEN  ((SCREEN_WIDTH) * (SCREEN_HEIGHT) / 8)

#define GRID_DELTA              20
#define GRID_TOP                19
#define GRID_CELLS_IN_HEIGHT    10
#define GRID_HEIGHT             ((GRID_CELLS_IN_HEIGHT) * (GRID_DELTA))
#define GRID_BOTTOM             ((GRID_TOP) + (GRID_HEIGHT))
#define GRID_WIDTH              (280)

#define GRID_RIGHT              ((Grid::Left()) + (Grid::Width()))

#define CUR_WIDTH               6
#define CUR_HEIGHT              8

#define MP_Y                    ((GRID_TOP) + 1)
#define MP_TITLE_WIDTH          79
#define MP_X                    ((GRID_RIGHT) - (MP_TITLE_WIDTH) - 1)
#define MP_TITLE_HEIGHT         34
#define MI_HEIGHT               33
#define MI_WIDTH                ((MP_TITLE_WIDTH) + 1)
#define MI_HEIGHT_VALUE         13
#define MI_WIDTH_VALUE          ((MI_WIDTH) - 4)
#define MOSI_HEIGHT             14
#define MOI_HEIGHT_TITLE        19
#define MOI_WIDTH               MP_TITLE_WIDTH
#define MOI_WIDTH_D_IP          34      /* Увеличение ширины открытого ip-адреса в случае наличия порта */
#define WIDTH_SB                19
#define LEFT_SB                 299 

typedef enum
{
    DrawMode_Auto,
    DrawMode_Hand
} DrawMode;

typedef enum
{
    SB_Signals,
    SB_Send,
    SB_Intermediate
} SourceBuffer;

typedef enum
{
    TypeFont_5,
    TypeFont_8,
    TypeFont_UGO,
    TypeFont_UGO2,
    TypeFont_Number,
    TypeFont_None
} TypeFont;


typedef enum
{
    SET_COLOR = 0x01,                   // 1 numColor:1
    FILL_REGION = 0x02,                 // 2 x:2 y:1 width:2 height:1
    END_SCENE = 0x03,                   // 3
    DRAW_HLINE = 0x04,                  // 4 y:1 x0:2 x1:2
    DRAW_VLINE = 0x05,                  // 5 x:2 y0:1 y1:1
    SET_POINT = 0x06,                   // 6 x:2 y:1
    DRAW_SIGNAL_LINES = 0x07,           // 7 x:2 points:281
    DRAW_TEXT = 0x08,                   // 
    SET_PALETTE = 0x09,                 // 9 numColor:1 colorValue:2
    SET_FONT = 0x0a,                    // 10 numFont:1
    DRAW_HPOINT_LINE = 0x0b,            //
    DRAW_VPOINT_LINE = 0x0c,            // 12 x:2 y0:1 delta:1 numPoints:1
    DRAW_VLINES_ARRAY = 0x0d,           // 13 x0:2 numLines:1 lines:numLines * 2
    DRAW_SIGNAL_POINTS = 0x0e,          // 14 x:2 points:281
    SET_BRIGHTNESS = 0x0f,              //
    DRAW_MULTI_HPOINT_LINES = 0x10,     //
    DRAW_MULTI_HPOINT_LINES_2 = 0x11,   // 17 numLines:1 x:2 numPoints:1 dx:1 lines:numLines
    DRAW_MULTI_VPOINT_LINES = 0x12,     // 18 numLines:1 y:1 numPoints:1 dy:1 empty:1 lines:numLines*2
    LOAD_FONT = 0x13,                   // 19 typeFont:1 height:4 symbols:9 * 256 . Описание symbols смотри в Font.h
    DRAW_PICTURE = 0x14,                // 
    GET_POINT = 0x15,                   // 
    RUN_BUFFER = 0x16,
    SET_REINIT = 0x17,
    SET_ORIENTATION = 0x18,             // Установить ориентацию экрана. 0 - нормальная, 1 - обратная. Действительна только в С8-54
    NUM_COMMANDS
} Command;
