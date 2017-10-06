#include "defines.h"
#include "SCPI.h"
#include "Globals.h"
#include "VCP/VCP.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Map.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Process_AUTOSEND(uint8 *buffer);
static void Process_MAPPING(uint8 *buffer);
static void Process_ACCUM(uint8 *buffer);
static void Process_ACCUM_NUMBER(uint8 *buffer);
static void Process_ACCUM_MODE(uint8 *buffer);
static void Process_ACCUM_CLEAR(uint8 *buffer);
static void Process_AVERAGE(uint8 *buffer);
static void Process_AVERAGE_NUMBER(uint8 *buffer);
static void Process_AVERAGE_MODE(uint8 *buffer);
static void Process_MINMAX(uint8 *buffer);
static void Process_FILTR(uint8 *buffer);
static void Process_FPS(uint8 *buffer);
static void Process_WINDOW(uint8 *buffer);
static void Process_GRID(uint8 *buffer);
static void Process_GRID_TYPE(uint8 *buffer);
static void Process_GRID_BRIGHTNESS(uint8 *buffer);


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(Process_DISPLAY)
    {"AUTOSEND",    Process_AUTOSEND},
    {"MAPPING",     Process_MAPPING},   // Отображение
    {"MAP",         Process_MAPPING},
    {"ACCUMULATION",Process_ACCUM},     // Накопление
    {"ACCUM",       Process_ACCUM},     
    {"AVERAGE",     Process_AVERAGE},   // Усреднение
    {"AVE",         Process_AVERAGE},
    {"MINMAX",      Process_MINMAX},    // Мин Макс
    {"FILTR",       Process_FILTR},     // Сглаживание
    {"FPS",         Process_FPS},       // Частота обновл
    {"WINDOW",      Process_WINDOW},    // Окно памяти
    {"GRID",        Process_GRID},      // СЕТКА
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(Process_GRID)
    {"TYPE",        Process_GRID_TYPE},
    {"BRIGHTNESS",  Process_GRID_BRIGHTNESS},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(Process_ACCUM)
    {"NUMBER",  Process_ACCUM_NUMBER},
    {"NUM",     Process_ACCUM_NUMBER},
    {"MODE",    Process_ACCUM_MODE},
    {"CLEAR",   Process_ACCUM_CLEAR},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(Process_AVERAGE)
    {"NUMBER",  Process_AVERAGE_NUMBER},
    {"NUM",     Process_AVERAGE_NUMBER},
    {"MODE",    Process_AVERAGE_MODE},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Process_AUTOSEND(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 1},
        {"2", 2},
        {0}
    };
    ENTER_ANALYSIS
        if (1 == value)         { painter.SendFrame(true); }
        else if (2 == value)    { painter.SendFrame(false); } 
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_MAPPING(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"POINTS",  1},
        {"LINES",   2},
        {"?",       3},
        {0}
    };
    ENTER_ANALYSIS
        if (1 == value)         { MODE_DRAW_SIGNAL = ModeDrawSignal_Points; }
        else if (2 == value)    { MODE_DRAW_SIGNAL = ModeDrawSignal_Lines; }
        else if (3 == value)    { SCPI_SEND(":DISPLAY:MAPPING %s", MODE_DRAW_IS_SIGNAL_LINES ? "LINES" : "POINTS"); }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnPress_Accumulation_Clear();

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_ACCUM_NUMBER(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"DIS",         0},
        {"2",           1},
        {"4",           2},
        {"8",           3},
        {"16",          4},
        {"32",          5},
        {"64",          6},
        {"128",         7},
        {"INFINITY",    8},
        {"?",           9},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 7)         { ENUM_ACCUM = (ENumAccumulation)value; }
        else if (8 == value)    { ENUM_ACCUM = NumAccumulation_Infinity; }
        else if (9 == value)
        {
            SCPI_SEND(":DISPLAY:ACCUMULATION:NUMBER %s", map[ENUM_ACCUM].key);
        }
    LEAVE_ANALYSIS
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_ACCUM_MODE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"NORESET", 0},
        {"RESET",   1},
        {"?",       2},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 1) { MODE_ACCUM = (ModeAccumulation)value; }
        else if (2 == value)
        {
            SCPI_SEND(":DISPLAY:ACCUMULATION:MODE %s", map[ENUM_ACCUM].key);
        }
    LEAVE_ANALYSIS
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_ACCUM_CLEAR(uint8 *buffer)
{
    OnPress_Accumulation_Clear();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_AVERAGE_NUMBER(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"DIS", 0},
        {"2",   1},
        {"4", 2},
        {"8", 3},
        {"16", 4},
        {"32", 5},
        {"64", 6},
        {"128", 7},
        {"256", 8},
        {"512", 9},
        {"?", 10},
        {0}
    };

    ENTER_ANALYSIS
        if (value <= 9) { ENUM_AVE = (ENumAveraging)value; }
        else if (10 == value)
        {
            SCPI_SEND(":DISPLAY:AVERAGE:NUMBER %s", map[ENUM_AVE].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_AVERAGE_MODE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ACCURACY",    0},
        {"APPROXIMATE", 1},
        {"?",           2},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 1) { MODE_AVE = (ModeAveraging)value; }
        else if (2 == value)
        {
            SCPI_SEND(":DISPLAY:AVARAGE:MODE %s", map[MODE_AVE].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_MINMAX(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"4", 2},
        {"8", 3},
        {"16", 4},
        {"32", 5},
        {"64", 6},
        {"128", 7},
        {"DIS", 8},
        {"?", 9},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 7)         { ENUM_MIN_MAX = (ENumMinMax)value; }
        else if (8 == value)    { ENUM_MIN_MAX = NumMinMax_1; }
        else if (9 == value)
        {
            SCPI_SEND(":DISPLAY:MINMAX %s", map[ENUM_MIN_MAX].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_FILTR(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"3", 2},
        {"4", 3},
        {"5", 4},
        {"6", 5},
        {"7", 6},
        {"8", 7},
        {"9", 8},
        {"10", 9},
        {"DIS", 10},
        {"?", 11},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= 9)         { SMOOTHING = (Smoothing)value; }
        else if (10 == value)   { SMOOTHING = Smoothing_Disable; }
        else if (11 == value)
        {
            SCPI_SEND(":DISPLAY:FILTR %s", map[SMOOTHING].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnChanged_RefreshFPS(bool active);


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_FPS(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"25",  0},
        {"10",  1},
        {"5",   2},
        {"2",   3},
        {"1",   4},
        {"?",   5},
        {0}
    };
    ENTER_ANALYSIS
        if (value < 5) { ENUM_SIGNALS_IN_SEC = (ENumSignalsInSec)value; OnChanged_RefreshFPS(true); }
        else if (5 == value)
        {
            SCPI_SEND(":DISPLAY:FPS %s", map[ENUM_SIGNALS_IN_SEC].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_WINDOW(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"STANDARD",    0},
        {"SIMPLE",      1},
        {"?",           2},
        {0}
    };
    ENTER_ANALYSIS
        if (value == 0) { set.display.showFullMemoryWindow = true; }
        else if (1 == value) { set.display.showFullMemoryWindow = false; }
        else if (2 == value)
        {
            SCPI_SEND(":DISPLAY:WINDOW %s", set.display.showFullMemoryWindow ? "STANDARD" : "SIMPLE");
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnChanged_Grid_Brightness();


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_GRID_BRIGHTNESS(uint8 *buffer)
{
    int intVal = 0;
    if (SCPI_FirstIsInt(buffer, &intVal, 0, 100))
    {
        BRIGHTNESS_GRID = intVal;
        display.RunAfterDraw(OnChanged_Grid_Brightness);
    }
    else
    {
        static const MapElement map[] =
        {
            {"?", 0},
            {0}
        };

        ENTER_ANALYSIS
            if (0 == value)
            {
                extern ColorType colorTypeGrid;
                Color_Init(&colorTypeGrid);
                SCPI_SEND(":DISPLAY:GRID:BRIGHTNESS %d", (int)(colorTypeGrid.brightness * 100.0f));
            }
        LEAVE_ANALYSIS
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void Process_GRID_TYPE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"1", 0},
        {"2", 1},
        {"3", 2},
        {"4", 3},
        {"?", 4},
        {0}
    };

    ENTER_ANALYSIS
        if (value <= 3) { TYPE_GRID = (TypeGrid)value; }
        else if (4 == value)
        {
            SCPI_SEND(":DISPLAY:GRID:TYPE %s", map[TYPE_GRID].key);
        }
    LEAVE_ANALYSIS
}
