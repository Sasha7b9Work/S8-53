// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Grid.h"
#include "Settings/Settings.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GridLeft(void)
{
    return (MenuIsMinimize() && MenuIsShown() ? 9 : 20) + Measure_GetDeltaGridLeft();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridRight(void)
{
    return ((MenuIsMinimize() && MenuIsShown()) ? 9 : 20) + 280;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridChannelBottom(void)
{
    return (sDisplay_IsSeparate()) ? (GRID_TOP + GRID_HEIGHT / 2) : GridFullBottom();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridSignalWidth(void)
{
    return GridWidth();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridChannelHeight(void)
{
    return (sDisplay_IsSeparate()) ? GridFullHeight() / 2 : GridFullHeight();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridChannelCenterHeight(void)
{
    return (GRID_TOP + GridChannelBottom()) / 2;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridFullBottom(void)
{
    return GRID_BOTTOM - Measure_GetDeltaGridBottom();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridFullHeight(void)
{
    return GridFullBottom() - GRID_TOP;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridWidth(void)
{
    return GridRight() - GridLeft();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridFullCenterHeight(void)
{
    return (GridFullBottom() + GRID_TOP) / 2;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridWidthInCells(void)
{
    return MenuIsShown() ? 10 : 14;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float GridDeltaY(void)
{
    float delta = (GridFullBottom() - GRID_TOP) / 10.0f;
    return sDisplay_IsSeparate() ? (delta / 2.0f) : delta;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float GridDeltaX(void)
{
    float delta = (GridRight() - GridLeft()) / 14.0f;
    return delta;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridMathTop(void)
{
    return GridMathBottom() - GridMathHeight();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridMathHeight(void)
{
    if (ENABLED_FFT || MODE_DRAW_MATH_IS_SEPARATE)
    {
        return GridFullHeight() / 2;
    }
    return GridFullHeight();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int GridMathBottom(void)
{
    return GridFullBottom();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int BottomMessages(void)
{
    int retValue = GridFullBottom();
    if (MODE_WORK_IS_MEMINT)
    {
        retValue -= 12;
    }
    else if (SHOW_MEASURES)
    {
        retValue = gBF.topMeasures;
        if (MEAS_NUM_IS_6_1 || MEAS_NUM_IS_6_2 || MEAS_NUM_IS_1)
        {
            retValue = GridFullBottom();
        }
    }
    return retValue - 12;;
}
