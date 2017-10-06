// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "FPGA_Types.h"
#include "FPGA.h"
#include "Settings/Settings.h"
#include "Hardware/Hardware.h"
#include "Hardware/FSMC.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Log.h"
#include "Hardware/Timer.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint8 masksRange[RangeSize] =
{
    BINARY_U8(0000000),
    BINARY_U8(1000000),
    BINARY_U8(1100000),
    BINARY_U8(1010000),
    BINARY_U8(1000100),
    BINARY_U8(1100100),
    BINARY_U8(1010100),
    BINARY_U8(1000010),
    BINARY_U8(1100010),
    BINARY_U8(1010010),
    BINARY_U8(1000110),
    BINARY_U8(1100110),
    BINARY_U8(1010110)
};


// Добавочные смещения по времени для разверёток режима рандомизатора.
static int16 deltaTShift[TBaseSize] = {505, 489, 464, 412, 258};

typedef struct
{
    uint8 maskNorm;         // Маска. Требуется для записи в аппаратную часть при выключенном режиме пикового детектора.
    uint8 maskPeackDet;     // Маска. Требуется для записи в аппаратную часть при включенном режиме пикового детектора.
} TBaseMaskStruct;

static const TBaseMaskStruct masksTBase[TBaseSize] =
{
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00000000), BINARY_U8(00000000)},
    {BINARY_U8(00100010), BINARY_U8(00000000)},
    {BINARY_U8(00100001), BINARY_U8(00100011)},
    {BINARY_U8(01000000), BINARY_U8(00100001)},
    {BINARY_U8(01000010), BINARY_U8(01000000)},
    {BINARY_U8(01000001), BINARY_U8(01000011)},
    {BINARY_U8(01000100), BINARY_U8(01000001)},
    {BINARY_U8(01000110), BINARY_U8(01000100)},
    {BINARY_U8(01000101), BINARY_U8(01000111)},
    {BINARY_U8(01001000), BINARY_U8(01000101)},
    {BINARY_U8(01001010), BINARY_U8(01001000)},
    {BINARY_U8(01001001), BINARY_U8(01001011)},
    {BINARY_U8(01001100), BINARY_U8(01001001)},
    {BINARY_U8(01001110), BINARY_U8(01001100)},
    {BINARY_U8(01001101), BINARY_U8(01001111)},
    {BINARY_U8(01010000), BINARY_U8(01001101)},
    {BINARY_U8(01010010), BINARY_U8(01010000)},
    {BINARY_U8(01010001), BINARY_U8(01010011)},
    {BINARY_U8(01010100), BINARY_U8(01010001)},
    {BINARY_U8(01010110), BINARY_U8(01010100)},
    {BINARY_U8(01010101), BINARY_U8(01010111)},
    {BINARY_U8(01011000), BINARY_U8(01010101)},
    {BINARY_U8(01011010), BINARY_U8(01011000)},
    {BINARY_U8(01011001), BINARY_U8(01011011)},
    {BINARY_U8(01011100), BINARY_U8(01011001)}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void LoadTBase(void);                                    // Загрузка коэффицента развёртки в аппаратную часть.
static void LoadTShift(void);                                   // Загрузка смещения по времени в аппаратную часть.
static void LoadRange(Channel chan);                            // Загрузка масштаба по напряжению в аппаратную часть.
static void LoadRShift(Channel chan);                           // Загрузка смещения по напряжению в аппаратную часть.
static void LoadTrigLev(void);                                  // Загрузка уровня синхронизации в аппаратную часть.
static void LoadTrigPolarity(void);                             // Загузка полярности синхронизации в аппаратную часть.
static void SetAttribChannelsAndTrig(TypeWriteAnalog type);     // Загрузить все параметры напряжения каналов и синхронизации в аппаратную часть.
static void LoadRegUPR(void);                                   // Загрузить регистр WR_UPR (пиковый детектор и калибратор).


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FPGA_LoadSettings(void)
{
    FPGA_LoadKoeffCalibration(A);
    FPGA_LoadKoeffCalibration(B);
    SetAttribChannelsAndTrig(TypeWriteAnalog_All);
    LoadTBase();
    LoadTShift();
    LoadRange(A);
    LoadRShift(A);
    LoadRange(B);
    LoadRShift(B);
    LoadTrigLev();
    LoadTrigPolarity();
    LoadRegUPR();

    switch(BALANCE_ADC_TYPE) 
    {
        case BalanceADC_Settings:
            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, BALANCE_SHIFT_ADC_A, false);
            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, BALANCE_SHIFT_ADC_B, false);
            break;
        case BalanceADC_Hand:
            FPGA_SetPeackDetMode(PEAKDET);
            FPGA_SetTBase(TBASE);
            if (PEAKDET)
            {
                FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
                FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
            }
            else
            {
                FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, BALANCE_ADC_A, false);
                FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, BALANCE_ADC_B, false);
            }
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SetAttribChannelsAndTrig(TypeWriteAnalog type) 
{
    uint data = 0;

    /*
    b0...b7 - Channel1
    b8...b15 - Channel2
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

    // Range0, Range1
    data |= masksRange[RANGE_A];
    data |= (masksRange[RANGE_B] << 8);

    // Параметры каналов
    static const uint maskCouple[2][3] = 
    {
        {0x0008, 0x0000, 0x0030},
        {0x0800, 0x0000, 0x3000}
    };

    data |= maskCouple[A][MODE_COUPLE_A];
    data |= maskCouple[B][MODE_COUPLE_B];

    static const uint maskFiltr[2][2] = 
    {
        {0x0000, 0x0080},
        {0x0000, 0x0100}
    };

    data |= maskFiltr[A][FILTR_A];
    data |= maskFiltr[B][FILTR_B];


    // Параметры синхронизации
    static const uint maskSource[3] = {0x000000, 0x800000, 0x400000};
    data |= maskSource[TRIG_SOURCE];

    static const uint maskInput[4] = {0x000000, 0x030000, 0x020000, 0x010000};
    data |= maskInput[TRIG_INPUT];

    Hardware_WriteToAnalog(type, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetRange(Channel chan, Range range)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    if (range < RangeSize && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(RSHIFT(chan), RANGE(chan));
        float trigLevAbs = RSHIFT_2_ABS(TRIG_LEVEL(chan), RANGE(chan));
        sChannel_SetRange(chan, range);
        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            RSHIFT(chan) = Math_RShift2Rel(rShiftAbs, range);
            TRIG_LEVEL(chan) = Math_RShift2Rel(trigLevAbs, range);
        }
        LoadRange(chan);
    }
    else
    {
        Display_ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadRange(Channel chan)
{
    SetAttribChannelsAndTrig(TypeWriteAnalog_Range0);
    LoadRShift(chan);
    if (chan == (Channel)TRIG_SOURCE)
    {
        LoadTrigLev();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTBase(TBase tBase)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }
    if (tBase < TBaseSize && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(TSHIFT, TBASE);
        sTime_SetTBase(tBase);
        LoadTBase();
        FPGA_SetTShift(TSHIFT_2_REL(tShiftAbsOld, TBASE));
        Display_Redraw();
    }
    else
    {
        Display_ShowWarningBad(LimitSweep_Time);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTBase(void)
{
    TBase tBase = TBASE;
    uint8 mask = PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    FPGA_WriteToHardware(WR_RAZVERTKA, mask, true);
    ADD_SHIFT_T0 = deltaTShift[tBase];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TBaseDecrease(void)
{
    if (PEAKDET && TBASE <= MIN_TBASE_PEC_DEAT)
    {
        Display_ShowWarningBad(LimitSweep_Time);
        Display_ShowWarningBad(EnabledPeakDet);
        return;
    }

    if ((int)TBASE > 0)
    {
        if (SELFRECORDER && TBASE == MIN_TBASE_P2P)
        {
            Display_ShowWarningBad(TooFastScanForSelfRecorder);
        }
        else
        {
            TBase base = (TBase)((int)TBASE - 1);
            FPGA_SetTBase(base);
        }
    }
    else
    {
        Display_ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_TBaseIncrease(void)
{
    if (TBASE < (TBaseSize - 1))
    {
        TBase base = (TBase)(TBASE + 1);
        FPGA_SetTBase(base);
    }
    else
    {
        Display_ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetRShift(Channel chan, int16 rShift)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    Display_ChangedRShiftMarkers(true);

    if (rShift > RShiftMax || rShift < RShiftMin)
    {
        Display_ShowWarningBad(chan == A ? LimitChan1_RShift : LimitChan2_RShift);
    }

    LIMITATION(rShift, rShift, RShiftMin, RShiftMax);
    if (rShift > RShiftZero)
    {
        rShift &= 0xfffe;                                            // Делаем кратным двум, т.к. у нас 800 значений на 400 точек
    }
    else if (rShift < RShiftZero)
    {
        rShift = (rShift + 1) & 0xfffe;
    }
    RSHIFT(chan) = rShift;
    LoadRShift(chan);
    Display_RotateRShift(chan);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadRShift(Channel chan)
{
    static const uint16 mask[2] = {0x2000, 0x6000};

    Range range = RANGE(chan);
    ModeCouple mode = MODE_COUPLE(chan);
    static const int index[3] = {0, 1, 1};
    int16 rShiftAdd = set.chan[chan].rShiftAdd[range][index[mode]];

    uint16 rShift = RSHIFT(chan) + (INVERSE(chan) ? -1 : 1) * rShiftAdd;

    int16 delta = -(rShift - RShiftZero);
    if (INVERSE(chan))
    {
        delta = -delta;
    }
    rShift = delta + RShiftZero;

    rShift = RShiftMax + RShiftMin - rShift;
    Hardware_WriteToDAC(chan == A ? TypeWriteDAC_RShift0 : TypeWriteDAC_RShift1, mask[chan] | (rShift << 2));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigLev(TrigSource chan, int16 trigLev)
{
    Display_ChangedRShiftMarkers(true);
    if (trigLev < TrigLevMin || trigLev > TrigLevMax)
    {
        Display_ShowWarningBad(LimitSweep_Level);
    }
    LIMITATION(trigLev, trigLev, TrigLevMin, TrigLevMax);

    if (trigLev > TrigLevZero)
    {
        trigLev &= 0xfffe;
    }
    else if (trigLev < TrigLevZero)
    {
        trigLev = (trigLev + 1) & 0xfffe;
    }

    if (TRIG_LEVEL(chan) != trigLev)
    {
        TRIG_LEVEL(chan) = trigLev;
        LoadTrigLev();
        Display_RotateTrigLev();
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTrigLev(void)
{
    uint16 data = 0xa000;
    uint16 trigLev = TRIG_LEVEL_SOURCE;
    trigLev = TrigLevMax + TrigLevMin - trigLev;
    data |= trigLev << 2;
    // FPGA_WriteToHardware(WR_DAC_LOW, data.byte[0], true);
    // FPGA_WriteToHardware(WR_DAC_HI, data.byte[1], true);
    Hardware_WriteToDAC(TypeWriteDAC_TrigLev, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTShift(int tShift)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }

    if (tShift < sTime_TShiftMin() || tShift > TShiftMax)
    {
        LIMITATION(tShift, tShift, sTime_TShiftMin(), TShiftMax);
        Display_ShowWarningBad(LimitSweep_TShift);
    }

    sTime_SetTShift(tShift);
    LoadTShift();
    Display_Redraw();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetDeltaTShift(int16 shift)
{
    deltaTShift[TBASE] = shift;
    LoadTShift();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetPeackDetMode(PeackDetMode peackDetMode)
{
    PEAKDET = peackDetMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetCalibratorMode(CalibratorMode calibratorMode)
{
    CALIBRATOR = calibratorMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadRegUPR(void)
{
    uint8 data = 0;
    if (sTime_RandomizeModeEnabled())
    {
        _SET_BIT(data, 0);
    }
    if (!PEAKDET_IS_DISABLE)
    {
        _SET_BIT(data, 1);
    }
    if (CALIBRATOR_IS_FREQ)
    {
        _SET_BIT(data, 2);
    }
    else if (CALIBRATOR_IS_DC)
    {
        _SET_BIT(data, 3);
    }

    FPGA_WriteToHardware(WR_UPR, data, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_LoadKoeffCalibration(Channel chan)
{
    FPGA_WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, STRETCH_ADC(chan) * 0x80, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTShift(void)
{
    static const int16 k[TBaseSize] = {50, 20, 10, 5, 2};
    int16 tShift = TSHIFT - sTime_TShiftMin() + 1;
    int16 tShiftOld = tShift;
    TBase tBase = TBASE;
    if (tBase < TBase_100ns)
    {
        tShift = tShift / k[tBase] + deltaTShift[tBase];
    }
    int additionShift = (tShiftOld % k[tBase]) * 2;
    FPGA_SetAdditionShift(additionShift);
    uint16 post = tShift;
    post = (~post);
    FPGA_WriteToHardware(WR_POST_LOW, (uint8)post, true);
    FPGA_WriteToHardware(WR_POST_HI, (uint8)(post >> 8), true);
    uint16 pred = (tShift > 511) ? 1023 : (511 - post);
    pred = (~(pred - 1)) & 0x1ff;
    FPGA_WriteToHardware(WR_PRED_LOW, (uint8)pred, true);
    FPGA_WriteToHardware(WR_PRED_HI, (uint8)(pred >> 8), true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *FPGA_GetTShiftString(int16 tShiftRel, char buffer[20])
{
    float tShiftVal = TSHIFT_2_ABS(tShiftRel, TBASE);
    return Time2String(tShiftVal, true, buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_RangeIncrease(Channel chan)
{
    bool retValue = false;
    if (RANGE(chan) < RangeSize - 1)
    {
        FPGA_SetRange(chan, (Range)(RANGE(chan) + 1));
        retValue = true;
    }
    else
    {
       Display_ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display_Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA_RangeDecrease(Channel chan)
{
    bool retValue = false;
    if (RANGE(chan) > 0)
    {
        FPGA_SetRange(chan, (Range)(RANGE(chan) - 1));
        retValue = true;
    }
    else
    {
        Display_ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display_Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigSource(TrigSource trigSource)
{
    TRIG_SOURCE = trigSource;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
    if (!TRIG_SOURCE_IS_EXT)
    {
        FPGA_SetTrigLev(TRIG_SOURCE, TRIG_LEVEL_SOURCE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigPolarity(TrigPolarity polarity)
{
    TRIG_POLARITY = polarity;
    LoadTrigPolarity();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void LoadTrigPolarity(void)
{
    FPGA_WriteToHardware(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01 : 0x00, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetTrigInput(TrigInput trigInput)
{
    TRIG_INPUT = trigInput;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_SetModeCouple(Channel chan, ModeCouple modeCoupe)
{
    MODE_COUPLE(chan) = modeCoupe;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParam0 : TypeWriteAnalog_ChanParam1);
    FPGA_SetRShift(chan, RSHIFT(chan));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA_EnableChannelFiltr(Channel chan, bool enable)
{
    FILTR(chan) = enable;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParam0 : TypeWriteAnalog_ChanParam1);
}
