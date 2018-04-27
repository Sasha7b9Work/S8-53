#include "defines.h"
#include "Log.h"
#include "SCPI.h"
#include "Settings/Settings.h"
#include "Utils/Map.h"
#include "Utils/GlobalFunctions.h"
#include "VCP/VCP.h"
#include "FPGA/FPGA.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Channel chan = A;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENTER_PARSE_FUNC(SCPI::ProcessCHANNEL)
        {"INPUT",       SCPI::CHANNEL::INPUT},
        {"COUPLING",    SCPI::CHANNEL::COUPLE},
        {"COUPL",       SCPI::CHANNEL::COUPLE},
        {"FILTR",       SCPI::CHANNEL::FILTR},
        {"SET_INVERSE", SCPI::CHANNEL::INVERSE},
        {"INV",         SCPI::CHANNEL::INVERSE},
        {"SET_RANGE",   SCPI::CHANNEL::RANGE},
        {"OFFSET",      SCPI::CHANNEL::OFFSET},
        {"FACTOR",      SCPI::CHANNEL::FACTOR},
        {"FACT",        SCPI::CHANNEL::FACTOR},
        {0}
    };

    chan = (char)(*(buffer - 2)) == '1' ? A : B;

    SCPI::ProcessingCommand(commands, buffer);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::INPUT(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_ENABLED(chan) = true; }
        else if (1 == value)    { SET_ENABLED(chan) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:INPUT %s", Tables_GetNumChannel(chan), sChannel_Enabled(chan) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnChanged_CoupleA(bool);
extern void OnChanged_CoupleB(bool);


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::COUPLE(uint8 *buffer)
{
    static const pFuncVB func[2] = {OnChanged_CoupleA, OnChanged_CoupleB};

    static const MapElement map[] = 
    {
        {"DC",  0},
        {"AC",  1},
        {"GND", 2},
        {"?",   3},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_COUPLE(chan) = ModeCouple_DC; func[chan](true); }
        else if (1 == value)    { SET_COUPLE(chan) = ModeCouple_AC; func[chan](true); }
        else if (2 == value)    { SET_COUPLE(chan) = ModeCouple_GND; func[chan](true); }
        else if (3 == value)
        {
            SCPI_SEND(":CHANNEL%d:COUPLING %s", Tables_GetNumChannel(chan), map[SET_COUPLE(chan)].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
extern void OnChanged_FiltrA(bool activate);
extern void OnChanged_FiltrB(bool activate);


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::FILTR(uint8 *buffer)
{
    static const pFuncVB func[2] = {OnChanged_FiltrA, OnChanged_FiltrB};

    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_FILTR(chan) = true; func[chan](true); }
        else if (1 == value)    { SET_FILTR(chan) = false; func[chan](true); }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:FILTR %s", Tables_GetNumChannel(chan), SET_FILTR(chan) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::INVERSE(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"ON",  0},
        {"OFF", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (0 == value)         { SET_INVERSE(chan) = true; }
        else if (1 == value)    { SET_INVERSE(chan) = false; }
        else if (2 == value)
        {
            SCPI_SEND(":CHANNEL%d:SET_INVERSE %s", Tables_GetNumChannel(chan), SET_INVERSE(chan) ? "ON" : "OFF");
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::RANGE(uint8 *buffer)
{
    static const MapElement map[] = 
    {
        {"2MV",   (uint8)Range_2mV},
        {"5MV",   (uint8)Range_5mV},
        {"10MV",  (uint8)Range_10mV},
        {"20MV",  (uint8)Range_20mV},
        {"50MV",  (uint8)Range_50mV},
        {"100MV", (uint8)Range_100mV},
        {"200MV", (uint8)Range_200mV},
        {"500MV", (uint8)Range_500mV},
        {"1V",    (uint8)Range_1V},
        {"2V",    (uint8)Range_2V},
        {"5V",    (uint8)Range_5V},
        {"10V",   (uint8)Range_10V},
        {"20V",   (uint8)Range_20V},
        {"?",     (uint8)RangeSize},
        {0}
    };
    ENTER_ANALYSIS
        if (value <= (uint8)Range_20V)      { FPGA::SetRange(chan, (Range)value); }
        else if (value == (uint8)RangeSize)
        {
            SCPI_SEND(":CHANNEL%d:SET_RANGE %s", Tables_GetNumChannel(chan), map[SET_RANGE(chan)].key);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::OFFSET(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"?", 0},
        {0}
    };
    int intVal = 0;
    if (SCPI::FirstIsInt(buffer, &intVal, -240, 240))
    {
        int rShift = RShiftZero + 2 * intVal;
        FPGA::SetRShift(chan, (int16)rShift);
        return;
    }
    ENTER_ANALYSIS
        if (value == 0)
        {
            int retValue = 0.5f * (SET_RSHIFT(chan) - RShiftZero);
            SCPI_SEND(":CHANNNEL%d:OFFSET %d", Tables_GetNumChannel(chan), retValue);
        }
    LEAVE_ANALYSIS
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI::CHANNEL::FACTOR(uint8 *buffer)
{
    static const MapElement map[] =
    {
        {"X1",  0},
        {"X10", 1},
        {"?",   2},
        {0}
    };
    ENTER_ANALYSIS
        if (value == 0)         { SET_DIVIDER(chan) = Divider_1; }
        else if (value == 1)    { SET_DIVIDER(chan) = Divider_10; }
        else if (value == 2)
        {
            SCPI_SEND(":CHANNEL%d:PROBE %s", Tables_GetNumChannel(chan), map[SET_DIVIDER(chan)].key);
        }
    LEAVE_ANALYSIS
}
