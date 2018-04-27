#include "Measures.h"
#include "GlobalFunctions.h"
#include "Math.h"
#include "Menu/Pages/PageMeasures.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Hardware/Sound.h"


#include <stdio.h>


typedef struct
{
    const char *name;
    const char UGO;
} StructMeasure;

static const StructMeasure measures[Measure_NumMeasures] =
{
    {"",            '\x00'},
    {"U макс",      '\x20'},
    {"U мин",       '\x25'},
    {"U пик",       '\x2a'},
    {"U макс уст",  '\x40'},
    {"U мин уст",   '\x45'},
    {"U ампл",      '\x4a'},
    {"U ср",        '\x60'},
    {"U скз",       '\x65'},
    {"Выброс+",     '\x6a'},
    {"Выброс-",     '\x80'},
    {"Период",      '\x85'},
    {"Частота",     '\x8a'},
    {"Вр нараст",   '\xa0'},
    {"Вр спада",    '\xa5'},
    {"Длит+",       '\xaa'},
    {"Длит-",       '\xc0'},
    {"Скважн+",     '\xc5'},
    {"Скважн-",     '\xca'},
    {"Задержка\xa7",'\xe0'},
    {"Задержка\xa6",'\xe5'},
    {"Фаза\xa7",    '\xe0'},
    {"Фаза\xa6",    '\xe5'}
};

static int8 posActive = 0;                  // Позиция активного измерения (на котором курсор)
static int8 posOnPageChoice = 0;            // Позиция курсора на странице выбора измерения

bool Measure_IsActive(int row, int col)
{
    if(posActive >= Measure_NumCols() * Measure_NumRows())
    {
        posActive = 0;
    }
    return (row * Measure_NumCols() + col) == posActive;
}

void Measure_GetActive(int *row, int *col)
{
    *row = posActive / Measure_NumCols();
    *col = posActive - (*row) * Measure_NumCols();
}

void Measure_SetActive(int row, int col)
{
    posActive = row * Measure_NumCols() + col;
}

char  Measure_GetChar(Measure measure)
{
    return measures[measure].UGO;
}

int Measure_GetDY()
{
    if(MEAS_SOURCE_IS_A_B)
    {
        return 30;
    }
    return 21;
}

int Measure_GetDX()
{
    return GRID_WIDTH / 5; 
}

const char* Measure_Name(int row, int col)
{
    int numMeasure = row * Measure_NumCols() + col;
    return measures[MEASURE(numMeasure)].name;
}

Measure Measure_Type(int row, int col)
{
    int numMeasure = row * Measure_NumCols() + col;
    return MEASURE(numMeasure);
}

int Measure_GetTopTable()
{
    if(MEAS_NUM_IS_6_2 || MEAS_NUM_IS_6_2)
    {
        return GRID_BOTTOM - Measure_GetDY() * 6;
    }
    return GRID_BOTTOM - Measure_NumRows() * Measure_GetDY();
}

int Measure_NumCols()
{
    static const int cols[] = {1, 2, 5, 5, 5, 1, 2};
    return cols[MEAS_NUM];
}

int Measure_NumRows()
{
    static const int rows[] = {1, 1, 1, 2, 3, 6, 6};
    return rows[MEAS_NUM];
}

int Measure_GetDeltaGridLeft()
{
    if(SHOW_MEASURES && MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if(MEAS_NUM_IS_6_1)
        {
            return Measure_GetDX();
        }
        else if(MEAS_NUM_IS_6_2)
        {
            return Measure_GetDX() * 2;
        }
    }
    return 0;
}

int Measure_GetDeltaGridBottom()
{
    if(SHOW_MEASURES && MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if(MEAS_NUM_IS_1_5)
        {
            return Measure_GetDY();
        }
        else if(MEAS_NUM_IS_2_5)
        {
            return Measure_GetDY() * 2;
        }
        else if(MEAS_NUM_IS_3_5)
        {
            return Measure_GetDY() * 3;
        }
    }
    return 0;
}

void Measure_RotateRegSet(int angle)
{
    static const int8 step = 3;
    static int8 currentAngle = 0;
    currentAngle += (int8)angle;
    if (currentAngle < step && currentAngle > -step)
    {
        return;
    }
    if (PageMeasures::choiceMeasuresIsActive)
    {
        posOnPageChoice += (int8)Math_Sign(currentAngle);
        Sound::RegulatorSwitchRotate();
        if (posOnPageChoice < 0)
        {
            posOnPageChoice = Measure_NumMeasures - 1;
        }
        else if (posOnPageChoice == Measure_NumMeasures)
        {
            posOnPageChoice = 0;
        }
        MEASURE(posActive) = (Measure)posOnPageChoice;
        Painter::ResetFlash();
    }
    else
    {
        int row = 0;
        int col = 0;
        Measure_GetActive(&row, &col);
        col += Math_Sign(currentAngle);
        if (col < 0)
        {
            col = Measure_NumCols() - 1;
            row--;
            if (row < 0)
            {
                row = Measure_NumRows() - 1;
            }
        }
        else if (col == Measure_NumCols())
        {
            col = 0;
            row++;
            if (row >= Measure_NumRows())
            {
                row = 0;
            }
        }
        Measure_SetActive(row, col);
        Sound::RegulatorSwitchRotate();
    }
    currentAngle = 0;
}

void Measure_ShorPressOnSmallButtonSettings()
{
    PageMeasures::choiceMeasuresIsActive = !PageMeasures::choiceMeasuresIsActive;
    if(PageMeasures::choiceMeasuresIsActive)
    {
        posOnPageChoice = MEASURE(posActive);
    }
}

void Measure_ShortPressOnSmallButonMarker()
{
    if(MEASURE_IS_MARKED(posActive))
    {
        MEAS_MARKED = Measure_None;
    }
    else
    {
        MEAS_MARKED = MEASURE(posActive);
    }
}

void Measure_DrawPageChoice()
{
    if(!PageMeasures::choiceMeasuresIsActive)
    {
        return;
    }

    bool num61or62 = MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2;

    int x = num61or62 ? (Grid::Right() - 3 * GRID_WIDTH / 5) : Grid::Left();
    int y = GRID_TOP;
    int dX = GRID_WIDTH / 5;
    int dY = 22;
    int maxRow = num61or62 ? 8 : 5;
    int maxCol = num61or62 ? 3 : 5;
    Measure meas = Measure_None;
    Painter::SetFont(TypeFont_UGO);
    for(int row = 0; row < maxRow; row++)
    {
        for(int col = 0; col < maxCol; col++)
        {
            if(meas >= Measure_NumMeasures)
            {
                break;
            }
            int x0 = x + col * dX;
            int y0 = y + row * dY;
            bool active = meas == posOnPageChoice;
            Painter::DrawRectangleC(x0, y0, dX, dY, COLOR_FILL);
            Painter::FillRegionC(x0 + 1, y0 + 1, dX - 2, dY - 2, active ? COLOR_FLASH_10 : COLOR_BACK);
            Painter::SetColor(active ? COLOR_FLASH_01 : COLOR_FILL);
            Painter::Draw10SymbolsInRect(x0 + 2, y0 + 1, Measure_GetChar(meas));
            if(meas < Measure_NumMeasures)
            {
                Painter::SetFont(TypeFont_5);
                Painter::DrawTextRelativelyRightC(x0 + dX, y0 + 12, measures[meas].name, active ? COLOR_FLASH_01 : COLOR_FILL);
                Painter::SetFont(TypeFont_UGO);
            }
            ++meas;
        }
    }
    Painter::SetFont(TypeFont_8);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Measure& operator++(Measure &measure)
{
    measure = (Measure)((int)measure + 1);
    return measure;
}
