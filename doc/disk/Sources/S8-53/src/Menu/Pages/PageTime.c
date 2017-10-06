#include "defines.h"
#include "Settings/Settings.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "FPGA/FPGA_Types.h"
#include "FPGA/FPGA.h"
#include "Log.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Hardware/FSMC.h"
#include "Display/Grid.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageTime
 *  @{
 */


static const Choice mcSample;           // РАЗВЕРТКА - Выборка
static bool  IsActive_Sample(void);
static const Choice mcPeakDet;          // РАЗВЕРТКА - Пик дет
static bool  IsActive_PeakDet(void);
void        OnChanged_PeakDet(bool active);
static const Choice mcTPos;             // РАЗВЕРТКА - To
void        OnChanged_TPos(bool active);
static const Choice mcSelfRecorder;     // РАЗВЕРТКА - Самописец
static bool  IsActive_SelfRecorder(void);
static const Choice mcDivRole;          // РАЗВЕРТКА - Ф-ция ВР/ДЕЛ


extern Page mainPage;


// РАЗВЕРТКА //////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpTime =      ///< РАЗВЕРТКА
{
    Item_Page, &mainPage, 0,
    {
        "РАЗВЕРТКА", "SCAN",
        "Содержит настройки развёртки.",
        "Contains scan settings."
    },
    Page_Time,
    {
        (void*)&mcSample,       // РАЗВЕРТКА - Выборка
        (void*)&mcPeakDet,      // РАЗВЕРТКА - Пик дет
        (void*)&mcTPos,         // РАЗВЕРТКА - To
        (void*)&mcSelfRecorder, // РАЗВЕРТКА - Самописец
        (void*)&mcDivRole       // РАЗВЕРТКА - Ф-ция ВР/ДЕЛ
    }
};

// РАЗВЕРТКА - Выборка -------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSample =
{
    Item_Choice, &mpTime, IsActive_Sample,
    {
        "Выборка", "Sampling"
        ,
        "\"Реальная\" - \n"
        "\"Эквивалентная\" -"
        ,
        "\"Real\" - \n"
        "\"Equals\" - "
    },
    {
        {"Реальное время",  "Real"},
        {"Эквивалентная",   "Equals"}
    },
    (int8*)&SAMPLE_TYPE
};

static bool IsActive_Sample(void)
{
    return sTime_RandomizeModeEnabled();
}

// РАЗВЕРТКА - Пик дет -------------------------------------------------------------------------------------------------------------------------------
static const Choice mcPeakDet =
{
    Item_Choice, &mpTime, IsActive_PeakDet,
    {
        "Пик дет", "Pic deat",
        "Включает/выключает пиковый детектор.",
        "Turns on/off peak detector."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
        /* , {"Среднее",   "Average"} */
    },
    (int8*)&PEAKDET, OnChanged_PeakDet
};

static bool IsActive_PeakDet(void)
{
    return (TBASE >= MIN_TBASE_PEC_DEAT);
}

void OnChanged_PeakDet(bool active)
{
    if (active)
    {
        FPGA_SetPeackDetMode(PEAKDET);
        FPGA_SetTBase(TBASE);
        if (PEAKDET_IS_DISABLE)
        {
            int8 shift[2][3] =
            {
                {0, BALANCE_SHIFT_ADC_A, BALANCE_ADC_A},
                {0, BALANCE_SHIFT_ADC_B, BALANCE_ADC_B}
            };

            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, shift[0][BALANCE_ADC_TYPE], false);
            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, shift[1][BALANCE_ADC_TYPE], false);
        }
        else
        {
            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, 3, false);     // Почему-то при пиковом детекторе смещение появляется. Вот его и компенсируем.
            FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, 3, false);
        }
        if (PEAKDET_IS_DISABLE)
        {
            int centerX = SHIFT_IN_MEMORY + GridWidth() / 2;
            SHIFT_IN_MEMORY = centerX * 2 - GridWidth() / 2;
            ENUM_POINTS = set.time.oldNumPoints;
            ChangeC_Memory_NumPoints(true);
        }
        else if (PEAKDET_IS_ENABLE)
        {
            int centerX = SHIFT_IN_MEMORY + GridWidth() / 2;
            LIMITATION(SHIFT_IN_MEMORY, centerX / 2 - GridWidth() / 2, 0, sMemory_GetNumPoints(false) - GridWidth());
            ChangeC_Memory_NumPoints(true);
        }
    }
    else
    {
        Display_ShowWarningBad(TooSmallSweepForPeakDet);
    }
}

// РАЗВЕРТКА - To ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTPos =
{
    Item_Choice, &mpTime, 0,
    {
        "\x7b", "\x7b",
        "Задаёт точку привязки нулевого смещения по времени к экрану - левый край, центр, правый край.",
        "Sets the anchor point nuleovgo time offset to the screen - the left edge, center, right edge."
    },
    {
        {"Лево",    "Left"},
        {"Центр",   "Center"},
        {"Право",   "Right"}
    },
    (int8*)&TPOS, OnChanged_TPos
};

void OnChanged_TPos(bool active)
{
    ChangeC_Memory_NumPoints(active);
    FPGA_SetTShift(TSHIFT);
}

// РАЗВЕРТКА - Самописец -----------------------------------------------------------------------------------------------------------------------------
static const Choice mcSelfRecorder =
{
    Item_Choice, &mpTime, IsActive_SelfRecorder,
    {
        "Самописец", "Self-Recorder",
        "Включает/выключает режим самописца. Этот режим доступен на развёртках 20мс/дел и более медленных.",
        "Turn on/off the recorder. This mode is available for scanning 20ms/div and slower."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SELFRECORDER
};

static bool IsActive_SelfRecorder(void)
{
    return TBASE >= MIN_TBASE_P2P;
}

// РАЗВЕРТКА - Ф-ция ВР/ДЕЛ --------------------------------------------------------------------------------------------------------------------------
static const Choice mcDivRole =
{
    Item_Choice, &mpTime, 0,
    {
        "Ф-ция ВР/ДЕЛ", "Func Time/DIV"
        ,
        "Задаёт функцию для ручки ВРЕМЯ/ДЕЛ: в режиме сбора информации (ПУСК/СТОП в положении ПУСК):\n"
        "1. \"Время\" - изменение смещения по времени.\n"
        "2. \"Память\" - перемещение по памяти."
        ,
        "Sets the function to handle TIME/DIV: mode of collecting information (START/STOP to start position):\n"
        "1. \"Time\" - change the time shift.\n"
        "2. \"Memory\" - moving from memory."
    },
    {
        {"Время",   "Time"},
        {"Память",  "Memory"}
    },
    (int8*)&TIME_DIV_XPOS
};


/** @}  @}
 */
