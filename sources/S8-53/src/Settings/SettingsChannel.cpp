

#include "Settings.h"
#include "SettingsChannel.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"


/** @addtogroup Settings
 *  @{
 *  @addtogroup SettingsChannel
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Структура для описания диапазона масштаба по напряжению.
typedef struct
{
    const char* name[2][2];     ///< Название диапазона в текстовом виде, пригодном для вывода на экран.
} RangeStruct;


// Массив структур описаний масштабов по напряжению.
static const RangeStruct ranges[RangeSize] =
{
    {"2\x10мВ",     "20\x10мВ", "2\x10mV",  "20\x10mV"},
    {"5\x10мВ",     "50\x10мВ", "5\x10mV",  "50\x10mV"},
    {"10\x10мВ",    "0.1\x10В", "10\x10mV", "0.1\x10V"},
    {"20\x10мВ",    "0.2\x10В", "20\x10mV", "0.2\x10V"},
    {"50\x10мВ",    "0.5\x10В", "50\x10mV", "0.5\x10V"},
    {"0.1\x10В",    "1\x10В",   "0.1\x10V", "1\x10V"},
    {"0.2\x10В",    "2\x10В",   "0.2\x10V", "2\x10V"},
    {"0.5\x10В",    "5\x10В",   "0.5\x10V", "5\x10V"},
    {"1\x10В",      "10\x10В",  "1\x10V",   "10\x10V"},
    {"2\x10В",      "20\x10В",  "2\x10V",   "20\x10V"},
    {"5\x10В",      "50\x10В",  "5\x10V",   "50\x10V"},
    {"10\x10В",     "100\x10В", "10\x10V",  "100\x10V"},
    {"20\x10В",     "200\x10В", "20\x10V",  "200\x10V"}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sChannel_SetRange(Channel chan, Range range)
{
    SET_RANGE(chan) = range;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sChannel_MultiplierRel2Abs(Divider multiplier)
{
    switch (multiplier)
    {
        case Divider_10:
            return 10;
        case Divider_1:
            return 1;
    }
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool sChannel_Enabled(Channel chan)
{
    if (chan == Math && !DISABLED_DRAW_MATH)
    {
        return true;
    }
    return SET_ENABLED(chan);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* sChannel_Range2String(Range range, Divider multiplier)
{
    return ranges[range].name[set.common.lang][multiplier];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* sChannel_RShift2String(int16 rShiftRel, Range range, Divider multiplier, char buffer[20])
{
    float rShiftVal = RSHIFT_2_ABS(rShiftRel, range) * sChannel_MultiplierRel2Abs(multiplier);
    return Voltage2String(rShiftVal, true, buffer);
};


/** @}  @}
 */
