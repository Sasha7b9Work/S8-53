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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FPGA::LoadSettings(void)
{
    LoadKoeffCalibration(A);
    LoadKoeffCalibration(B);
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
            WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)SET_BALANCE_ADC_A, false);
            WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)SET_BALANCE_ADC_B, false);
            break;
        case BalanceADC_Hand:
            SetPeackDetMode(PEAKDET);
            SetTBase(SET_TBASE);
            if (PEAKDET)
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
                WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
            }
            else
            {
                WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)BALANCE_ADC_A, false);
                WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)BALANCE_ADC_B, false);
            }
            break;
        case BalanceADC_Disable:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetAttribChannelsAndTrig(TypeWriteAnalog type) 
{
    uint data = 0;

    /*
    b0...b7 - Channel1
    b8...b15 - Channel2
    b16...b23 - Trig
    Передаваться биты должны начиная с b0
    */

    // Range0, Range1
    data |= masksRange[SET_RANGE_A];
    data |= (masksRange[SET_RANGE_B] << 8);

    // Параметры каналов
    static const uint maskCouple[2][3] = 
    {
        {0x0008, 0x0000, 0x0030},
        {0x0800, 0x0000, 0x3000}
    };

    data |= maskCouple[A][SET_COUPLE_A];
    data |= maskCouple[B][SET_COUPLE_B];

    static const uint maskFiltr[2][2] = 
    {
        {0x0000, 0x0080},
        {0x0000, 0x0100}
    };

    data |= maskFiltr[A][SET_FILTR_A];
    data |= maskFiltr[B][SET_FILTR_B];


    // Параметры синхронизации
    static const uint maskSource[3] = {0x000000, 0x800000, 0x400000};
    data |= maskSource[TRIG_SOURCE];

    static const uint maskInput[4] = {0x000000, 0x030000, 0x020000, 0x010000};
    data |= maskInput[TRIG_INPUT];

    WriteToAnalog(type, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetRange(Channel chan, Range range)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    if (range < RangeSize && (int)range >= 0)
    {
        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT(chan), SET_RANGE(chan));
        float trigLevAbs = RSHIFT_2_ABS(TRIG_LEVEL(chan), SET_RANGE(chan));
        sChannel_SetRange(chan, range);
        if (LINKING_RSHIFT_IS_VOLTAGE)
        {
            SET_RSHIFT(chan) = (int16)Math_RShift2Rel(rShiftAbs, range);
            TRIG_LEVEL(chan) = (int16)Math_RShift2Rel(trigLevAbs, range);
        }
        LoadRange(chan);
    }
    else
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRange(Channel chan)
{
    SetAttribChannelsAndTrig(TypeWriteAnalog_Range0);
    LoadRShift(chan);
    if (chan == (Channel)TRIG_SOURCE)
    {
        LoadTrigLev();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTBase(TBase tBase)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }
    if (tBase < TBaseSize && (int)tBase >= 0)
    {
        float tShiftAbsOld = TSHIFT_2_ABS(TSHIFT, SET_TBASE);
        sTime_SetTBase(tBase);
        LoadTBase();
        FPGA::SetTShift(TSHIFT_2_REL(tShiftAbsOld, SET_TBASE));
        Display::Redraw();
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTBase(void)
{
    TBase tBase = SET_TBASE;
    uint8 mask = PEAKDET ? masksTBase[tBase].maskPeackDet : masksTBase[tBase].maskNorm;
    FPGA::WriteToHardware(WR_RAZVERTKA, mask, true);
    ADD_SHIFT_T0 = deltaTShift[tBase];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::TBaseDecrease(void)
{
    if (PEAKDET && SET_TBASE <= MIN_TBASE_PEC_DEAT)
    {
        Display::ShowWarningBad(LimitSweep_Time);
        Display::ShowWarningBad(EnabledPeakDet);
        return;
    }

    if ((int)SET_TBASE > 0)
    {
        if (SET_SELFRECORDER && SET_TBASE == MIN_TBASE_P2P)
        {
            Display::ShowWarningBad(TooFastScanForSelfRecorder);
        }
        else
        {
            TBase base = (TBase)((int)SET_TBASE - 1);
            FPGA::SetTBase(base);
        }
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::TBaseIncrease(void)
{
    if (SET_TBASE < (TBaseSize - 1))
    {
        TBase base = (TBase)(SET_TBASE + 1);
        FPGA::SetTBase(base);
    }
    else
    {
        Display::ShowWarningBad(LimitSweep_Time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetRShift(Channel chan, int16 rShift)
{
    if (!sChannel_Enabled(chan))
    {
        return;
    }
    Display::ChangedRShiftMarkers();

    if (rShift > RShiftMax || rShift < RShiftMin)
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_RShift : LimitChan2_RShift);
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
    SET_RSHIFT(chan) = rShift;
    LoadRShift(chan);
    Display::RotateRShift(chan);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRShift(Channel chan)
{
    static const uint16 mask[2] = {0x2000, 0x6000};

    Range range = SET_RANGE(chan);
    ModeCouple mode = SET_COUPLE(chan);
    static const int index[3] = {0, 1, 1};
    int16 rShiftAdd = RSHIFT_ADD(chan, range, index[mode]);

    uint16 rShift = (uint16)(SET_RSHIFT(chan) + (SET_INVERSE(chan) ? -1 : 1) * rShiftAdd);

    int16 delta = -(rShift - RShiftZero);
    if (SET_INVERSE(chan))
    {
        delta = -delta;
    }
    rShift = (uint16)(delta + RShiftZero);

    rShift = (uint16)(RShiftMax + RShiftMin - rShift);
    WriteToDAC(chan == A ? TypeWriteDAC_RShiftA : TypeWriteDAC_RShiftB, (uint16)(mask[chan] | (rShift << 2)));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigLev(TrigSource chan, int16 trigLev)
{
    Display::ChangedRShiftMarkers();
    if (trigLev < TrigLevMin || trigLev > TrigLevMax)
    {
        Display::ShowWarningBad(LimitSweep_Level);
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
        Display::RotateTrigLev();
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTrigLev(void)
{
    uint16 data = 0xa000;
    uint16 trigLev = (uint16)TRIG_LEVEL_SOURCE;
    trigLev = (uint16)(TrigLevMax + TrigLevMin - trigLev);
    data |= trigLev << 2;
    // FPGA_WriteToHardware(WR_DAC_LOW, data.byte[0], true);
    // FPGA_WriteToHardware(WR_DAC_HI, data.byte[1], true);
    WriteToDAC(TypeWriteDAC_TrigLev, data);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTShift(int tShift)
{
    if (!sChannel_Enabled(A) && !sChannel_Enabled(B))
    {
        return;
    }

    if (tShift < sTime_TShiftMin() || tShift > TShiftMax)
    {
        LIMITATION(tShift, tShift, sTime_TShiftMin(), TShiftMax);
        Display::ShowWarningBad(LimitSweep_TShift);
    }

    sTime_SetTShift((int16)tShift);
    LoadTShift();
    Display::Redraw();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetDeltaTShift(int16 shift)
{
    deltaTShift[SET_TBASE] = shift;
    LoadTShift();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetPeackDetMode(PeackDetMode peackDetMode)
{
    PEAKDET = peackDetMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetCalibratorMode(CalibratorMode calibratorMode)
{
    CALIBRATOR = calibratorMode;
    LoadRegUPR();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadRegUPR(void)
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

    FPGA::WriteToHardware(WR_UPR, data, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadKoeffCalibration(Channel chan)
{
    FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, STRETCH_ADC(chan) * 0x80, false);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTShift(void)
{
    static const int16 k[TBaseSize] = {50, 20, 10, 5, 2};
    int16 tShift = TSHIFT - sTime_TShiftMin() + 1;
    int16 tShiftOld = tShift;
    TBase tBase = SET_TBASE;
    if (tBase < TBase_100ns)
    {
        tShift = tShift / k[tBase] + deltaTShift[tBase];
    }
    int additionShift = (tShiftOld % k[tBase]) * 2;
    FPGA::SetAdditionShift(additionShift);
    uint16 post = (uint16)tShift;
    post = (uint16)(~post);
    WriteToHardware(WR_POST_LOW, (uint8)post, true);
    WriteToHardware(WR_POST_HI, (uint8)(post >> 8), true);
    uint16 pred = (uint16)((tShift > 511) ? 1023 : (511 - post));
    pred = (uint16)((~(pred - 1)) & 0x1ff);
    WriteToHardware(WR_PRED_LOW, (uint8)pred, true);
    WriteToHardware(WR_PRED_HI, (uint8)(pred >> 8), true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *FPGA::GetTShiftString(int16 tShiftRel, char buffer[20])
{
    float tShiftVal = TSHIFT_2_ABS(tShiftRel, SET_TBASE);
    return Time2String(tShiftVal, true, buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::RangeIncrease(Channel chan)
{
    bool retValue = false;
    if (SET_RANGE(chan) < RangeSize - 1)
    {
        SetRange(chan, (Range)(SET_RANGE(chan) + 1));
        retValue = true;
    }
    else
    {
       Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::RangeDecrease(Channel chan)
{
    bool retValue = false;
    if (SET_RANGE(chan) > 0)
    {
        SetRange(chan, (Range)(SET_RANGE(chan) - 1));
        retValue = true;
    }
    else
    {
        Display::ShowWarningBad(chan == A ? LimitChan1_Volts : LimitChan2_Volts);
    }
    Display::Redraw();
    return retValue;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigSource(TrigSource trigSource)
{
    TRIG_SOURCE = trigSource;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
    if (!TRIG_SOURCE_IS_EXT)
    {
        SetTrigLev(TRIG_SOURCE, TRIG_LEVEL_SOURCE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigPolarity(TrigPolarity polarity)
{
    TRIG_POLARITY = polarity;
    LoadTrigPolarity();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadTrigPolarity(void)
{
    WriteToHardware(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01U : 0x00U, true);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetTrigInput(TrigInput trigInput)
{
    TRIG_INPUT = trigInput;
    SetAttribChannelsAndTrig(TypeWriteAnalog_TrigParam);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetModeCouple(Channel chan, ModeCouple modeCoupe)
{
    SET_COUPLE(chan) = modeCoupe;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParam0 : TypeWriteAnalog_ChanParam1);
    SetRShift(chan, SET_RSHIFT(chan));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::EnableChannelFiltr(Channel chan, bool enable)
{
    SET_FILTR(chan) = enable;
    SetAttribChannelsAndTrig(chan == A ? TypeWriteAnalog_ChanParam0 : TypeWriteAnalog_ChanParam1);
}
