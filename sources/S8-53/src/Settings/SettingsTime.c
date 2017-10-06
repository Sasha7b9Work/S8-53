#include "SettingsTime.h"
#include "Settings.h"
#include "Log.h"
#include "Utils/Math.h"
#include "FPGA/FPGA.h"


/** @addtogroup Settings
 *  @{
 *  @addtogroup SettingsTime
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sTime_SetTBase(TBase tBase)
{
    TBASE = tBase;
    fpga.ClearData();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void sTime_SetTShift(int16 shift)
{
    TSHIFT = shift;
    fpga.ClearData();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sTime_TPosInPoints(PeackDetMode peakDet, int numPoints, TPos tPos)
{
    if (peakDet == PeackDet_Disable)
    {
        static const int m[3][3] =
        {
            {0, 140, 280},
            {0, 255, 511},
            {0, 512, 1022}
        };
        return m[sMemory_IntNumPoints2FPGA_NUM_POINTS(numPoints)][tPos];
    }
    else
    {
        static const int m[3][3] =
        {
            {0, 140, 280},
            {0, 256, 510},
            {0, 256, 510}
        };
        return m[sMemory_IntNumPoints2FPGA_NUM_POINTS(numPoints)][tPos];
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int sTime_TShiftInPoints(PeackDetMode peakDet)
{
    return TSHIFT * (peakDet == PeackDet_Disable ? 2 : 1);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 sTime_TShiftMin()
{
    static const int16 m[3][3] = {{-511, -441, -371},
    {-511, -383, -255},
    {-511, -255, 0}};

    ENUM_POINTS_FPGA numPoints = ENUM_POINTS;
    if ((int)numPoints < 3 && (int)numPoints >= 0)
    {
        return m[numPoints][TPOS];
    }
    LOG_ERROR("");
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 sTime_TShiftZero()
{
    return -sTime_TShiftMin();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool sTime_RandomizeModeEnabled()
{
    return TBASE < TBase_100ns;
}


/** @}  @}
 */
