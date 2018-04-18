#include "Settings.h"
#include "SettingsDisplay.h"
#include "Utils/Math.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sDisplay_NumSignalsInS()
{
    return Tables_ENUMtoNumSignalsInSec(ENUM_SIGNALS_IN_SEC);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_SetNumSignalsInS(int numSignalsInS)
{
    ENUM_SIGNALS_IN_SEC = Tables_NumSignalsInSecToENUM(numSignalsInS);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_NumPointSmoothing()
{
    if (SMOOTHING_IS_DISABLE)
    {
        return 0;
    }
    return (int)SMOOTHING + 1;          // WARN Здесь очень коряво реализовано
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_PointsOnDisplay(int *firstPoint, int *lastPoint)
{
    int numPointsOnScreen = 281;
    *firstPoint = SHIFT_IN_MEMORY;
    *lastPoint = SHIFT_IN_MEMORY + numPointsOnScreen - 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_TimeMenuAutoHide()
{
    static const int times[] =
    {
        0, 5, 10, 15, 30, 60
    };
    return times[MENU_AUTO_HIDE] * 1000;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool sDisplay_IsSeparate()
{
    return (!DISABLED_DRAW_MATH && MODE_DRAW_MATH_IS_SEPARATE) || ENABLED_FFT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void sDisplay_SetBrightness(int16 brightness)
{
    BRIGHTNESS = brightness;
    Painter::SetBrightnessDisplay(brightness);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
ModeAveraging sDisplay_ModeAveraging()
{
    if (sTime_RandomizeModeEnabled())
    {
        return Averaging_Around;
    }
    return MODE_AVE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sDisplay_NumAverage()
{
    if (sTime_RandomizeModeEnabled() && (NUM_AVE_FOR_RAND >= NUM_AVE))
    {
        return NUM_AVE_FOR_RAND;
    }
    return NUM_AVE;
}
