#include "defines.h"
#include "Log.h"
#include "SCPI.h"
#include "Globals.h"
#include "VCP/VCP.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Map.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENTER_PARSE_FUNC(SCPI::ProcessDISPLAY)
    {"AUTOSEND",    SCPI::DISPLAY::AUTOSEND},
    {"MAPPING",     SCPI::DISPLAY::MAPPING},    // Отображение
    {"MAP",         SCPI::DISPLAY::MAPPING},
    {"ACCUMULATION",SCPI::DISPLAY::ACCUM},      // Накопление
    {"ACCUM",       SCPI::DISPLAY::ACCUM},     
    {"AVERAGE",     SCPI::DISPLAY::AVERAGE},    // Усреднение
    {"AVE",         SCPI::DISPLAY::AVERAGE},
    {"MINMAX",      SCPI::DISPLAY::MINMAX},     // Мин Макс
    {"FILTR",       SCPI::DISPLAY::FILTR},      // Сглаживание
    {"FPS",         SCPI::DISPLAY::FPS},        // Частота обновл
    {"WINDOW",      SCPI::DISPLAY::WINDOW},     // Окно памяти
    {"GRID",        SCPI::DISPLAY::GRID},       // СЕТКА
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(SCPI::DISPLAY::GRID)
    {"TYPE",        SCPI::DISPLAY::GRID_TYPE},
    {"BRIGHTNESS",  SCPI::DISPLAY::GRID_BRIGHTNESS},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(SCPI::DISPLAY::ACCUM)
    {"NUMBER",  SCPI::DISPLAY::ACCUM_NUMBER},
    {"NUM",     SCPI::DISPLAY::ACCUM_NUMBER},
    {"MODE",    SCPI::DISPLAY::ACCUM_MODE},
    {"CLEAR",   SCPI::DISPLAY::ACCUM_CLEAR},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
ENTER_PARSE_FUNC(SCPI::DISPLAY::AVERAGE)
    {"NUMBER",  SCPI::DISPLAY::AVERAGE_NUMBER},
    {"NUM",     SCPI::DISPLAY::AVERAGE_NUMBER},
    {"MODE",    SCPI::DISPLAY::AVERAGE_MODE},
LEAVE_PARSE_FUNC


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::DISPLAY::AUTOSEND(uint8 *buffer)
{
    static int counter = 0;
    counter++;
    
    static const MapElement map[] =
    {
        {"1", 1},       // Этот запрос для запроса первого фрейма с палитрой и шрифтами
        {"2", 2},       // Этот запрос для запроса последующих фреймов
        {"3", 3},       // Этот запрос для запроса первого фрейма без палитры и шрифтов
        {0}
    };
    ENTER_ANALYSIS
        if (1 == value)      { Painter::SendFrame(true);  }
        else if (2 == value) { Painter::SendFrame(false); }
        else if (3 == value) { Painter::SendFrame(true, true); }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::DISPLAY::MAPPING(uint8 *buffer)
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
void SCPI::DISPLAY::ACCUM_NUMBER(uint8 *buffer)
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
void SCPI::DISPLAY::ACCUM_MODE(uint8 *buffer)
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
            SCPI_SEND(":DISPLAY:ACCUMULATION:MODE %s", map[MODE_ACCUM].key);
        }
    LEAVE_ANALYSIS
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::DISPLAY::ACCUM_CLEAR(uint8 *buffer)
{
    OnPress_Accumulation_Clear();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::DISPLAY::AVERAGE_NUMBER(uint8 *buffer)
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
void SCPI::DISPLAY::AVERAGE_MODE(uint8 *buffer)
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
void SCPI::DISPLAY::MINMAX(uint8 *buffer)
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
void SCPI::DISPLAY::FILTR(uint8 *buffer)
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
void SCPI::DISPLAY::FPS(uint8 *buffer)
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
void SCPI::DISPLAY::WINDOW(uint8 *buffer)
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
void SCPI::DISPLAY::GRID_BRIGHTNESS(uint8 *buffer)
{
    int intVal = 0;
    if (SCPI::FirstIsInt(buffer, &intVal, 0, 100))
    {
        BRIGHTNESS_GRID = (int16)intVal;
        Display::RunAfterDraw(OnChanged_Grid_Brightness);
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
void SCPI::DISPLAY::GRID_TYPE(uint8 *buffer)
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
