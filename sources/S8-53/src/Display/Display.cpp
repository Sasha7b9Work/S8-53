#include "defines.h"
#include "Globals.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Symbols.h"
#include "Display.h"
#include "Grid.h"
#include "Painter.h"
#include "font/Font.h"
#include "Colors.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"
#include "FPGA/Storage.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/CLOCK.h"
#include "VCP/VCP.h"
#include "Menu/Menu.h"
#include "Menu/Pages/PageCursors.h"
#include "Menu/Pages/PageMemory.h"
#include "Menu/Pages/PageHelp.h"
#include "Utils/Measures.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/ProcessingSignal.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "PainterMem.h"
#include <math.h>
#include <limits.h>
#include "Hardware/DisplayHardware.inc"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_P2P_POINTS (FPGA_MAX_POINTS)
static uint8 dataP2P_0[NUM_P2P_POINTS];
static uint8 dataP2P_1[NUM_P2P_POINTS];
static int   lastP2Pdata = 0;
static bool  dataP2PIsEmpty = true;

#define MAX_NUM_STRINGS         35
#define SIZE_BUFFER_FOR_STRINGS 2000
static char                     *strings[MAX_NUM_STRINGS] = {0};
static char                     bufferForStrings[SIZE_BUFFER_FOR_STRINGS] = {0};
static int                      lastStringForPause = -1;

#define NUM_WARNINGS            10
static const char               *warnings[NUM_WARNINGS] = {0};      // Здесь предупреждающие сообщения.
static uint                     timeWarnings[NUM_WARNINGS] = {0};   // Здесь время, когда предупреждающее сообщение поступило на экран.

static pFuncVV funcOnHand       = 0;
static pFuncVV funcAdditionDraw = 0;
static pFuncVV funcAfterDraw    = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Display::Init() 
{
    /*
    PainterMem_SetBuffer(buffer, 100, 100);
    PainterMem_FillRect(0, 0, 99, 99, ColorChannel(A));
    PainterMem_DrawRectangle(10, 10, 10, 30, COLOR_FILL);
    PainterMem_DrawHLine(90, 10, 90, ColorChannel(B));
    PainterMem_DrawVLine(90, 10, 90, COLOR_BACK);
    PainterMem_DrawRectangle(0, 0, 99, 99, COLOR_FILL);
    */

    Painter::ResetFlash();

    InitHardware();

    Painter::LoadFont(TypeFont_5);
    Painter::LoadFont(TypeFont_8);
    Painter::LoadFont(TypeFont_UGO);
    Painter::LoadFont(TypeFont_UGO2);
    Painter::SetFont(TypeFont_8);

    Painter::LoadPalette();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawStringNavigation() 
{
    char buffer[100];
    char *string = Menu::StringNavigation(buffer);
    if(string) 
    {
        int length = Font_GetLengthText(string);
        int height = 10;
        Painter::DrawRectangleC(Grid::Left(), GRID_TOP, length + 2, height, COLOR_FILL);
        Painter::FillRegionC(Grid::Left() + 1, GRID_TOP + 1, length, height - 2, COLOR_BACK);
        Painter::DrawTextC(Grid::Left() + 2, GRID_TOP + 1, string, COLOR_FILL);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RotateRShift(Channel chan)
{
    ResetP2Ppoints(true);
    LAST_AFFECTED_CHANNEL = chan;
    if(TIME_SHOW_LEVELS)
    {
        (chan == A) ? (SHOW_LEVEL_RSHIFT_0 = 1) : (SHOW_LEVEL_RSHIFT_1 = 1);
        Timer::Enable((chan == A) ? kShowLevelRShift0 : kShowLevelRShift1, TIME_SHOW_LEVELS  * 1000, (chan == A) ? FuncOnTimerDisableShowLevelRShiftA :
                     FuncOnTimerDisableShowLevelRShiftB);
    };
    Display::Redraw();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::FuncOnTimerDisableShowLevelRShiftA()
{
    DisableShowLevelRShiftA();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::FuncOnTimerDisableShowLevelRShiftB()
{
    Display::DisableShowLevelRShiftB();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RotateTrigLev()
{
    if (TIME_SHOW_LEVELS)
    {
        SHOW_LEVEL_TRIGLEV = 1;
        Timer::Enable(kShowLevelTrigLev, TIME_SHOW_LEVELS * 1000, FuncOnTimerDisableShowLevelTrigLev);
    }
    Display::Redraw();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Display::FuncOnTimerDisableShowLevelTrigLev()
{
    Display::DisableShowLevelTrigLev();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::Redraw(void)
{
    NEED_FINISH_REDRAW = 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Display::ChannelNeedForDraw(const uint8 *data, Channel chan, const DataSettings *ds)
{
    if (!data)
    {
        return false;
    }

    if (MODE_WORK_IS_DIRECT)
    {
        if (!sChannel_Enabled(chan))
        {
            return false;
        }
    }
    else if (ds != 0)
    {
        if ((chan == A && ds->enableCh0 == 0) || (chan == B && ds->enableCh1 == 0))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawMarkersForMeasure(float scale, Channel chan)
{
    if (chan == Math)
    {
        return;
    }
    Painter::SetColor(ColorCursors(chan));
    for(int numMarker = 0; numMarker < 2; numMarker++)
    {
        int pos = Processing::GetMarkerHorizontal(chan, numMarker);
        if(pos != ERROR_VALUE_INT && pos > 0 && pos < 200)
        {
            Painter::DrawDashedHLine(Grid::FullBottom() - pos * scale, Grid::Left(), Grid::Right(), 3, 2, 0);
        }

        pos = Processing::GetMarkerVertical(chan, numMarker);
        if (pos != ERROR_VALUE_INT && pos > 0 && pos < Grid::Right())
        {
            Painter::DrawDashedVLine(Grid::Left() + pos * scale, GRID_TOP, Grid::FullBottom(), 3, 2, 0);
        }
       
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
#define CONVERT_DATA_TO_DISPLAY(out, in)                \
    out = (uint8)(maxY - ((in) - MIN_VALUE) * scaleY);  \
    if(out < minY)          { out = (uint8)minY; }      \
    else if (out > maxY)    { out = (uint8)maxY; };


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawSignalLined(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX, 
    bool calculateFiltr)
{
    if (endPoint < startPoint)
    {
        return;
    }
    uint8 dataCD[281];

	int gridLeft = Grid::Left();
	int gridRight = Grid::Right();
    
    int numPoints = sMemory_GetNumPoints(false);
    int numSmoothing = sDisplay_NumPointSmoothing();

    if (ds->peakDet == PeackDet_Disable)
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            float x0 = gridLeft + (i - startPoint) * scaleX;
            if (x0 >= gridLeft && x0 <= gridRight)
            {
                int index = i - startPoint;
                int y = calculateFiltr ? Math_CalculateFiltr(data, i, numPoints, numSmoothing) : data[i];
                CONVERT_DATA_TO_DISPLAY(dataCD[index], y);
            }
        }
    }
    else
    {
        int shift = (int)ds->length1channel;

        int yMinNext = -1;
        int yMaxNext = -1;
       
        for (int i = startPoint; i < endPoint; i++)
        {
            float x = gridLeft + (i - startPoint) * scaleX;
            if (x >= gridLeft && x <= gridRight)
            {
                int yMin = yMinNext;
                if (yMin == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMin, data[i + shift]);
                }
                int yMax = yMaxNext;
                if (yMax == -1)
                {
                    CONVERT_DATA_TO_DISPLAY(yMax, data[i]);
                }

                CONVERT_DATA_TO_DISPLAY(yMaxNext, data[i + 1]);
                if (yMaxNext < yMin)
                {
                    yMin = yMaxNext + 1;
                }

                CONVERT_DATA_TO_DISPLAY(yMinNext, data[i + shift + 1]);
                if (yMinNext > yMax)
                {
                    yMax = yMinNext - 1;
                }

                Painter::DrawVLine((int)x, yMin, yMax);
            }
        }
    }

    if(endPoint - startPoint < 281)
    {
        int _numPoints = 281 - (endPoint - startPoint);
        for(int i = 0; i < _numPoints; i++)
        {
            int index = endPoint - startPoint + i;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], MIN_VALUE);
        }
    }

    if(ds->peakDet == PeackDet_Disable)
    {
        CONVERT_DATA_TO_DISPLAY(dataCD[280], data[endPoint]);
        Painter::DrawSignal(Grid::Left(), dataCD, true);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawSignalPointed(const uint8 *data, const DataSettings *ds, int startPoint, int endPoint, int minY, int maxY, float scaleY, float scaleX)
{
    int numPoints = sMemory_GetNumPoints(false);
    int numSmoothing = sDisplay_NumPointSmoothing();
    
    uint8 dataCD[281];

    if (scaleX == 1.0f) //-V550
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            CONVERT_DATA_TO_DISPLAY(dataCD[index], Math_CalculateFiltr(data, i, numPoints, numSmoothing));
        }
        Painter::DrawSignal(Grid::Left(), dataCD, false);

        if (ds->peakDet)
        {
            int size = endPoint - startPoint;
            startPoint += numPoints;
            endPoint = startPoint + size;;

            for (int i = startPoint; i < endPoint; i++)
            {
                int index = i - startPoint;
                CONVERT_DATA_TO_DISPLAY(dataCD[index], Math_CalculateFiltr(data, i, numPoints, numSmoothing));
            }
            Painter::DrawSignal(Grid::Left(), dataCD, false);
        }
    }
    else
    {
        for (int i = startPoint; i < endPoint; i++)
        {
            int index = i - startPoint;
            int dat = 0;
            CONVERT_DATA_TO_DISPLAY(dat, Math_CalculateFiltr(data, i, numPoints, numSmoothing));
            Painter::SetPoint(Grid::Left() + index * scaleX, dat);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
// Если data == 0, то данные брать из GetData
void Display::DrawDataChannel(uint8 *data, Channel chan, DataSettings *ds, int minY, int maxY)
{
    bool calculateFiltr = true;
    if (data == 0)
    {
        calculateFiltr = false;
        if (chan == A)
        {
            Processing::GetData(&data, 0, &ds);
        }
        else
        {
            Processing::GetData(0, &data, &ds);
        }
    }

    if (!ChannelNeedForDraw(data, chan, ds))
    {
        return;
    }

    float scaleY = (float)(maxY - minY) / (MAX_VALUE - MIN_VALUE);
    float scaleX = (float)Grid::Width() / 280.0f;

    if(SHOW_MEASURES)
    {
        DrawMarkersForMeasure(scaleY, chan);
    }

    int firstPoint = 0;
    int lastPoint = 0;
    sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);
    if(data == dataP2P_0 || data == dataP2P_1)
    {
        if(SET_SELFRECORDER)
        {
            LOG_TRACE
        }
        else if(lastPoint > lastP2Pdata)
        {
            lastPoint = lastP2Pdata;
        }
    }

    Painter::SetColor(ColorChannel(chan));
    if(MODE_DRAW_IS_SIGNAL_LINES)
    {
        /*
        if (set.display.numAveraging > NumAveraging_1)
        {
            Painter::SetColor(ColorGrid());
            DrawSignalLined(DS_GetData(chan, 0), ds, firstPoint, lastPoint, minY, maxY, scaleY, scaleX, calculateFiltr);    // WARN
        }
        Painter::SetColor(ColorChannel(chan));
        */
        DrawSignalLined(data, ds, firstPoint, lastPoint, minY, maxY, scaleY, scaleX, calculateFiltr);
    }
    else
    {
        DrawSignalPointed(data, ds, firstPoint, lastPoint, minY, maxY, scaleY, scaleX);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawMath()
{
    if (DISABLED_DRAW_MATH || Storage::GetData(A, 0) == 0 || Storage::GetData(B, 0) == 0)
    {
        return;
    }

    uint8 *dataRel0 = 0;
    uint8 *dataRel1 = 0;
    DataSettings *ds = 0;
    Processing::GetData(&dataRel0, &dataRel1, &ds);

    float dataAbs0[FPGA_MAX_POINTS];
    float dataAbs1[FPGA_MAX_POINTS];

    Math_PointsRelToVoltage(dataRel0, (int)ds->length1channel, ds->range[A], (int16)ds->rShiftCh0, dataAbs0);
    Math_PointsRelToVoltage(dataRel1, (int)ds->length1channel, ds->range[B], (int16)ds->rShiftCh1, dataAbs1);

    Math_CalculateMathFunction(dataAbs0, dataAbs1, (int)ds->length1channel);
    
    uint8 points[FPGA_MAX_POINTS];
    Math_PointsVoltageToRel(dataAbs0, (int)ds->length1channel, SET_RANGE_MATH, SET_RSHIFT_MATH, points);

    DrawDataChannel(points, Math, ds, Grid::MathTop(), Grid::MathBottom());

    static const int WIDTH = 71;
    static const int HEIGHT = 10;
    int delta = (SHOW_STRING_NAVIGATION && MODE_DRAW_MATH_IS_TOGETHER) ? 10 : 0;
    Painter::DrawRectangleC(Grid::Left(), Grid::MathTop() + delta, WIDTH, HEIGHT, COLOR_FILL);
    Painter::FillRegionC(Grid::Left() + 1, Grid::MathTop() + 1 + delta, WIDTH - 2, HEIGHT - 2, COLOR_BACK);
    Divider multiplier = MATH_MULTIPLIER;
    Painter::DrawTextC(Grid::Left() + 2, Grid::MathTop() + 1 + delta, sChannel_Range2String(SET_RANGE_MATH, multiplier), COLOR_FILL);
    Painter::DrawText(Grid::Left() + 25, Grid::MathTop() + 1 + delta, ":");
    char buffer[20];
    Painter::DrawText(Grid::Left() + 27, Grid::MathTop() + 1 + delta, sChannel_RShift2String(SET_RSHIFT_MATH, SET_RANGE_MATH, multiplier, buffer));


}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawSpectrumChannel(const float *spectrum, Color color)
{
    Painter::SetColor(color);
	int gridLeft = Grid::Left();
	int gridBottom = Grid::MathBottom();
	int gridHeight = Grid::MathHeight();
    for (int i = 0; i < 256; i++) 
    {
        Painter::DrawVLine(gridLeft + i, gridBottom, gridBottom - gridHeight * spectrum[i]);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::WriteParametersFFT(Channel chan, float freq0, float density0, float freq1, float density1)
{
    int x = Grid::Left() + 259;
    int y = Grid::ChannelBottom() + 5;
    int dY = 10;

    char buffer[20];
    Painter::SetColor(COLOR_FILL);
    Painter::DrawText(x, y, Freq2String(freq0, false, buffer));
    y += dY;
    Painter::DrawText(x, y, Freq2String(freq1, false, buffer));
    if (chan == A)
    {
        y += dY + 2;
    }
    else
    {
        y += dY * 3 + 4;
    }
    Painter::SetColor(ColorChannel(chan));
    Painter::DrawText(x, y, SCALE_FFT_IS_LOG ? Float2Db(density0, 4, buffer) : Float2String(density0, false, 7, buffer));
    y += dY;
    Painter::DrawText(x, y, SCALE_FFT_IS_LOG ? Float2Db(density1, 4, buffer) : Float2String(density1, false, 7, buffer));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DRAW_SPECTRUM(const uint8 *data, int numPoints, Channel channel)
{
    if (!sChannel_Enabled(channel))
    {
        return;
    }
    float dataR[FPGA_MAX_POINTS];
    float spectrum[FPGA_MAX_POINTS];

    float freq0 = 0.0f;
    float freq1 = 0.0f;
    float density0 = 0.0f;
    float density1 = 0.0f;
    int y0 = 0;
    int y1 = 0;
    int s = 2;

    Math_PointsRelToVoltage(data, numPoints, gDSet->range[channel], channel == A ? (int16)gDSet->rShiftCh0 : (int16)gDSet->rShiftCh1, dataR);
    Math_CalculateFFT(dataR, numPoints, spectrum, &freq0, &density0, &freq1, &density1, &y0, &y1);
    DrawSpectrumChannel(spectrum, ColorChannel(channel));
    if (!MenuIsShown() || MenuIsMinimize())
    {
        Color color = COLOR_FILL;
        WriteParametersFFT(channel, freq0, density0, freq1, density1);
        Painter::DrawRectangleC(FFT_POS_CURSOR_0 + Grid::Left() - s, y0 - s, s * 2, s * 2, color);
        Painter::DrawRectangle(FFT_POS_CURSOR_1 + Grid::Left() - s, y1 - s, s * 2, s * 2);

        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_0, Grid::MathBottom(), y0 + s);
        Painter::DrawVLine(Grid::Left() + FFT_POS_CURSOR_1, Grid::MathBottom(), y1 + s);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawSpectrum()
{
    if (!ENABLED_FFT)
    {
        return;
    }

    Painter::DrawVLineC(Grid::Right(), Grid::ChannelBottom() + 1, Grid::MathBottom() - 1, COLOR_BACK);

    if (MODE_WORK_IS_DIRECT)
    {
        int numPoints = sMemory_GetNumPoints(false);
        if (numPoints < 512)
        {
            numPoints = 256;
        }

        if (SOURCE_FFT_IS_A)
        {
            DRAW_SPECTRUM(gData0, numPoints, A);
        }
        else if (SOURCE_FFT_IS_B)
        {
            DRAW_SPECTRUM(gData1, numPoints, B);
        }
        else
        {
            if (LAST_AFFECTED_CHANNEL_IS_A)
            {
                DRAW_SPECTRUM(gData1, numPoints, B);
                DRAW_SPECTRUM(gData0, numPoints, A);
            }
            else
            {
                DRAW_SPECTRUM(gData0, numPoints, A);
                DRAW_SPECTRUM(gData1, numPoints, B);
            }
        }
    }

    Painter::DrawHLineC(Grid::ChannelBottom(), Grid::Left(), Grid::Right(), COLOR_FILL);
    Painter::DrawHLine(Grid::MathBottom(), Grid::Left(), Grid::Right());
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawBothChannels(uint8 *data0, uint8 *data1)
{
	if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(data0, A, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data1, B, gDSet, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(data1, B, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(data0, A, gDSet, GRID_TOP, Grid::ChannelBottom());
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawDataMemInt()
{
    if(gDSmemInt != 0)
     {
        DrawDataChannel(gData0memInt, A, gDSmemInt, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(gData1memInt, B, gDSmemInt, GRID_TOP, Grid::ChannelBottom());
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawDataInModeWorkLatest()
{
    if (gDSmemLast != 0)
    {
        DrawDataChannel(gData0memLast, A, gDSmemLast, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(gData1memLast, B, gDSmemLast, GRID_TOP, Grid::ChannelBottom());
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawDataInModePoint2Point()
{
    uint8 *data0 = 0;
    uint8 *data1 = 0;
    DataSettings *ds = 0;
    Processing::GetData(&data0, &data1, &ds);

    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        if (SET_SELFRECORDER || !Storage::NumElementsWithCurrentSettings())
        {
            DrawDataChannel(dataP2P_0, A, ds, GRID_TOP, Grid::ChannelBottom());
            DrawDataChannel(dataP2P_1, B, ds, GRID_TOP, Grid::ChannelBottom());
        }
        else
        {
            DrawDataChannel(data0, A, ds, GRID_TOP, Grid::ChannelBottom());
            DrawDataChannel(data1, B, ds, GRID_TOP, Grid::ChannelBottom());
        }
    }
    else
    {
        if (SET_SELFRECORDER || !Storage::NumElementsWithCurrentSettings())
        {
            DrawDataChannel(dataP2P_1, B, ds, GRID_TOP, Grid::ChannelBottom());
            DrawDataChannel(dataP2P_0, A, ds, GRID_TOP, Grid::ChannelBottom());
        }
        else
        {
            DrawDataChannel(data1, B, ds, GRID_TOP, Grid::ChannelBottom());
            DrawDataChannel(data0, A, ds, GRID_TOP, Grid::ChannelBottom());
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawDataInModeSelfRecorder()
{
    LOG_TRACE
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Display::DrawDataInModeNormal()
{
    static void* prevAddr = 0;
    bool retValue = true;

    uint8 *data0 = 0;
    uint8 *data1 = 0;
    DataSettings *ds = 0;
    Processing::GetData(&data0, &data1, &ds);

    int16 numSignals = (int16)Storage::NumElementsWithSameSettings();
    LIMITATION(numSignals, numSignals, 1, NUM_ACCUM);
    if (numSignals == 1 || ENUM_ACCUM_IS_INFINITY || MODE_ACCUM_IS_RESET || sTime_RandomizeModeEnabled())
    {
        DrawBothChannels(0, 0);
        if (prevAddr == 0 || prevAddr != ds->addrPrev)
        {
            NUM_DRAWING_SIGNALS++;
            prevAddr = ds->addrPrev;
        }
    }
    else
    {
        for (int i = 0; i < numSignals; i++)
        {
            DrawBothChannels(Storage::GetData(A, i), Storage::GetData(B, i));
        }
    }

    return retValue;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawDataMinMax()
{
    ModeDrawSignal modeDrawSignalOld = MODE_DRAW_SIGNAL;
    MODE_DRAW_SIGNAL = ModeDrawSignal_Lines;
    if (LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawDataChannel(Storage::GetLimitation(A, 0), A, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(A, 1), A, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(B, 0), B, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(B, 1), B, gDSet, GRID_TOP, Grid::ChannelBottom());
    }
    else
    {
        DrawDataChannel(Storage::GetLimitation(B, 0), B, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(B, 1), B, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(A, 0), A, gDSet, GRID_TOP, Grid::ChannelBottom());
        DrawDataChannel(Storage::GetLimitation(A, 1), A, gDSet, GRID_TOP, Grid::ChannelBottom());
    }
    MODE_DRAW_SIGNAL = modeDrawSignalOld;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Display::DrawDataNormal()
{
    bool retValue = true;
    if (!dataP2PIsEmpty)
    {
        static const pFuncVV funcs[2] = {DrawDataInModePoint2Point, DrawDataInModeSelfRecorder};
        funcs[(int)SET_SELFRECORDER]();
    }
    else
    {
        retValue = DrawDataInModeNormal();
    }
    return retValue;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Display::DrawData()
{
    bool retValue = true;

    if (Storage::AllDatas())
    {

        if (MODE_WORK_IS_MEMINT)
        {
            if (!MODE_SHOW_MEMINT_IS_DIRECT)
            {
                DrawDataMemInt();
            }
            if (!MODE_SHOW_MEMINT_IS_SAVED)
            {
                DrawDataNormal();
            }
        }
        else if (MODE_WORK_IS_LATEST)
        {
            DrawDataInModeWorkLatest();
        }
        else
        {
            if (INT_SHOW_ALWAYS)
            {
                DrawDataMemInt();
            }
            retValue = DrawDataNormal();
        }

        if (NUM_MIN_MAX != 1)
        {
            DrawDataMinMax();
        }
    }

    Painter::DrawRectangleC(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight(), COLOR_FILL);

    return retValue;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawTime(int x, int y)
{
    int dField = 10;
    int dSeparator = 2;

    PackedTime time = Clock::GetPackedTime();
    
    char buffer[20];
    
    Painter::SetColor(COLOR_FILL);
    
    if (MODE_WORK_IS_MEMINT || MODE_WORK_IS_LATEST)
    {
        DataSettings *ds = MODE_WORK_IS_MEMINT ? gDSmemInt : gDSmemLast;

        if (ds != 0)
        {
            y -= 9;
            time.day = ds->time.day;
            time.hours = ds->time.hours;
            time.minutes = ds->time.minutes;
            time.seconds = ds->time.seconds;
            time.month = ds->time.month;
            time.year = ds->time.year;
            Painter::DrawText(x, y, Int2String((int)time.day, false, 2, buffer));
            Painter::DrawText(x + dField, y, ":");
            Painter::DrawText(x + dField + dSeparator, y, Int2String((int)time.month, false, 2, buffer));
            Painter::DrawText(x + 2 * dField + dSeparator, y, ":");
            Painter::DrawText(x + 2 * dField + 2 * dSeparator, y, Int2String((int)time.year + 2000, false, 4, buffer));
            y += 9;
        }
        else
        {
            return;
        }
    }
    
    
    Painter::DrawText(x, y, Int2String((int)time.hours, false, 2, buffer));
    Painter::DrawText(x + dField, y, ":");
    Painter::DrawText(x + dField + dSeparator, y, Int2String((int)time.minutes, false, 2, buffer));
    Painter::DrawText(x + 2 * dField + dSeparator, y, ":");
    Painter::DrawText(x + 2 * dField + 2 * dSeparator, y, Int2String((int)time.seconds, false, 2, buffer));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawHiPart()
{
    WriteCursors();
    DrawHiRightPart();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена иниформация о максимумах.
void Display::DrawDataInRect(int x, int width, const uint8 *data, int numElems, Channel chan, int shiftForPeakDet)
{
    if(numElems == 0)
    {
        return;
    }

    width--;
    float elemsInColumn = (float)numElems / (float)width;
    uint8 min[300];
    uint8 max[300];


    if (SET_TBASE >= TBase_20ms && PEAKDET)
    {
        for (int col = 0; col < width; col++)
        {
            float firstElem = col * elemsInColumn;
            float lastElem = firstElem + elemsInColumn - 1;
            min[col] = data[(int)firstElem];
            max[col] = data[(int)firstElem + 1];
            for (int elem = firstElem + 2; elem <= lastElem; elem += 2)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem + 1], max[col]);
            }
        }
    }
    else if (shiftForPeakDet == 0)
    {
        uint8 *iMin = &min[0];
        uint8 *iMax = &max[0];

        for (int col = 0; col < width; col++, iMin++, iMax++)
        {
            int firstElem = col * elemsInColumn;
            int lastElem = firstElem + elemsInColumn - 1;
            *iMin = data[firstElem];
            *iMax = data[firstElem];
            for (int elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], *iMin);
                SET_MAX_IF_LARGER(data[elem], *iMax);
            }
        }
    }
    else        // А здесь будет, если пик. дет. включен
    {
        for (int col = 0; col < width; col++)
        {
            float firstElem = col * elemsInColumn;
            float lastElem = firstElem + elemsInColumn - 1;
            min[col] = data[(int)firstElem];
            max[col] = data[(int)firstElem + shiftForPeakDet];
            for (int elem = firstElem + 1; elem <= lastElem; elem++)
            {
                SET_MIN_IF_LESS(data[elem], min[col]);
                SET_MAX_IF_LARGER(data[elem + shiftForPeakDet], max[col]);
            }
        }
    }

    int bottom = 16;
    int height = 14;
    float scale = (float)height / (float)(MAX_VALUE - MIN_VALUE);

#define ORDINATE(x) bottom - scale * LimitationInt(x - MIN_VALUE, 0, 200)

#define NUM_POINTS (300 * 2)
    uint8 points[NUM_POINTS];

    points[0] = ORDINATE(max[0]);
    points[1] = ORDINATE(min[0]);



    for(int i = 1; i < width; i++)
    {
        int value0 = min[i] > max[i - 1] ? max[i - 1] : min[i];
        int value1 = max[i] < min[i - 1] ? min[i - 1] : max[i];
        points[i * 2] = ORDINATE(value1);
        points[i * 2 + 1] = ORDINATE(value0);
    }
	if(width < 256)
    {
		Painter::DrawVLineArray(x, width, points, ColorChannel(chan));
	}
    else
    {
		Painter::DrawVLineArray(x, 255, points, ColorChannel(chan));
		Painter::DrawVLineArray(x + 255, width - 255, points + 255 * 2, ColorChannel(chan));
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
// shiftForPeakDet - если рисуем информацию с пикового детектора - то через shiftForPeakDet точек расположена иниформация о максимумах.
void Display::DrawChannelInWindowMemory(int timeWindowRectWidth, int xVert0, int xVert1, int startI, int endI, const uint8 *data, int rightX, Channel chan, int shiftForPeakDet)
{
    if(data == dataP2P_0 && data == dataP2P_1)
    {

    }
    else
    {
        DrawDataInRect(1,          xVert0 - 1,              &(data[0]),        startI,                             chan, shiftForPeakDet);
        DrawDataInRect(xVert0 + 2, timeWindowRectWidth - 2, &(data[startI]),   281,                                chan, shiftForPeakDet);
        DrawDataInRect(xVert1 + 2, rightX - xVert1 + 2,     &(data[endI + 1]), sMemory_GetNumPoints(false) - endI, chan, shiftForPeakDet);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawMemoryWindow()
{
    uint8 *dat0 = gData0memInt;
    uint8 *dat1 = gData1memInt;
    DataSettings *ds = gDSmemInt;
    
    if(MODE_WORK_IS_DIRECT || MODE_WORK_IS_LATEST)
    {
        dat0 = gData0;
        dat1 = gData1;
        ds = gDSet;
    }
    
    int leftX = 3;
    int top = 1;
    int height = GRID_TOP - 3;
    int bottom = top + height;

    static const int rightXses[3] = {276, 285, 247};
    int rightX = rightXses[MODE_WORK];
    if (sCursors_NecessaryDrawCursors())
    {
        rightX = 68;
    }

    int timeWindowRectWidth = (rightX - leftX) * (282.0f / sMemory_GetNumPoints(false));
    float scaleX = (float)(rightX - leftX + 1) / sMemory_GetNumPoints(false);

    int16 shiftInMemory = SHIFT_IN_MEMORY;
    
    int startI = shiftInMemory;
    int endI = startI + 281;

    const int xVert0 = leftX + shiftInMemory * scaleX;
    const int xVert1 = leftX + shiftInMemory * scaleX + timeWindowRectWidth;
    bool showFull = set.display.showFullMemoryWindow;
    Painter::DrawRectangleC(xVert0, top + (showFull ? 0 : 1), xVert1 - xVert0, bottom - top - (showFull ? 0 : 2), COLOR_FILL);

    if(!dataP2PIsEmpty)
    {
        dat0 = dataP2P_0;
        dat1 = dataP2P_1;
    }

    if (showFull)
    {
        if (gData0 || gData1 || (!dataP2PIsEmpty))
        {
            Channel chanFirst = LAST_AFFECTED_CHANNEL_IS_A ? B : A;
            Channel chanSecond = LAST_AFFECTED_CHANNEL_IS_A ? A : B;
            const uint8 *dataFirst = LAST_AFFECTED_CHANNEL_IS_A ? dat1 : dat0;
            const uint8 *dataSecond = LAST_AFFECTED_CHANNEL_IS_A ? dat0 : dat1;

            int shiftForPeakDet = ds->peakDet == PeackDet_Disable ? 0 : (int)ds->length1channel;

            if (ChannelNeedForDraw(dataFirst, chanFirst, ds))
            {
                DrawChannelInWindowMemory(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataFirst, rightX, chanFirst, shiftForPeakDet);
            }
            if (ChannelNeedForDraw(dataSecond, chanSecond, ds))
            {
                DrawChannelInWindowMemory(timeWindowRectWidth, xVert0, xVert1, startI, endI, dataSecond, rightX, chanSecond, shiftForPeakDet);
            }
        }
    }
    else
    {
        Painter::DrawVLineC(leftX - 2, top, bottom, COLOR_FILL);
        Painter::DrawVLine(rightX + 2, top, bottom);
        Painter::DrawHLine((bottom + top) / 2 - 3, leftX, xVert0 - 2);
        Painter::DrawHLine((bottom + top) / 2 + 3, leftX, xVert0 - 2);
        Painter::DrawHLine((bottom + top) / 2 + 3, xVert1 + 2, rightX);
        Painter::DrawHLine((bottom + top) / 2 - 3, xVert1 + 2, rightX);
    }

    int x[] = {leftX, (rightX - leftX) / 2 + leftX + 1, rightX};
    int x0 = x[SET_TPOS];

    // Маркер TPos
    Painter::FillRegionC(x0 - 3, 9, 6, 6, COLOR_BACK);
    Painter::DrawCharC(x0 - 3, 9, SYMBOL_TPOS_1, COLOR_FILL);

    // Маркер tShift
    float scale = (float)(rightX - leftX + 1) / ((float)sMemory_GetNumPoints(false) - (sMemory_GetNumPoints(false) == 281 ? 1 : 0));
    float xShift = 1 + (sTime_TPosInPoints((PeackDetMode)gDSet->peakDet, (int)gDSet->length1channel, SET_TPOS) - sTime_TShiftInPoints((PeackDetMode)gDSet->peakDet)) * scale;
    
    if(xShift < leftX - 2)
    {
        xShift = leftX - 2;
    }

    Painter::FillRegionC(xShift - 1, 3, 6, 6, COLOR_BACK);
    Painter::FillRegionC(xShift, 4, 4, 4, COLOR_FILL);
    Painter::SetColor(COLOR_BACK);

    if(xShift == leftX - 2)
    {
        xShift = leftX - 2;
        Painter::DrawLine(xShift + 3, 5, xShift + 3, 7);
        Painter::DrawLine(xShift + 1, 6, xShift + 2, 6);
    }
    else if(xShift > rightX - 1)
    {
        xShift = rightX - 2;
        Painter::DrawLine(xShift + 1, 5, xShift + 1, 7);
        Painter::DrawLine(xShift + 2, 6, xShift + 3, 6);
    }
    else
    {
        Painter::DrawLine(xShift + 1, 5, xShift + 3, 5);
        Painter::DrawLine(xShift + 2, 6, xShift + 2, 7);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::WriteCursors()
{
    char buffer[20];
    int startX = 43;
    if(MODE_WORK_IS_DIRECT)
    {
        startX += 29;
    }
    int x = startX;
    int y1 = 0;
    int y2 = 9;
    if(sCursors_NecessaryDrawCursors())
    {
        Painter::DrawVLineC(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;
        Channel source = CURS_SOURCE;
        Color colorText = ColorChannel(source);
        if(!CURS_CNTRL_U_IS_DISABLE(source))
        {
            Painter::DrawTextC(x, y1, "1:", colorText);
            Painter::DrawText(x, y2, "2:");
            x += 7;
            Painter::DrawText(x, y1, sCursors_GetCursVoltage(source, 0, buffer));
            Painter::DrawText(x, y2, sCursors_GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = Math_VoltageCursor(sCursors_GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = Math_VoltageCursor(sCursors_GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = fabs(pos1 - pos0);
            Painter::DrawText(x, y1, ":dU=");
            Painter::DrawText(x + 17, y1, Voltage2String(delta, false, buffer));
            Painter::DrawText(x, y2, ":");
            Painter::DrawText(x + 10, y2, sCursors_GetCursorPercentsU(source, buffer));
        }

        x = startX + 101;
        Painter::DrawVLineC(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;
        if(!CURS_CNTRL_T_IS_DISABLE(source))
        {
            Painter::SetColor(colorText);
            Painter::DrawText(x, y1, "1:");
            Painter::DrawText(x, y2, "2:");
            x+=7;
            Painter::DrawText(x, y1, sCursors_GetCursorTime(source, 0, buffer));
            Painter::DrawText(x, y2, sCursors_GetCursorTime(source, 1, buffer));
            x = startX + 153;
            float pos0 = Math_TimeCursor(CURS_POS_T0(source), SET_TBASE);
            float pos1 = Math_TimeCursor(CURS_POS_T1(source), SET_TBASE);
            float delta = fabs(pos1 - pos0);
            Painter::DrawText(x, y1, ":dT=");
            char buf[20];
            Painter::DrawText(x + 17, y1, Time2String(delta, false, buf));
            Painter::DrawText(x, y2, ":");
            Painter::DrawText(x + 8, y2, sCursors_GetCursorPercentsT(source, buf));

            if(CURSORS_SHOW_FREQ)
            {
                int width = 65;
                int x0 = Grid::Right() - width;
                Painter::DrawRectangleC(x0, GRID_TOP, width, 12, COLOR_FILL);
                Painter::FillRegionC(x0 + 1, GRID_TOP + 1, width - 2, 10, COLOR_BACK);
                Painter::DrawTextC(x0 + 1, GRID_TOP + 2, "1/dT=", colorText);
                char buff[20];
                Painter::DrawText(x0 + 25, GRID_TOP + 2, Freq2String(1.0f / delta, false, buff));
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawHiRightPart()
    {
    // Синхроимпульс
    int y = 2;

    static const int xses[3] = {280, 271, 251};
    int x = xses[MODE_WORK];

    if (!MODE_WORK_IS_LATEST)
    {
        Painter::DrawVLineC(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;

        if (TRIG_ENABLE)
        {
            Painter::FillRegion(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7);
            Painter::DrawTextC(x + 3, 3 + y, set.common.lang == Russian ? "СИ" : "Tr", COLOR_BACK);
        }
    }

    // Режим работы
    static const char *strings_[][2] =
    {
        {"ИЗМ",     "MEAS"},
        {"ПОСЛ",    "LAST"},
        {"ВНТР",    "INT"}
    };

    if(!MODE_WORK_IS_DIRECT)
    {
        x += 18;
        Painter::DrawVLineC(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 2;
        Painter::DrawText(set.common.lang == Russian ? x : x + 3, -1, set.common.lang == Russian ? "режим" : "mode");
        Painter::DrawStringInCenterRect(x + 1, 9, 25, 8, strings_[MODE_WORK][set.common.lang]);
    }
    else
    {
        x -= 9;
    }

    if (!MODE_WORK_IS_LATEST)
    {

        x += 27;
        Painter::DrawVLineC(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;
        y = 1;
        if (FPGA::CurrentStateWork() == StateWorkFPGA_Work)
        {
            Painter::Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if (FPGA::CurrentStateWork() == StateWorkFPGA_Stop)
        {
            Painter::FillRegion(x + 3, y + 3, 10, 10);
        }
        else if (FPGA::CurrentStateWork() == StateWorkFPGA_Wait)
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Painter::FillRegion(x, y + 1, w, h);
            Painter::FillRegion(x + w + delta, y + 1, w, h);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursorsRShift()
{
    if (!DISABLED_DRAW_MATH)
    {
        DrawCursorRShift(Math);
    }
    if(LAST_AFFECTED_CHANNEL_IS_B)
    {
        DrawCursorRShift(A);
        DrawCursorRShift(B);
    }
    else
    {
        DrawCursorRShift(B);
        DrawCursorRShift(A);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Display::NeedForClearScreen()
{
    int numAccum = NUM_ACCUM;
    if (sTime_RandomizeModeEnabled() || numAccum == 1 || MODE_ACCUM_IS_NORESET || SET_SELFRECORDER)
    {
        return true;
    }
    if (NEED_FINISH_REDRAW)
    {
        NEED_FINISH_REDRAW = 0;
        return true;
    }
    if (MODE_ACCUM_IS_RESET && NUM_DRAWING_SIGNALS >= numAccum)
    {
        NUM_DRAWING_SIGNALS = 0;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::Update(bool endScene)
{
	uint timeStart = gTimerTics;
    if (funcOnHand != 0)
    {
        funcOnHand();
        return;
    }

    bool needClear = NeedForClearScreen();

    if (needClear)
    {
        Painter::BeginScene(COLOR_BACK);
        DrawMemoryWindow();
        DrawFullGrid();
    }

    DrawData();

    if (needClear)
    {
        DrawMath();
        DrawSpectrum();
        DrawCursors();
        DrawHiPart();
        DrawLowPart();
        DrawCursorsWindow();
        DrawCursorTrigLevel();
        DrawCursorsRShift();
        DrawMeasures();
        DrawStringNavigation();
        DrawCursorTShift();
    }
    
    Menu::Draw();

    if (needClear)
    {
        DrawWarnings();

        if (funcAdditionDraw)
        {
            funcAdditionDraw();
        }
    }

    DrawConsole();

    if (needClear)    
    {
        WriteValueTrigLevel();
    }

    DrawTimeForFrame(gTimerTics - timeStart);

    Painter::SetColor(COLOR_FILL);

    Painter::EndScene(endScene);

    if (NEED_SAVE_TO_DRIVE)
    {
        if (Painter::SaveScreenToFlashDrive())
        {
            Display::ShowWarningGood(FileIsSaved);
        }
        NEED_SAVE_TO_DRIVE = 0;
    }

    if (funcAfterDraw)
    {
        funcAfterDraw();
        funcAfterDraw = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::WriteValueTrigLevel()
{
    if (SHOW_LEVEL_TRIGLEV && MODE_WORK_IS_DIRECT)
    {
        float trigLev = RSHIFT_2_ABS(TRIG_LEVEL_SOURCE, SET_RANGE(TRIG_SOURCE));     // WARN Здесь для внешней синхронизации неправильно рассчитывается уровень.
        TrigSource trigSource = TRIG_SOURCE;
        if (TRIG_INPUT_IS_AC && trigSource <= TrigSource_ChannelB)
        {
            int16 rShift = SET_RSHIFT(trigSource);
            float rShiftAbs = RSHIFT_2_ABS(rShift, SET_RANGE(trigSource));
            trigLev += rShiftAbs;
        }
        char buffer[20];
        strcpy(buffer, LANG_RU ? "Ур синхр = " : "Trig lvl = ");
        char bufForVolt[20];
        strcat(buffer, Voltage2String(trigLev, true, bufForVolt));
        int width = 96;
        int x = (Grid::Width() - width) / 2 + Grid::Left();
        int y = Grid::BottomMessages() - 20;
        Painter::DrawRectangleC(x, y, width, 10, COLOR_FILL);
        Painter::FillRegionC(x + 1, y + 1, width - 2, 8, COLOR_BACK);
        Painter::DrawTextC(x + 2, y + 1, buffer, COLOR_FILL);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawGridSpectrum()
{
    if (SCALE_FFT_IS_LOG)
    {
        static const int nums[] = {4, 6, 8};
        static const char *strs[] = {"0", "-10", "-20", "-30", "-40", "-50", "-60", "-70"};
        int numParts = nums[FFT_MAX_DB];
        float scale = (float)Grid::MathHeight() / numParts;
        for (int i = 1; i < numParts; i++)
        {
            int y = Grid::MathTop() + i * scale;
            Painter::DrawHLineC(y, Grid::Left(), Grid::Left() + 256, ColorGrid());
            if (!MenuIsMinimize())
            {
                Painter::SetColor(COLOR_FILL);
                Painter::DrawText(3, y - 4, strs[i]);
            }
        }
        if (!MenuIsMinimize())
        {
            Painter::SetColor(COLOR_FILL);
            Painter::DrawText(5, Grid::MathTop() + 1, "дБ");
        }
    }
    else if (SCALE_FFT_IS_LINEAR)
    {
        static const char *strs[] = {"1.0", "0.8", "0.6", "0.4", "0.2"};
        float scale = (float)Grid::MathHeight() / 5;
        for (int i = 1; i < 5; i++)
        {
            int y = Grid::MathTop() + i * scale;
            Painter::DrawHLineC(y, Grid::Left(), Grid::Left() + 256, ColorGrid());
            if (!MenuIsMinimize())
            {
                Painter::DrawTextC(5, y - 4, strs[i], COLOR_FILL);
            }
        }
    }
    Painter::DrawVLineC(Grid::Left() + 256, Grid::MathTop(), Grid::MathBottom(), COLOR_FILL);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawFullGrid()
{
    if (sDisplay_IsSeparate())
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight() / 2);
        if (ENABLED_FFT)
        {
            DrawGridSpectrum();
        }
        if (!DISABLED_DRAW_MATH)
        {
            DrawGrid(Grid::Left(), GRID_TOP + Grid::FullHeight() / 2, Grid::Width(), Grid::FullHeight() / 2);
        }
        Painter::DrawHLineC(GRID_TOP + Grid::FullHeight() / 2, Grid::Left(), Grid::Left() + Grid::Width(), COLOR_FILL);
    }
    else
    {
        DrawGrid(Grid::Left(), GRID_TOP, Grid::Width(), Grid::FullHeight());
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int Display::CalculateCountV()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_1_5)
        {
            return MEAS_SOURCE_IS_A_B ? 42 : 44;
        }
        if (MEAS_NUM_IS_2_5)
        {
            return MEAS_SOURCE_IS_A_B ? 69 : 39;
        }
        if (MEAS_NUM_IS_3_5)
        {
            return MEAS_SOURCE_IS_A_B ? 54 : 68;
        }
    }

    return 49;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int Display::CalculateCountH()
{
    if (MODE_VIEW_SIGNALS_IS_COMPRESS)
    {
        if (MEAS_NUM_IS_6_1)
        {
            return 73;
        }
        if (MEAS_NUM_IS_6_2)
        {
            return 83;
        }
    }
    return 69;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawGridType1(int left, int top, int right, int bottom, float centerX, float centerY, float deltaX, float deltaY, float stepX, float stepY)
{
    uint16 masX[17];
    masX[0] = (uint16)(left + 1);
    for (int i = 1; i < 7; i++)
    {
        masX[i] = left + deltaX * i;
    }
    for (int i = 7; i < 10; i++)
    {
        masX[i] = centerX - 8 + i;
    }
    for (int i = 10; i < 16; i++)
    {
        masX[i] = centerX + deltaX * (i - 9);
    }
    masX[16] = (uint16)(right - 1);

    Painter::DrawMultiVPointLine(17, top + stepY, masX, stepY, CalculateCountV(), ColorGrid());

    uint8 mas[13];
    mas[0] = (uint8)(top + 1);
    for (int i = 1; i < 5; i++)
    {
        mas[i] = top + deltaY * i;
    }
    for (int i = 5; i < 8; i++)
    {
        mas[i] = centerY - 6 + i;
    }
    for (int i = 8; i < 12; i++)
    {
        mas[i] = centerY + deltaY * (i - 7);
    }
    mas[12] = (uint8)(bottom - 1);

    Painter::DrawMultiHPointLine(13, left + stepX, mas, stepX, CalculateCountH(), ColorGrid());
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawGridType2(int left, int top, int right, int bottom, int deltaX, int deltaY, int stepX, int stepY)
{ 
    uint16 masX[15];
    masX[0] = (uint16)(left + 1);
    for (int i = 1; i < 14; i++)
    {
        masX[i] = (uint16)(left + deltaX * i);
    }
    masX[14] = (uint16)(right - 1);
    Painter::DrawMultiVPointLine(15, top + stepY, masX, stepY, CalculateCountV(), ColorGrid());

    uint8 mas[11];
    mas[0] = (uint8)(top + 1);
    for (int i = 1; i < 10; i++)
    {
        mas[i] = (uint8)(top + deltaY * i);
    }
    mas[10] = (uint8)(bottom - 1);
    Painter::DrawMultiHPointLine(11, left + stepX, mas, stepX, CalculateCountH(), ColorGrid());
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawGridType3(int left, int top, int right, int bottom, int centerX, int centerY, int deltaX, int deltaY, int stepX, int stepY)
{
    Painter::DrawHPointLine(centerY, left + stepX, right, stepX);
    uint8 masY[6] = {(uint8)(top + 1), (uint8)(top + 2), (uint8)(centerY - 1), (uint8)(centerY + 1), (uint8)(bottom - 2), (uint8)(bottom - 1)};
    Painter::DrawMultiHPointLine(6, left + deltaX, masY, deltaX, (right - top) / deltaX, ColorGrid());
    Painter::DrawVPointLine(centerX, top + stepY, bottom, stepY, ColorGrid());
    uint16 masX[6] = {(uint16)(left + 1), (uint16)(left + 2), (uint16)(centerX - 1), (uint16)(centerX + 1), (uint16)(right - 2), (uint16)(right - 1)};
    Painter::DrawMultiVPointLine(6, top + deltaY, masX, deltaY, (bottom - top) / deltaY, ColorGrid());

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawGrid(int left, int top, int width, int height)
{
    int right = left + width;
    int bottom = top + height;

    Painter::SetColor(COLOR_FILL);

    if (top == GRID_TOP)
    {
        Painter::DrawHLine(top, 1, left - 2);
        Painter::DrawHLine(top, right + 2, SCREEN_WIDTH - 2);

        if (!MenuIsMinimize() || !MenuIsShown())
        {
            Painter::DrawVLine(1, top + 2, bottom - 2);
            Painter::DrawVLine(318, top + 2, bottom - 2);
        }
    }

    float deltaX = Grid::DeltaX() *(float)width / width;
    float deltaY = Grid::DeltaY() * (float)height / height;
    float stepX = deltaX / 5;
    float stepY = deltaY / 5;
    
    int centerX = left + width / 2;
    int centerY = top + height / 2;

    Painter::SetColor(ColorGrid());
    if (TYPE_GRID_IS_1)
    {
        DrawGridType1(left, top, right, bottom, centerX, centerY, deltaX, deltaY, stepX, stepY);
    }
    else if (TYPE_GRID_IS_2)
    {
        DrawGridType2(left, top, right, bottom, deltaX, deltaY, stepX, stepY);
    }
    else if (TYPE_GRID_IS_3)
    {
        DrawGridType3(left, top, right, bottom, centerX, centerY, deltaX, deltaY, stepX, stepY);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define  DELTA 5

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawScaleLine(int x, bool forTrigLev)
{
    if(ALT_MARKERS_HIDE)
    {
        return;
    }
    int width = 6;
    int topY = GRID_TOP + DELTA;
    int x2 = width + x + 2;
    int bottomY  = Grid::ChannelBottom() - DELTA;
    int centerY = (Grid::ChannelBottom() + GRID_TOP) / 2;
    int levels[] =
    {
        topY,
        bottomY,
        centerY,
        centerY - (bottomY - topY) / (forTrigLev ? 8 : 4),
        centerY + (bottomY - topY) / (forTrigLev ? 8 : 4)
    };
    for(int i = 0; i < 5; i++)
    {
        Painter::DrawLineC(x + 1, levels[i], x2 - 1, levels[i], COLOR_FILL);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursorsWindow()
{
    if((!MenuIsMinimize() || !MenuIsShown()) && DRAW_RSHIFT_MARKERS)
    {
        DrawScaleLine(2, false);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursorTrigLevel()
{
    TrigSource chan = TRIG_SOURCE;
    if (chan == TrigSource_Ext)
    {
        return;
    }
    int trigLev = TRIG_LEVEL(chan) + ((chan == TrigSource_Ext) ? 0 : SET_RSHIFT(chan) - RShiftZero);
    float scale = 1.0f / ((TrigLevMax - TrigLevMin) / 2 / Grid::ChannelHeight());
    int y0 = (GRID_TOP + Grid::ChannelBottom()) / 2 + scale * (TrigLevZero - TrigLevMin);
    int y = y0 - scale * (trigLev - TrigLevMin);

    if(chan != TrigSource_Ext)
    {
        y = (y - Grid::ChannelCenterHeight()) + Grid::ChannelCenterHeight();
    }

    int x = Grid::Right();
    Painter::SetColor(ColorTrig());
    if(y > Grid::ChannelBottom())
    {
        Painter::DrawChar(x + 3, Grid::ChannelBottom() - 11, SYMBOL_TRIG_LEV_LOWER);
        Painter::SetPoint(x + 5, Grid::ChannelBottom() - 2);
        y = Grid::ChannelBottom() - 7;
        x--;
    }
    else if(y < GRID_TOP)
    {
        Painter::DrawChar(x + 3, GRID_TOP + 2, SYMBOL_TRIG_LEV_ABOVE);
        Painter::SetPoint(x + 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x--;
    }
    else
    {
        Painter::DrawChar(x + 1, y - 4, SYMBOL_TRIG_LEV_NORMAL);
    }
    Painter::SetFont(TypeFont_5);

    const char simbols[3] = {'1', '2', 'В'};
    int dY = 0;
    
    Painter::DrawCharC(x + 5, y - 9 + dY, simbols[TRIG_SOURCE], COLOR_BACK);
    Painter::SetFont(TypeFont_8);

    if (DRAW_RSHIFT_MARKERS && !MenuIsMinimize())
    {
        DrawScaleLine(SCREEN_WIDTH - 11, true);
        int left = Grid::Right() + 9;
        int height = Grid::ChannelHeight() - 2 * DELTA;
        int shiftFullMin = RShiftMin + TrigLevMin;
        int shiftFullMax = RShiftMax + TrigLevMax;
        scale = (float)height / (shiftFullMax - shiftFullMin);
        int shiftFull = TRIG_LEVEL_SOURCE + (TRIG_SOURCE_IS_EXT ? 0 : SET_RSHIFT(chan));
        int yFull = GRID_TOP + DELTA + height - scale * (shiftFull - RShiftMin - TrigLevMin) - 4;
        Painter::FillRegionC(left + 2, yFull + 1, 4, 6, ColorTrig());
        Painter::SetFont(TypeFont_5);
        Painter::DrawCharC(left + 3, yFull - 5 + dY, simbols[TRIG_SOURCE], COLOR_BACK);
        Painter::SetFont(TypeFont_8);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursorRShift(Channel chan)
{
    float x = Grid::Right() - Grid::Width() - Measure_GetDeltaGridLeft();

    if (chan == Math)
    {
        int rShift = SET_RSHIFT_MATH;
        float scale = (float)Grid::MathHeight() / 960;
        float y = (Grid::MathTop() + Grid::MathBottom()) / 2 - scale * (rShift - RShiftZero);
        Painter::DrawCharC(x - 9, y - 4, SYMBOL_RSHIFT_NORMAL, COLOR_FILL);
        Painter::DrawCharC(x - 8, y - 5, 'm', COLOR_BACK);
        return;
    }
    if(!sChannel_Enabled(chan))
    {
        return;
    }

    int rShift = SET_RSHIFT(chan);
 
    float scale = (float)Grid::ChannelHeight() / (STEP_RSHIFT * 200);
    float y = Grid::ChannelCenterHeight() - scale * (rShift - RShiftZero);

    float scaleFull = (float)Grid::ChannelHeight() / (RShiftMax - RShiftMin) * (sService_MathEnabled() ? 0.9f : 0.91f);
    float yFull = Grid::ChannelCenterHeight() - scaleFull *(rShift - RShiftZero);

    if(y > Grid::ChannelBottom())
    {
        Painter::DrawCharC(x - 7, Grid::ChannelBottom() - 11, SYMBOL_RSHIFT_LOWER, ColorChannel(chan));
        Painter::SetPoint(x - 5, Grid::ChannelBottom() - 2);
        y = Grid::ChannelBottom() - 7;
        x++;
    }
    else if(y < GRID_TOP)
    {
        Painter::DrawCharC(x - 7, GRID_TOP + 2, SYMBOL_RSHIFT_ABOVE, ColorChannel(chan));
        Painter::SetPoint(x - 5, GRID_TOP + 2);
        y = GRID_TOP + 7;
        x++;
    }
    else
    {
        Painter::DrawCharC(x - 8, y - 4, SYMBOL_RSHIFT_NORMAL, ColorChannel(chan));
        if(((chan == A) ? (SHOW_LEVEL_RSHIFT_0 == 1) : (SHOW_LEVEL_RSHIFT_1 == 1)) && MODE_WORK_IS_DIRECT)
        {
            Painter::DrawDashedHLine(y, Grid::Left(), Grid::Right(), 7, 3, 0);
        }
    }

    Painter::SetFont(TypeFont_5);
    int dY = 0;

    if((!MenuIsMinimize() || !MenuIsShown()) && DRAW_RSHIFT_MARKERS)
    {
        Painter::FillRegionC(4, yFull - 3, 4, 6, ColorChannel(chan));
        Painter::DrawCharC(5, yFull - 9 + dY, chan == A ? '1' : '2', COLOR_BACK);
    }
    Painter::DrawCharC(x - 7, y - 9 + dY, chan == A ? '1' : '2', COLOR_BACK);
    Painter::SetFont(TypeFont_8);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursorTShift()
{
    int firstPoint = 0;
    int lastPoint = 0;
    sDisplay_PointsOnDisplay(&firstPoint, &lastPoint);

    // Рисуем TPos
    int shiftTPos = sTime_TPosInPoints((PeackDetMode)gDSet->peakDet, (int)gDSet->length1channel, SET_TPOS) - SHIFT_IN_MEMORY;
    float scale = (lastPoint - firstPoint) / Grid::Width();
    int gridLeft = Grid::Left();
    int x = gridLeft + shiftTPos * scale - 3;
    if (IntInRange(x + 3, gridLeft, Grid::Right() + 1))
    {
        Painter::Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TPOS_2, SYMBOL_TPOS_3, COLOR_BACK, COLOR_FILL);
    };

    // Рисуем tShift
    int shiftTShift = sTime_TPosInPoints((PeackDetMode)gDSet->peakDet, (int)gDSet->length1channel, SET_TPOS) - sTime_TShiftInPoints((PeackDetMode)gDSet->peakDet);
    if(IntInRange(shiftTShift, firstPoint, lastPoint))
    {
        x = gridLeft + shiftTShift - firstPoint - 3;
        Painter::Draw2SymbolsC(x, GRID_TOP - 1, SYMBOL_TSHIFT_NORM_1, SYMBOL_TSHIFT_NORM_2, COLOR_BACK, COLOR_FILL);
    }
    else if(shiftTShift < firstPoint)
    {
        Painter::Draw2SymbolsC(gridLeft + 1, GRID_TOP, SYMBOL_TSHIFT_LEFT_1, SYMBOL_TSHIFT_LEFT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLineC(Grid::Left() + 9, GRID_TOP + 1, Grid::Left() + 9, GRID_TOP + 7, COLOR_BACK);
    }
    else if(shiftTShift > lastPoint)
    {
        Painter::Draw2SymbolsC(Grid::Right() - 8, GRID_TOP, SYMBOL_TSHIFT_RIGHT_1, SYMBOL_TSHIFT_RIGHT_2, COLOR_BACK, COLOR_FILL);
        Painter::DrawLineC(Grid::Right() - 9, GRID_TOP + 1, Grid::Right() - 9, GRID_TOP + 7, COLOR_BACK);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawHorizontalCursor(int y, int xTearing)
{
    y += GRID_TOP;
    if(xTearing == -1)
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, Grid::Right() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedHLine(y, Grid::Left() + 2, xTearing - 2, 1, 1, 0);
        Painter::DrawDashedHLine(y, xTearing + 2, Grid::Right() - 1, 1, 1, 0);
    }
    Painter::DrawRectangle(Grid::Left() - 1, y - 1, 2, 2);
    Painter::DrawRectangle(Grid::Right() - 1, y - 1, 2, 2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawVerticalCursor(int x, int yTearing)
{
    x += Grid::Left();
    if(yTearing == -1)
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }
    else
    {
        Painter::DrawDashedVLine(x, GRID_TOP + 2, yTearing - 2, 1, 1, 0);
        Painter::DrawDashedVLine(x, yTearing + 2, Grid::ChannelBottom() - 1, 1, 1, 0);
    }
    Painter::DrawRectangle(x - 1, GRID_TOP - 1, 2, 2);
    Painter::DrawRectangle(x - 1, Grid::ChannelBottom() - 1, 2, 2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawCursors()
{
    Channel source = CURS_SOURCE;
    Painter::SetColor(ColorCursors(source));
    if (sCursors_NecessaryDrawCursors())
    {
        bool bothCursors = !CURS_CNTRL_T_IS_DISABLE(source) && !CURS_CNTRL_U_IS_DISABLE(source);  // Признак того, что включены и вертикальные и 
                                                                            // горизонтальные курсоры - надо нарисовать квадраты в местах пересечения
        int x0 = -1;
        int x1 = -1;
        int y0 = -1;
        int y1 = -1;

        if (bothCursors)
        {
            x0 = Grid::Left() + CURS_POS_T0(source);
            x1 = Grid::Left() + CURS_POS_T1(source);
            y0 = GRID_TOP + sCursors_GetCursPosU(source, 0);
            y1 = GRID_TOP + sCursors_GetCursPosU(source, 1);

            Painter::DrawRectangle(x0 - 2, y0 - 2, 4, 4);
            Painter::DrawRectangle(x1 - 2, y1 - 2, 4, 4);
        }

        CursCntrl cntrl = CURS_CNTRL_T(source);
        if (cntrl != CursCntrl_Disable)
        {
            DrawVerticalCursor(CURS_POS_T0(source), y0);
            DrawVerticalCursor(CURS_POS_T1(source), y1);
        }
        cntrl = CURsU_CNTRL;
        if (cntrl != CursCntrl_Disable)
        {
            DrawHorizontalCursor(sCursors_GetCursPosU(source, 0), x0);
            DrawHorizontalCursor(sCursors_GetCursPosU(source, 1), x1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawMeasures()
{
    if(!SHOW_MEASURES)
    {
        TOP_MEASURES = GRID_BOTTOM;
        return;
    }

    Processing::CalculateMeasures();

    if(MEAS_FIELD_IS_HAND)
    {
        int x0 = MEAS_POS_CUR_T0 - SHIFT_IN_MEMORY + Grid::Left();
        int y0 = MEAS_POS_CUR_U0 + GRID_TOP;
        int x1 = MEAS_POS_CUR_T1 - SHIFT_IN_MEMORY + Grid::Left();
        int y1 = MEAS_POS_CUR_U1 + GRID_TOP;
        SortInt(&x0, &x1);
        SortInt(&y0, &y1);
        Painter::DrawRectangleC(x0, y0, x1 - x0, y1 - y0, COLOR_FILL);
    }

    int x0 = Grid::Left() - Measure_GetDeltaGridLeft();
    int dX = Measure_GetDX();
    int dY = Measure_GetDY();
    int y0 = Measure_GetTopTable();

    int numRows = Measure_NumRows();
    int numCols = Measure_NumCols();

    for(int str = 0; str < numRows; str++)
    {
        for(int elem = 0; elem < numCols; elem++)
        {
            int x = x0 + dX * elem;
            int y = y0 + str * dY;
            bool active = Measure_IsActive(str, elem) && Menu::GetNameOpenedPage() == Page_SB_MeasTuneMeas;
            Color color = active ? COLOR_BACK : COLOR_FILL;
            Measure meas = Measure_Type(str, elem);
            if(meas != Measure_None)
            {
                Painter::FillRegionC(x, y, dX, dY, COLOR_BACK);
                Painter::DrawRectangleC(x, y, dX, dY, COLOR_FILL);
                TOP_MEASURES = Math_MinFrom2Int(TOP_MEASURES, y);
            }
            if(active)
            {
                Painter::FillRegionC(x + 2, y + 2, dX - 4, dY - 4, COLOR_FILL);
            }
            if(meas != Measure_None)
            {
                char buffer[20];
                Painter::DrawTextC(x + 4, y + 2, Measure_Name(str, elem), color);
                if(meas == MEAS_MARKED)
                {
                    Painter::FillRegionC(x + 1, y + 1, dX - 2, 9, active ? COLOR_BACK : COLOR_FILL);
                    Painter::DrawTextC(x + 4, y + 2, Measure_Name(str, elem), active ? COLOR_FILL : COLOR_BACK);
                }
                if(MEAS_SOURCE_IS_A)
                {
                    Painter::DrawTextC(x + 2, y + 11, Processing::GetStringMeasure(meas, A, buffer), ColorChannel(A));
                }
                else if(MEAS_SOURCE_IS_B)
                {
                    Painter::DrawTextC(x + 2, y + 11, Processing::GetStringMeasure(meas, B, buffer), ColorChannel(B));
                }
                else
                {
                    Painter::DrawTextC(x + 2, y + 11, Processing::GetStringMeasure(meas, A, buffer), ColorChannel(A));
                    Painter::DrawTextC(x + 2, y + 20, Processing::GetStringMeasure(meas, B, buffer), ColorChannel(B));
                }
            }
        }
    }

    if(Menu::GetNameOpenedPage() == Page_SB_MeasTuneMeas)
    {
        Measure_DrawPageChoice();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::WriteTextVoltage(Channel chan, int x, int y)
{
    static const char *couple[] =
    {
        "\x92",
        "\x91",
        "\x90"
    };
    Color color = ColorChannel(chan);

    bool inverse = SET_INVERSE(chan);
    ModeCouple modeCouple = SET_COUPLE(chan);
    Divider multiplier = SET_DIVIDER(chan);
    Range range = SET_RANGE(chan);
    uint rShift = (uint)SET_RSHIFT(chan);
    bool enable = SET_ENABLED(chan);

    if (!MODE_WORK_IS_DIRECT)
    {
        DataSettings *ds = MODE_WORK_IS_DIRECT ? gDSet : gDSmemInt;
        if (ds != 0)
        {
            inverse = (chan == A) ? ds->inverseCh0 : ds->inverseCh1;
            modeCouple = (chan == A) ? ds->modeCouple0 : ds->modeCouple1;
            multiplier = (chan == A) ? ds->multiplier0 : ds->multiplier1;
            range = ds->range[chan];
            rShift = (chan == A) ? ds->rShiftCh0 : ds->rShiftCh1;
            enable = (chan == A) ? ds->enableCh0 : ds->enableCh1;
        }
    }

    if(enable)
    {
        const int widthField = 91;
        const int heightField = 8;

        Color colorDraw = inverse ? COLOR_WHITE : color;
        if(inverse)
        {
            Painter::FillRegionC(x, y, widthField, heightField, color);
        }

        char buffer[100] = {0};

        sprintf(buffer, "%s\xa5%s\xa5%s", (chan == A) ? (set.common.lang == Russian ? "1к" : "1c") : (set.common.lang == Russian ? "2к" : "2c"), couple[modeCouple], 
            sChannel_Range2String(range, multiplier));

        Painter::DrawTextC(x + 1, y, buffer, colorDraw);

        char bufferTemp[20];
        sprintf(buffer, "\xa5%s", sChannel_RShift2String((int16)rShift, range, multiplier, bufferTemp));
        Painter::DrawText(x + 46, y, buffer);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::WriteStringAndNumber(char *text, int x, int y, int number)
{
    char buffer[100];
    Painter::DrawTextC(x, y, text, COLOR_FILL);
    if(number == 0)
    {
        sprintf(buffer, "-");
    }
    else
    {
        sprintf(buffer, "%d", number);
    }
    Painter::DrawTextRelativelyRight(x + 41, y, buffer);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawLowPart()
{
    int y0 = SCREEN_HEIGHT - 19;
    int y1 = SCREEN_HEIGHT - 10;
    int x = -1;

    Painter::DrawHLineC(Grid::ChannelBottom(), 1, Grid::Left() - Measure_GetDeltaGridLeft() - 2, COLOR_FILL);
    Painter::DrawHLine(Grid::FullBottom(), 1, Grid::Left() - Measure_GetDeltaGridLeft() - 2);

    WriteTextVoltage(A, x + 2, y0);

    WriteTextVoltage(B, x + 2, y1);

    Painter::DrawVLineC(x + 95, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, COLOR_FILL);

    x += 98;
    char buffer[100] = {0};

    TBase tBase = SET_TBASE;
    int16 tShift = TSHIFT;

    if (!MODE_WORK_IS_DIRECT)
    {
        DataSettings *ds = MODE_WORK_IS_LATEST ? gDSmemLast : gDSmemInt;
        if (ds != 0)
        {
            tBase = ds->tBase;
            tShift = ds->tShift;
        }
    }

    sprintf(buffer, "р\xa5%s", Tables_GetTBaseString(tBase));
    Painter::DrawText(x, y0, buffer);

    buffer[0] = 0;
    char bufForVal[20];
    sprintf(buffer, "\xa5%s", FPGA::GetTShiftString(tShift, bufForVal));
    Painter::DrawText(x + 35, y0, buffer);

    buffer[0] = 0;
    const char *source[3] = {"1", "2", "\x82"};
    if (MODE_WORK_IS_DIRECT)
    {
        sprintf(buffer, "с\xa5\x10%s", source[TRIG_SOURCE]);
    }

    Painter::DrawTextC(x, y1, buffer, ColorTrig());

    buffer[0] = 0;
    static const char *couple[] =
    {
        "\x92",
        "\x91",
        "\x92",
        "\x92"
    };
    static const char *polar[] =
    {
        "\xa7",
        "\xa6"
    };
    static const char *filtr[] =
    {
        "\xb5\xb6",
        "\xb5\xb6",
        "\xb3\xb4",
        "\xb1\xb2"
    };
    if (MODE_WORK_IS_DIRECT)
    {
        sprintf(buffer, "\xa5\x10%s\x10\xa5\x10%s\x10\xa5\x10", couple[TRIG_INPUT], polar[TRIG_POLARITY]);
        Painter::DrawText(x + 18, y1, buffer);
        Painter::DrawChar(x + 45, y1, filtr[TRIG_INPUT][0]);
        Painter::DrawChar(x + 53, y1, filtr[TRIG_INPUT][1]);
    }

    buffer[0] = '\0';
    const char mode[] =
    {
        '\xb7',
        '\xa0',
        '\xb0'
    };
    if (MODE_WORK_IS_DIRECT)
    {
        sprintf(buffer, "\xa5\x10%c", mode[START_MODE]);
        Painter::DrawText(x + 63, y1, buffer);
    }
    
    Painter::DrawVLineC(x + 79, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2, COLOR_FILL);

    Painter::DrawHLine(GRID_BOTTOM, GRID_RIGHT + 2, SCREEN_WIDTH - 2);
    Painter::DrawHLine(Grid::ChannelBottom(), GRID_RIGHT + 2, SCREEN_WIDTH - 2);

    x += 82;
    y0 = y0 - 3;
    y1 = y1 - 6;
    int y2 = y1 + 6;
    Painter::SetFont(TypeFont_5);
    
    if (MODE_WORK_IS_DIRECT)
    {
        WriteStringAndNumber("накопл", x, y0, NUM_ACCUM);
        WriteStringAndNumber("усредн", x, y1, NUM_AVE);
        WriteStringAndNumber("мн\x93мкс", x, y2, NUM_MIN_MAX);
    }

    x += 42;
    Painter::DrawVLine(x, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Painter::SetFont(TypeFont_8);

    if (MODE_WORK_IS_DIRECT)
    {
        char mesFreq[20] = "\x7c=";
        char buf[20];
        float freq = FPGA::GetFreq();
        if (freq == -1.0f) //-V550
        {
            strcat(mesFreq, "******");
        }
        else
        {
            strcat(mesFreq, Freq2String(freq, false, buf));
        }
        Painter::DrawText(x + 3, GRID_BOTTOM + 2, mesFreq);
    }

    DrawTime(x + 3, GRID_BOTTOM + 11);

    Painter::DrawVLine(x + 55, GRID_BOTTOM + 2, SCREEN_HEIGHT - 2);

    Painter::SetFont(TypeFont_UGO2);

    // Флешка
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        Painter::Draw4SymbolsInRect(x + 57, GRID_BOTTOM + 2, SYMBOL_FLASH_DRIVE);
    }

    // Ethernet
    if ((CLIENT_LAN_IS_CONNECTED || CABLE_LAN_IS_CONNECTED) && gTimerMS > 2000)
    {
        Painter::Draw4SymbolsInRectC(x + 87, GRID_BOTTOM + 2, SYMBOL_ETHERNET, CLIENT_LAN_IS_CONNECTED ? COLOR_FILL : COLOR_FLASH_01);
    }

    if (CLIENT_VCP_IS_CONNECTED || CABLE_VCP_IS_CONNECTED)
    {
        Painter::Draw4SymbolsInRectC(x + 72, GRID_BOTTOM + 2, SYMBOL_USB, CLIENT_VCP_IS_CONNECTED ? COLOR_FILL : COLOR_FLASH_01);
    }
    
    Painter::SetColor(COLOR_FILL);
    // Пиковый детектор
    if(!PEAKDET_IS_DISABLE)
    {
       Painter::DrawChar(x + 38, GRID_BOTTOM + 11, '\x12');
       Painter::DrawChar(x + 46, GRID_BOTTOM + 11, '\x13');
    }

    if (MODE_WORK_IS_DIRECT)
    {
        Painter::SetFont(TypeFont_5);
        WriteStringAndNumber("СГЛАЖ.:", x + 57, GRID_BOTTOM + 10, sDisplay_NumPointSmoothing());
        Painter::SetFont(TypeFont_8);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawTimeForFrame(uint timeTicks)
{
    if(!SHOW_STATS)
    {
        return;
    }
    static char buffer[10];
    static bool first = true;
    static uint timeMSstartCalculation = 0;
    static int numFrames = 0;
    static float numMS = 0.0f;
    if(first)
    {
        timeMSstartCalculation = gTimerMS;
        first = false;
    }
    numMS += timeTicks / 120000.0f;
    numFrames++;
    
    if((gTimerMS - timeMSstartCalculation) >= 500)
    {
        sprintf(buffer, "%.1fms/%d", numMS / numFrames, numFrames * 2);
        timeMSstartCalculation = gTimerMS;
        numMS = 0.0f;
        numFrames = 0;
    }

    Painter::DrawRectangleC(Grid::Left(), Grid::FullBottom() - 10, 84, 10, COLOR_FILL);
    Painter::FillRegionC(Grid::Left() + 1, Grid::FullBottom() - 9, 82, 8, COLOR_BACK);
    Painter::DrawTextC(Grid::Left() + 2, Grid::FullBottom() - 9, buffer, COLOR_FILL);

    char message[20] = {0};
    sprintf(message, "%d", Storage::NumElementsWithSameSettings());
    strcat(message, "/");
    char numAvail[10] = {0};
    sprintf(numAvail, "%d", Storage::NumberAvailableEntries());
    strcat(message, numAvail);
    Painter::DrawText(Grid::Left() + 50, Grid::FullBottom() - 9, message);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DisableShowLevelRShiftA()
{
    SHOW_LEVEL_RSHIFT_0 = 0;
    Timer::Disable(kShowLevelRShift0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DisableShowLevelRShiftB()
{
    SHOW_LEVEL_RSHIFT_1 = 0;
    Timer::Disable(kShowLevelRShift1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DisableShowLevelTrigLev()
{
    SHOW_LEVEL_TRIGLEV = 0;
    Timer::Disable(kShowLevelTrigLev);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::EnableTrigLabel(bool enable)
{
    TRIG_ENABLE = enable ? 1U : 0U;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ResetP2Ppoints(bool empty)
{
    dataP2PIsEmpty = empty;
    lastP2Pdata = 0;
    memset(dataP2P_0, AVE_VALUE, NUM_P2P_POINTS);
    memset(dataP2P_1, AVE_VALUE, NUM_P2P_POINTS);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::AddPoints(uint8 data00, uint8 data01, uint8 data10, uint8 data11)
{
    dataP2PIsEmpty = false;
    if (SET_SELFRECORDER)
    {
        if (lastP2Pdata == NUM_P2P_POINTS)
        {
            memcpy(dataP2P_0, dataP2P_0 + 2, NUM_P2P_POINTS - 2);
            memcpy(dataP2P_1, dataP2P_1 + 2, NUM_P2P_POINTS - 2);
        }
    }

    dataP2P_0[lastP2Pdata] = data00;
    dataP2P_1[lastP2Pdata++] = data10;
    
    dataP2P_0[lastP2Pdata] = data01;
    dataP2P_1[lastP2Pdata++] = data11;
    if (!SET_SELFRECORDER && lastP2Pdata >= NUM_P2P_POINTS)
    {
        lastP2Pdata = 0;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::SetDrawMode(DrawMode mode, pFuncVV func)
{
    funcOnHand = mode == DrawMode_Auto ? 0 : func;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::SetAddDrawFunction(pFuncVV func)
{
    funcAdditionDraw = func;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RemoveAddDrawFunction()
{
    funcAdditionDraw = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::Clear()
{
    Painter::FillRegionC(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2, COLOR_BACK);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShiftScreen(int delta)
{
    LIMITATION(SHIFT_IN_MEMORY, SHIFT_IN_MEMORY + delta, 0, sMemory_GetNumPoints(false) - 282);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ChangedRShiftMarkers()
{
    DRAW_RSHIFT_MARKERS = ALT_MARKERS_HIDE ? 0U : 1U;
    Timer::Enable(kRShiftMarkersAutoHide, 5000, FuncOnTimerRShiftMarkersAutoHide);
}

void Display::FuncOnTimerRShiftMarkersAutoHide()
{
    Display::OnRShiftMarkersAutoHide();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::OnRShiftMarkersAutoHide()
{
    DRAW_RSHIFT_MARKERS = 0;
    Timer::Disable(kRShiftMarkersAutoHide);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Display::FirstEmptyString()
{
    for(int i = 0; i < MAX_NUM_STRINGS; i++)
    {
        if(strings[i] == 0)
        {
            return i;
        }
    }
    return MAX_NUM_STRINGS;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Display::CalculateFreeSize()
{
    int firstEmptyString = FirstEmptyString();
    if(firstEmptyString == 0)
    {
        return SIZE_BUFFER_FOR_STRINGS;
    }
    return SIZE_BUFFER_FOR_STRINGS - (strings[firstEmptyString - 1] - bufferForStrings) - strlen(strings[firstEmptyString - 1]) - 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DeleteFirstString()
{
    if(FirstEmptyString() < 2)
    {
        return;
    }
    int delta = strlen(strings[0]) + 1;
    int numStrings = FirstEmptyString();
    for(int i = 1; i < numStrings; i++)
    {
        strings[i - 1] = strings[i] - delta;
    }
    for(int i = numStrings - 1; i < MAX_NUM_STRINGS; i++)
    {
        strings[i] = 0;
    }
    for(int i = 0; i < SIZE_BUFFER_FOR_STRINGS - delta; i++)
    {
        bufferForStrings[i] = bufferForStrings[i + delta];
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::AddString(const char *string)
{
    if(CONSOLE_IN_PAUSE)
    {
        return;
    }
    static int num = 0;
    char buffer[100];
    sprintf(buffer, "%d\x11", num++);
    strcat(buffer, string);
    int size = strlen(buffer) + 1;
    while(CalculateFreeSize() < size)
    {
        DeleteFirstString();
    }
    if(!strings[0])
    {
        strings[0] = bufferForStrings;
        strcpy(strings[0], buffer);
    }
    else
    {
        char *addressLastString = strings[FirstEmptyString() - 1];
        char *address = addressLastString + strlen(addressLastString) + 1;
        strings[FirstEmptyString()] = address;
        strcpy(address, buffer);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::AddStringToIndicating(const char *string)
{
    if(FirstEmptyString() == MAX_NUM_STRINGS)
    {
        DeleteFirstString();
    }

    AddString(string);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::SetPauseForConsole(bool pause)
{
    if(pause)
    {
        lastStringForPause = FirstEmptyString() - 1;
    }
    else
    {
        lastStringForPause = -1;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::OneStringUp()
{
    if(!CONSOLE_IN_PAUSE)
    {
    }
    else if(lastStringForPause > NUM_STRINGS - 1)
    {
        lastStringForPause--;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::OneStringDown()
{
    if(!CONSOLE_IN_PAUSE)
    {
    }
    else if(lastStringForPause < FirstEmptyString() - 1)
    {
        lastStringForPause++;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawConsole()
{
    int count = 0;
    Painter::SetFont(sDebug_GetSizeFontForConsole() == 5 ? TypeFont_5 : TypeFont_8);
    int height = Font_GetSize();

    int lastString = FirstEmptyString() - 1;
    int numStr = NUM_STRINGS;
    if(height == 8 && numStr > 22)
    {
        numStr = 22;
    }
    int delta = 0;
    if(SHOW_STRING_NAVIGATION)
    {
        numStr -= ((height == 8) ? 1 : 2);
        delta = 10;
    }
    int firstString = lastString - numStr + 1;
    if(firstString < 0)
    {
        firstString = 0;
    }

    int dY = 0;
    
    for(int numString = firstString; numString <= lastString; numString++)
    {
        int width = Font_GetLengthText(strings[numString]);
        Painter::FillRegionC(Grid::Left() + 1, GRID_TOP + 1 + count * (height + 1) + delta, width, height + 1, COLOR_BACK);
        int y = GRID_TOP + 5 + count * (height + 1) - 4;
        if(Font_GetSize() == 5)
        {
            y -= 3;
        }
        Painter::DrawTextC(Grid::Left() + 2, y + dY + delta, strings[numString], COLOR_FILL);
        count++;
    }

    Painter::SetFont(TypeFont_8);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShowWarn(const char *message)
{
    if (warnings[0] == 0)
    {
        Timer::Enable(kShowMessages, 100, OnTimerShowWarning);
    }
    bool alreadyStored = false;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] == 0 && !alreadyStored)
        {
            warnings[i] = message;
            timeWarnings[i] = gTimerMS;
            alreadyStored = true;
        }
        else if (warnings[i] == message)
        {
            timeWarnings[i] = gTimerMS;
            return;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::OnTimerShowWarning()
{
    uint time = gTimerMS;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (time - timeWarnings[i] > TIME_MESSAGES * 1000)
        {
            timeWarnings[i] = 0;
            warnings[i] = 0;
        }
    }

    int pointer = 0;
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        if (warnings[i] != 0)
        {
            warnings[pointer] = warnings[i];
            timeWarnings[pointer] = timeWarnings[i];
            if (pointer != i)
            {
                timeWarnings[i] = 0;
                warnings[i] = 0;
            }
            pointer++;
        }
    }

    if (pointer == 0)
    {
        Timer::Disable(kShowMessages);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ClearFromWarnings()
{
    Timer::Disable(kShowMessages);
    for (int i = 0; i < NUM_WARNINGS; i++)
    {
        warnings[i] = 0;
        timeWarnings[i] = 0;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShowWarningBad(Warning warning)
{
    Painter::ResetFlash();
    ShowWarn(Tables_GetWarning(warning));
    Sound::WarnBeepBad();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::ShowWarningGood(Warning warning)
{
    Painter::ResetFlash();
    ShowWarn(Tables_GetWarning(warning));
    Sound::WarnBeepGood();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawStringInRectangle(int x, int y, char const *text)
{
    int width = Font_GetLengthText(text);
    int height = 8;
    Painter::DrawRectangleC(Grid::Left(), y, width + 4, height + 4, COLOR_FILL);
    Painter::DrawRectangleC(Grid::Left() + 1, y + 1, width + 2, height + 2, COLOR_BACK);
    Painter::FillRegionC(Grid::Left() + 2, y + 2, width, height, COLOR_FLASH_10);
    Painter::DrawTextC(Grid::Left() + 3, y + 2, text, COLOR_FLASH_01);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::DrawWarnings()
{
    int delta = 12;
    int y = Grid::BottomMessages();
    for(int i = 0; i < 10; i++)
    {
        if(warnings[i] != 0)
        {
            DrawStringInRectangle(Grid::Left(), y, warnings[i]);
            y -= delta;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Display::RunAfterDraw(pFuncVV func)
{
    funcAfterDraw = func;
}
