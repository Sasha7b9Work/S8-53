#include "defines.h"
#include "Definition.h"
#include "FlashDrive/FlashDrive.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"
#include "Hardware/FSMC.h"
#include "Hardware/Sound.h"
#include "Hardware/EPROM.h"
#include "Menu/Menu.h"
#include "Menu/Pages/PageDebug.h"
#include "Log.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageDebug
 *  @{
 */

extern const Page pDebug;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Choice     mcStats;                            ///< ОТЛАДКА - Статистика
extern const Page       mpConsole;                          ///< ОТЛАДКА - КОНСОЛЬ
extern const Governor   mgConsole_NumStrings;               ///< ОТЛАДКА - КОНСОЛЬ - Число строк
extern const Choice     mcConsole_SizeFont;                 ///< ОТЛАДКА - КОНСОЛЬ - Размер шрифта
extern const Page       mpConsole_Registers;                ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
extern const Choice     mcConsole_Registers_ShowAll;        ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
extern const Choice     mcConsole_Registers_RD_FL;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
static bool      IsActive_Console_Registers_RD_FL();
extern const Choice     mcConsole_Registers_RShiftA;        ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
extern const Choice     mcConsole_Registers_RShiftB;        ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
extern const Choice     mcConsole_Registers_TrigLev;        ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр.
extern const Choice     mcConsole_Registers_RangeA;         ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
extern const Choice     mcConsole_Registers_RangeB;         ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
extern const Choice     mcConsole_Registers_TrigParam;      ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
extern const Choice     mcConsole_Registers_ChanParamA;     ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
extern const Choice     mcConsole_Registers_ChanParamB;     ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
extern const Choice     mcConsole_Registers_TBase;          ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
extern const Choice     mcConsole_Registers_tShift;         ///< ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
extern const Page       mpADC;                              ///< ОТЛАДКА - АЦП
extern const Page       mpADC_Balance;                      ///< ОТЛАДКА - АЦП - БАЛАНС
extern const Choice     mcADC_Balance_Mode;                 ///< ОТЛАДКА - АЦП - БАЛАНС - Режим
static void     OnChanged_ADC_Balance_Mode(bool active);
static void          Draw_ADC_Balance_Mode(int x, int y);
extern const Governor   mgADC_Balance_ShiftA;               ///< ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
static bool      IsActive_ADC_Balance_Shift();
static void     OnChanged_ADC_Balance_ShiftA();
extern const Governor   mgADC_Balance_ShiftB;               ///< ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
static void     OnChanged_ADC_Balance_ShiftB();
extern const Page       mpADC_Stretch;                      ///< ОТЛАДКА - АЦП - РАСТЯЖКА
extern const Choice     mcADC_Stretch_Mode;                 ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
static void     OnChanged_ADC_Stretch_Mode(bool active);
void LoadStretchADC(Channel chan);
extern const Governor   mgADC_Stretch_ADC_A;                ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 1к
static bool      IsActive_ADC_Stretch_ADC();
static void     OnChanged_ADC_Stretch_ADC_A();
extern const Governor   mgADC_Stretch_ADC_B;                ///< ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 2к
static void     OnChanged_ADC_Stretch_ADC_B();
extern const Page       mpADC_AltRShift;                    ///< ОТЛАДКА - АЦП - ДОП СМЕЩ
extern const Button     mbADC_AltRShift_Reset;              ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
static void       OnPress_ADC_AltRShift_Reset();
extern const Governor   mbADC_AltRShift_2mV_DC_A;           ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
static void     OnChanged_ADC_AltRShift_A();
extern const Governor   mbADC_AltRShift_2mV_DC_B;           ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
static void     OnChanged_ADC_AltRShift_B();
extern const Governor   mbADC_AltRShift_5mV_DC_A;           ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
extern const Governor   mbADC_AltRShift_5mV_DC_B;           ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
extern const Governor   mbADC_AltRShift_10mV_DC_A;          ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
extern const Governor   mbADC_AltRShift_10mV_DC_B;          ///< ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост
extern const Page       mpRandomizer;                       ///< ОТЛАДКА - РАНД-ТОР
extern const Governor   mgRandomizer_SamplesForGates;       ///< ОТЛАДКА - РАНД-ТОР - Выб-к/ворота
static void     OnChanged_Randomizer_SamplesForGates();
extern const Governor   mgRandomizer_AltTShift0;            ///< ОТЛАДКА - РАНД-ТОР - tShift доп.
static void     OnChanged_Randomizer_AltTShift0();
extern const Governor   mgRandomizer_Average;               ///< ОТЛАДКА - РАНД-ТОР - Усредн.
extern const Choice     mcSizeSettings;                     ///< ОТЛАДКА - Размер настроек
static void        OnDraw_SizeSettings(int x, int y);
extern const Button     mbSaveFirmware;                     ///< ОТЛАДКА - Сохр. прошивку
static bool      IsActive_SaveFirmware();
static void       OnPress_SaveFirmware();
extern const       Page ppSerialNumber;                     ///< ОТЛАДКА - С/Н
static void       OnPress_SerialNumber();
static void Draw_EnterSerialNumber();
static void      OnRegSet_SerialNumber(int);
extern const SmallButton bSerialNumber_Exit;                ///< ОТЛАДКА - С/Н - Выход
static void       OnPress_SerialNumber_Exit();
extern const SmallButton bSerialNumber_Change;              ///< ОТЛАДКА - С/Н - Перейти
static void       OnPress_SerialNumber_Change();
static void          Draw_SerialNumber_Change(int, int);
extern const  SmallButton bSerialNumber_Save;               ///< ОТЛАДКА - С/Н - Сохранить
static void        OnPress_SerialNumber_Save();
static void           Draw_SerialNumber_Save(int, int);
extern const       Button bEraseData;                       ///< ОТЛАДКА - Стереть данные
static void       OnPress_EraseData();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// В этой структуре будут храниться данные серийного номера при открытой странице ppSerialNumer
typedef struct
{
    int number;     ///< Соответственно, порядковый номер.
    int year;       ///< Соответственно, год.
    int curDigt;    ///< Соответственно, номером (0) или годом (1) управляет ручка УСТАНОВКА.
} StructForSN;


extern Page mainPage;

void *PageDebug::SerialNumber::GetPointer()
{
    return (void *)&ppSerialNumber;
}


// ОТЛАДКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsDebug =
{
    (void *)&mcStats,            // ОТЛАДКА - Статистика
    (void *)&mpConsole,          // ОТЛАДКА - КОНСОЛЬ
    (void *)&mpADC,              // ОТЛАДКА - АЦП
    (void *)&mpRandomizer,       // ОТЛАДКА - РАНД-ТОР
    (void *)&mcSizeSettings,     // ОТЛАДКА - Размер настроек
    (void *)&mbSaveFirmware,     // ОТЛАДКА - Сохр. прошивку
    (void *)&bEraseData          // ОТЛАДКА - Стереть данные
    //(void*)&ppSerialNumber     // ОТЛАДКА - С/Н
    //(void*)&mspDebugInformation
};

const Page pDebug
(
    &mainPage, 0,
    "ОТЛАДКА", "DEBUG",
    "",
    "",
    Page_Debug, &itemsDebug
);

// ОТЛАДКА - Статистика ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcStats =
{
    Item_Choice, &pDebug, 0,
    {
        "Статистика", "Statistics",
        "Показывать/не показывать время/кадр, кадров в секунду, количество сигналов с последними настройками в памяти/количество сохраняемых в памяти сигналов",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        "Не показывать",    "Hide",
        "Показывать",       "Show"
    },
    (int8*)&SHOW_STATS
};


// ОТЛАДКА - КОНСОЛЬ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsConsole =
{
    (void*)&mgConsole_NumStrings,   // ОТЛАДКА - КОНСОЛЬ - Число строк
    (void*)&mcConsole_SizeFont,     // ОТЛАДКА - КОНСОЛЬ - Размер шрифта
    (void*)&mpConsole_Registers     // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ
};

static const Page mpConsole
(
    &pDebug, 0,
    "КОНСОЛЬ", "CONSOLE",
    "",
    "",
    Page_DebugConsole, &itemsConsole
);

// ОТЛАДКА - КОНСОЛЬ - Число строк -------------------------------------------------------------------------------------------------------------------
static const Governor mgConsole_NumStrings
(
    &mpConsole, 0,
    "Число строк", "Number strings",
    "",
    "",
    &NUM_STRINGS,  0, 33
);

// ОТЛАДКА - КОНСОЛЬ - Размер шрифта -----------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_SizeFont =
{
    Item_Choice, &mpConsole, 0,
    {
        "Размер шрифта", "Size font",
        "",
        ""
    },
    {
        "5",    "5",
        "8",    "8"
    },
    (int8*)&SIZE_FONT_CONSOLE
};


// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsConsole_Registers =
{
    (void*)&mcConsole_Registers_ShowAll,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все
    (void*)&mcConsole_Registers_RD_FL,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL
    (void*)&mcConsole_Registers_RShiftA,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к
    (void*)&mcConsole_Registers_RShiftB,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к
    (void*)&mcConsole_Registers_TrigLev,    // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр.
    (void*)&mcConsole_Registers_RangeA,     // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1
    (void*)&mcConsole_Registers_RangeB,     // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2
    (void*)&mcConsole_Registers_TrigParam,  // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр.
    (void*)&mcConsole_Registers_ChanParamA, // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1
    (void*)&mcConsole_Registers_ChanParamB, // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2
    (void*)&mcConsole_Registers_TBase,      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ
    (void*)&mcConsole_Registers_tShift      // ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см.
};

static const Page mpConsole_Registers
(
    &mpConsole, 0,
    "РЕГИСТРЫ", "REGISTERS",
    "",
    "",
    Page_DebugShowRegisters, &itemsConsole_Registers
);

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Показывать все -----------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ShowAll =
{
    Item_Choice, &mpConsole_Registers, 0,
    {
        "Показывать все", "Show all",
        "Показывать все значения, засылаемые в регистры",
        "To show all values transferred in registers"
    },
    {
        "Нет",  "No",
        "Да",   "Yes"
    },
    (int8*)&IS_SHOW_REGISTERS_ALL
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - RD_FL --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RD_FL =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "RD_FL", "RD_FL",
        "",
        ""
    },
    {
        DISABLE_RU, DISABLE_EN,
        ENABLE_RU,  ENABLE_EN
    },
    (int8*)&set.debug.showRegisters.flag
};

static bool IsActive_Console_Registers_RD_FL(void)
{
    return !IS_SHOW_REGISTERS_ALL;
}

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 1к -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U см. 1к", "U shift 1ch",
        "",
        ""
    },
    {
        DISABLE_RU, DISABLE_EN,
        ENABLE_RU,  ENABLE_EN
    },
    (int8*)&set.debug.showRegisters.rShiftA
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U см. 2к -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U см. 2к", "U shift 2ch",
        "",
        ""
    },
    {
        DISABLE_RU, DISABLE_EN,
        ENABLE_RU,  ENABLE_EN
    },
    (int8*)&set.debug.showRegisters.rShiftB
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - U синхр. -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigLev =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U синхр.", "U trig.",
        "",
        ""
    },
    {
        DISABLE_RU, DISABLE_EN,
        ENABLE_RU,  ENABLE_EN
    },
    (int8*)&set.debug.showRegisters.trigLev
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 1 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ВОЛЬТ/ДЕЛ 1", "Range 1",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.range[0]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВОЛЬТ/ДЕЛ 2 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ВОЛЬТ/ДЕЛ 2", "Range 2",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.range[1]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. синхр. ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigParam =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Парам. синхр.", "Trig param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.trigParam
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 1 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Парам. кан. 1",  "Chan 1 param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.chanParam[0]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Парам. кан. 2 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Парам. кан. 2", "Chan 2 param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.chanParam[1]
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - ВРЕМЯ/ДЕЛ ----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TBase =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ВРЕМЯ/ДЕЛ", "TBase",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.tBase
};

// ОТЛАДКА - КОНСОЛЬ - РЕГИСТРЫ - Т см. --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_tShift =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Т см.", "tShift",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.tShift
};


// ОТЛАДКА - АЦП /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsADC =
{
    (void*)&mpADC_Balance,      // ОТЛАДКА - АЦП - БАЛАНС
    (void*)&mpADC_Stretch,      // ОТЛАДКА - АЦП - РАСТЯЖКА
    (void*)&mpADC_AltRShift     // ОТЛАДКА - АЦП - ДОП СМЕЩ
    //, (void*)&mspDebugADCaltShift
};

static const Page mpADC
(
    &pDebug, 0,
    "АЦП", "ADC",
    "",
    "",
    Page_DebugADC, &itemsADC
);

// ОТЛАДКА - АЦП - БАЛАНС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsADC_Balance =
{
    (void*)&mcADC_Balance_Mode,     // ОТЛАДКА - АЦП - БАЛАНС - Режим
    (void*)&mgADC_Balance_ShiftA,   // ОТЛАДКА - АЦП - БАЛАНС - Смещение 1
    (void*)&mgADC_Balance_ShiftB    // ОТЛАДКА - АЦП - БАЛАНС - Смещение 2
};

static const Page mpADC_Balance
(
    &mpADC, 0,
    "БАЛАНС", "BALANCE",
    "",
    "",
    Page_DebugADCbalance, &itemsADC_Balance
);

// ОТЛАДКА - АЦП - БАЛАНС - Режим --------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Balance_Mode =
{
    Item_Choice, &mpADC_Balance, 0,
    {
        "Режим", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"Реальный",    "Real"},
        {"Ручной",      "Manual"}
    },
    (int8*)&BALANCE_ADC_TYPE, OnChanged_ADC_Balance_Mode, Draw_ADC_Balance_Mode
};

static int16 shiftADCA;
static int16 shiftADCB;

static void OnChanged_ADC_Balance_Mode(bool active)
{
    Draw_ADC_Balance_Mode(0, 0);

    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)shiftADCA, false);
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)shiftADCB, false);
}

static void Draw_ADC_Balance_Mode(int x, int y)
{
    int8 shift[2][3] =
    {
        {0, SET_BALANCE_ADC_A, BALANCE_ADC_A},
        {0, SET_BALANCE_ADC_B, BALANCE_ADC_B}
    };

    shiftADCA = shift[0][BALANCE_ADC_TYPE];
    shiftADCB = shift[1][BALANCE_ADC_TYPE];
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 1 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftA
(
    &mpADC_Balance, IsActive_ADC_Balance_Shift,
    "Смещение 1", "Offset 1",
    "",
    "",
    &shiftADCA, -125, 125, OnChanged_ADC_Balance_ShiftA
);

static void OnChanged_ADC_Balance_ShiftA(void)
{
    BALANCE_ADC_A = shiftADCA;
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC1, (uint8)BALANCE_ADC_A, false);
}

static bool IsActive_ADC_Balance_Shift(void)
{
    return BALANCE_ADC_TYPE_IS_HAND;
}

// ОТЛАДКА - АЦП - БАЛАНС - Смещение 2----------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftB
(
    &mpADC_Balance, IsActive_ADC_Balance_Shift,
    "Смещение 2", "Offset 2",
    "",
    "",
    &shiftADCB, -125, 125, OnChanged_ADC_Balance_ShiftB
);

static void OnChanged_ADC_Balance_ShiftB(void)
{
    BALANCE_ADC_B = shiftADCB;
    FPGA::WriteToHardware(WR_ADD_RSHIFT_DAC2, (uint8)BALANCE_ADC_B, false);
}


// ОТЛАДКА - АЦП - РАСТЯЖКА //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsADC_Stretch =
{
    (void*)&mcADC_Stretch_Mode,     // ОТЛАДКА - АЦП - РАСТЯЖКА - Режим
    (void*)&mgADC_Stretch_ADC_A,    // ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 1к
    (void*)&mgADC_Stretch_ADC_B     // ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 2к    
};

static const Page mpADC_Stretch
(
    &mpADC, 0,
    "РАСТЯЖКА", "STRETCH",
    "",
    "",
    Page_DebugADCstretch, &itemsADC_Stretch
);

// ОТЛАДКА - АЦП - РАСТЯЖКА - Режим ------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Stretch_Mode =
{
    Item_Choice, &mpADC_Stretch, 0,
    {
        "Режим", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"Реальный",    "Real"},
        {"Ручной",      "Manual"}
    },
    (int8*)&DEBUG_STRETCH_ADC_TYPE, OnChanged_ADC_Stretch_Mode
};

static void OnChanged_ADC_Stretch_Mode(bool active)
{
    if(active)
    {
        LoadStretchADC(A);
        LoadStretchADC(B);
    }
}

void LoadStretchADC(Channel chan)
{
    if(DEBUG_STRETCH_ADC_TYPE_IS_DISABLED)
    {
        FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, 0x80, true);
    }
    else if(DEBUG_STRETCH_ADC_TYPE_IS_HAND)
    {
        FPGA::WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, (uint8)DEBUG_STRETCH_ADC(chan), true);
    }
    else if(DEBUG_STRETCH_ADC_TYPE_IS_SETTINGS)
    {
        FPGA::LoadKoeffCalibration(chan);
    }
}

// ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 1к --------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_ADC_A
(
    &mpADC_Stretch, IsActive_ADC_Stretch_ADC, 
    "Коэфф. 1к", "Koeff. 1ch",
    "",
    "",
    &DEBUG_STRETCH_ADC_A, 0, 255, OnChanged_ADC_Stretch_ADC_A
);

static bool IsActive_ADC_Stretch_ADC(void)
{
    return DEBUG_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Stretch_ADC_A(void)
{
    FPGA::WriteToHardware(WR_CAL_A, (uint8)DEBUG_STRETCH_ADC_A, true);
}


// ОТЛАДКА - АЦП - РАСТЯЖКА - Коэфф. 2к --------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_ADC_B
(
    &mpADC_Stretch, IsActive_ADC_Stretch_ADC,
    "Коэфф. 2к", "Koeff. 2ch",
    "",
    "",
    &DEBUG_STRETCH_ADC_B, 0, 255, OnChanged_ADC_Stretch_ADC_B
);

static void OnChanged_ADC_Stretch_ADC_B(void)
{
    FPGA::WriteToHardware(WR_CAL_B, (uint8)DEBUG_STRETCH_ADC_B, true);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsADC_AltRShift =
{
    (void*)&mbADC_AltRShift_Reset,          // ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс
    (void*)&mbADC_AltRShift_2mV_DC_A,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост
    (void*)&mbADC_AltRShift_2mV_DC_B,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост
    (void*)&mbADC_AltRShift_5mV_DC_A,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост
    (void*)&mbADC_AltRShift_5mV_DC_B,       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост
    (void*)&mbADC_AltRShift_10mV_DC_A,      // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост
    (void*)&mbADC_AltRShift_10mV_DC_B       // ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост    
};

static const Page mpADC_AltRShift
(
    &mpADC, 0,
    "ДОП СМЕЩ", "ADD RSHFIT",
    "",
    "",
    Page_DebugADCrShift, &itemsADC_AltRShift
);

// ОТЛАДКА - АЦП - ДОП СМЕЩ - Сброс ------------------------------------------------------------------------------------------------------------------
static const Button mbADC_AltRShift_Reset
(
    &mpADC_AltRShift, 0,
    "Сброс", "Reset",
    "", "",
    OnPress_ADC_AltRShift_Reset
);

static void OnPress_ADC_AltRShift_Reset(void)
{
    for(int chan = 0; chan < 2; chan++)
    {
        for(int mode = 0; mode < 2; mode++)
        {
            for(int range = 0; range < RangeSize; range++)
            {
                RSHIFT_ADD(chan, range, mode) = 0;
            }
        }
    }
    FPGA::SetRShift(A, SET_RSHIFT_A);
    FPGA::SetRShift(B, SET_RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_2mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 2мВ пост", "Shift 1ch 2mV DC",
    "",
    "",
    &RSHIFT_ADD(A, Range_2mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_A
);

static void OnChanged_ADC_AltRShift_A(void)
{
    FPGA::SetRShift(A, SET_RSHIFT_A);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 2мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_2mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 2мВ пост", "Shift 2ch 2mV DC",
    "",
    "",
    &RSHIFT_ADD(B, Range_2mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_B
);

static void OnChanged_ADC_AltRShift_B(void)
{
    FPGA::SetRShift(B, SET_RSHIFT_B);
}

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_5mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 5мВ пост", "Shift 1ch 5mV DC",
    "",
    "",
    &RSHIFT_ADD(A, Range_5mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_A
);

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 5мВ пост ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_5mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 5мВ пост", "Shift 2ch 5mV DC",
    "",
    "",
    &RSHIFT_ADD(B, Range_5mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_B
);

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 1к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_10mV_DC_A
(
    &mpADC_AltRShift, 0,
    "См 1к 10мВ пост", "Shift 1ch 10mV DC",
    "",
    "",
    &RSHIFT_ADD(A, Range_10mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_A
);

// ОТЛАДКА - АЦП - ДОП СМЕЩ - См 2к 10мВ пост --------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_10mV_DC_B
(
    &mpADC_AltRShift, 0,
    "См 2к 10мВ пост", "Shift 2ch 10mV DC",
    "",
    "",
    &RSHIFT_ADD(B, Range_10mV, ModeCouple_DC), -100, 100, OnChanged_ADC_AltRShift_B
);


// ОТЛАДКА - РАНД-ТОР ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsRandomizer =
{
    (void*)&mgRandomizer_SamplesForGates,   // ОТЛАДКА - РАНД-ТОР - Выб-к/ворота
    (void*)&mgRandomizer_AltTShift0,        // ОТЛАДКА - РАНД-ТОР - tShift доп.
    (void*)&mgRandomizer_Average            // ОТЛАДКА - РАНД-ТОР - Усредн.
};

static const Page mpRandomizer
(
    &pDebug, 0,
    "РАНД-ТОР", "RANDOMIZER",
    "",
    "",
    Page_DebugRandomizer, &itemsRandomizer
);

// ОТЛАДКА - РАНД-ТОР - Выб-к/ворота -----------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_SamplesForGates
(
    &mpRandomizer, 0,
    "Выб-к/ворота", "Samples/gates",
    "",
    "",
    &NUM_MEAS_FOR_GATES, 1, 2500, OnChanged_Randomizer_SamplesForGates
);

static void OnChanged_Randomizer_SamplesForGates(void)
{
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}

// ОТЛАДКА - РАНД-ТОР - tShift доп. ------------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_AltTShift0
(
    &mpRandomizer, 0,
    "tShift доп.", "tShift alt.",
    "",
    "",
    &ADD_SHIFT_T0, 0, 510, OnChanged_Randomizer_AltTShift0
);

static void OnChanged_Randomizer_AltTShift0(void)
{
    FPGA::SetDeltaTShift(ADD_SHIFT_T0);
}

// ОТЛАДКА - РАНД-ТОР - Усредн. ----------------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_Average
(
    &mpRandomizer, 0,
    "Усредн.", "Average",
    "",
    "",
    &NUM_AVE_FOR_RAND, 1, 32
);


// ОТЛАДКА - Размер настроек  ------------------------------------------------------------------------------------------------------------------------
static const Choice mcSizeSettings =
{
    Item_Choice, &pDebug, 0,
    {
        "Размер настроек", "Size settings",
        "Вывод размера структуры Settings",
        "Show size of struct Settings"
    },
    {
        {"Размер", "Size"},
        {"Размер", "Size"}
    },
    0, 0, OnDraw_SizeSettings
};

static void OnDraw_SizeSettings(int x, int y)
{
    Painter::DrawFormatText(x + 5, y + 21, COLOR_BLACK, "Размер %d", sizeof(Settings));
}


// ОТЛАДКА - Сохр. прошивку --------------------------------------------------------------------------------------------------------------------------
static const Button mbSaveFirmware
(
    &pDebug, IsActive_SaveFirmware,
    "Сохр. прошивку", "Save firmware",
    "Сохранение прошивки - секторов 5, 6, 7 общим объёмом 3 х 128 кБ, где хранится программа",
    "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored",
    OnPress_SaveFirmware
);

static bool IsActive_SaveFirmware()
{
    return FLASH_DRIVE_IS_CONNECTED == 1;
}

static void OnPress_SaveFirmware()
{
    StructForWrite structForWrite;

    FlashDrive::OpenNewFileForWrite("S8-53.bin", &structForWrite);

    uint8 *address = (uint8*)0x08020000;
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FlashDrive::WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FlashDrive::CloseFile(&structForWrite);

    Display::ShowWarningGood(FirmwareSaved);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
static const Button bEraseData
(
    &pDebug, EmptyFuncBV,
    "Стереть данные", "Erase data",
    "Стирает сектора с данными",
    "Erases data sectors",
    OnPress_EraseData
);

static void OnPress_EraseData()
{
    EPROM::EraseData();
}


// ОТЛАДКА - С/Н /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsSerialNumber =
{
    (void*)&bSerialNumber_Exit,     // ОТЛАДКА - С/Н - Выход
    (void*)&bSerialNumber_Change,   // ОТЛАДКА - С/Н - Перейти
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)&bSerialNumber_Save      // ОТЛАДКА - С/Н - Сохранить    
};

static const Page ppSerialNumber
(
    &pDebug, 0,
    "С/Н", "S/N",
    "Запись серийного номера в OTP-память. ВНИМАНИЕ!!! ОТP-память - память с однократной записью.",
    "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory.",
    Page_SB_SerialNumber, &itemsSerialNumber, OnPress_SerialNumber, 0, OnRegSet_SerialNumber
);

static void OnPress_SerialNumber(void)
{
    Menu::OpenPageAndSetItCurrent(PageDebug::SerialNumber::GetPointer());
    Display::SetAddDrawFunction(Draw_EnterSerialNumber);
    MALLOC_EXTRAMEM(StructForSN, s);
    s->number = 01;
    s->year = 2017;
    s->curDigt = 0;
}

static void Draw_EnterSerialNumber(void)
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangleC(x0, y0, width, height, COLOR_FILL);
    Painter::FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, COLOR_BACK);

    int deltaX = 10;

    ACCESS_EXTRAMEM(StructForSN, s);

    bool selNumber = s->curDigt == 0;

    char buffer[20];
    snprintf(buffer, 19, "%02d", s->number);

    Color colorText = COLOR_FILL;
    Color colorBackground = COLOR_BACK;

    if (selNumber)
    {
        colorText = COLOR_FLASH_01;
        colorBackground = COLOR_FLASH_10;
    }

    int y = y0 + 50;

    Painter::SetColor(colorText);
    int x = Painter::DrawTextOnBackground(x0 + deltaX, y, buffer, colorBackground);

    colorText = COLOR_FLASH_01;
    colorBackground = COLOR_FLASH_10;

    if (selNumber)
    {
        colorText = COLOR_FILL;
        colorBackground = COLOR_BACK;
    }

    snprintf(buffer, 19, "%04d", s->year);

    Painter::SetColor(colorText);
    Painter::DrawTextOnBackground(x + 5, y, buffer, colorBackground);

    // Теперь выведем информацию об оставшемся месте в OTP-памяти для записи

    int allShots = OTP::GetSerialNumber(buffer);

    Painter::DrawFormatText(x0 + deltaX, y0 + 130, COLOR_FILL, "Текущий сохранённый номер %s", buffer[0] == 0 ? "-- ----" : buffer);

    Painter::DrawFormatText(x0 + deltaX, y0 + 100, COLOR_FILL, "Осталось места для %d попыток", allShots);
}

static void OnRegSet_SerialNumber(int angle)
{
    typedef int(*pFunc)(int*, int, int);

    pFunc p = angle > 0 ? CircleIncreaseInt : CircleDecreaseInt;

    ACCESS_EXTRAMEM(StructForSN, s);

    if (s->curDigt == 0)
    {
        p(&s->number, 1, 99);
    }
    else
    {
        p(&s->year, 2014, 2050);
    }
    Sound::GovernorChangedValue();
}

// ОТЛАДКА - С/Н - Выход -----------------------------------------------------------------------------------------------------------------------------
static const SmallButton bSerialNumber_Exit
(
    &ppSerialNumber,
    COMMON_BEGIN_SB_EXIT,
    OnPress_SerialNumber_Exit,
    DrawSB_Exit
);

static void OnPress_SerialNumber_Exit(void)
{
    Display::RemoveAddDrawFunction();
    FREE_EXTRAMEM();
}

// ОТЛАДКА - С/Н - Вставить --------------------------------------------------------------------------------------------------------------------------
static const SmallButton bSerialNumber_Change
(
    &ppSerialNumber, 0,
    "Вставить", "Insert",
    "Вставляет выбраный символ",
    "Inserts the chosen symbol",
    OnPress_SerialNumber_Change,
    Draw_SerialNumber_Change
);

static void OnPress_SerialNumber_Change(void)
{
    ACCESS_EXTRAMEM(StructForSN, s);
    ++s->curDigt;
    s->curDigt %= 2;
    Painter::ResetFlash();
}

static void Draw_SerialNumber_Change(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Painter::SetFont(TypeFont_8);
}

// ОТЛАДКА - С/Н - Сохранить -------------------------------------------------------------------------------------------------------------------------
static const SmallButton bSerialNumber_Save
(
    &ppSerialNumber, 0,
    "Сохранить", "Save",
    "Записывает серийный номер в OTP",
    "Records the serial number in OTP",
    OnPress_SerialNumber_Save,
    Draw_SerialNumber_Save
);

static void OnPress_SerialNumber_Save(void)
{
    ACCESS_EXTRAMEM(StructForSN, s);

    char stringSN[20];

    snprintf(stringSN, 19, "%02d %04d", s->number, s->year);

    if (!OTP::SaveSerialNumber(stringSN))
    {
        Display::ShowWarningBad(FullyCompletedOTP);
    }
}

static void Draw_SerialNumber_Save(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(TypeFont_8);
}



/** @}  @}
 */

        /*
        static void OnChangeRShiftADC(bool active)
        {
            FPGA::SetRShift(A, SET_RSHIFT_A);
            FPGA::SetRShift(B, SET_RSHIFT_B);
        }

        const Choice mcDebugADCrShiftMode =
        {
            Item_Choice, &mpADC_AltRShift, {"Режим",      "Mode"},
            {
                "",
                ""
            },
            EmptyFuncBV,
            {                                    {DISABLE_RU,   DISABLE_EN},
                                                 {"Реальный",   "Real"},
                                                 {"Ручной",     "Hand"}
            },
            (int8*)&set.debug.rShiftADCtype, OnChangeRShiftADC
        };

        // ОТЛАДКА - АЦП - ДОП. СМЕЩЕНИЕ - Смещение 1к 2мВ
        static TimeStructGovernor tsRShiftADC;
        static bool IsActiveRShiftADC(void)
        {
            return set.debug.rShiftADCtype == RShiftADC_Hand;
        }

        static void OnChangeRShiftADC0(void)
        {
            FPGA::SetRShift(A, SET_RSHIFT_A);
        }

        const Governor mgDebugRShiftADC00 =
        {
            &mpADC_AltRShift,
            {
                "Смещ 1к 2мВ", "Shift 1ch"
            },
            {
                "",
                ""
            },
            IsActiveRShiftADC,
            &set.debug.rShiftADC[0][0], -100, 100, &tsRShiftADC, OnChangeRShiftADC0
        };

        static TimeStructGovernor tsRShiftADC01;
        const Governor mgDebugRShiftADC01 =
        {
            &mpADC_AltRShift,
            {
                "Смещ 1к 500мВ", "Shift 1ch 500mV"
            },
            {
                "",
                ""
            },
            IsActiveRShiftADC,
            &set.debug.rShiftADC[1][0], -100, 100, &tsRShiftADC01, OnChangeRShiftADC0
        };

        static void OnChangeRShiftADC1(void)
        {
            FPGA::SetRShift(B, SET_RSHIFT_B);
        }

        // ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ.
const Page mspDebugADCaltShift;

/// ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ. - Величина
const Governor mgDebugADCaltShift =
{
    &mspDebugADCaltShift, 0,
    {
        "Величина", "Value",
        "",
        ""
    },
    &set.debug.altShift, -2, 2
};

// ОТЛАДКА - АЦП - ДОП СМЕЩ ПАМ /////////////////////////////////////////////////////////////////////////////////////////////
const Page mspDebugADCaltShift =
{
    Item_Page, &mpADC, 0,
    {
        "ДОП СМЕЩ ПАМ", "ALT SHIFT MEM",
        "",
        ""
    },
    Page_DebugADCaltShift,
    {
        (void*)&mgDebugADCaltShift
    }
};

const Governor mgDebugInfoNumber =
{
    &mspDebugInformation,
    {
        "Номер", "Number"
    },
    {
        "Установка номера серийного номера",
        "Installation numbers of the serial number"
    },
    EmptyFuncBV,
    &set.debug.infoNumber, 0, 100
};

const Governor mgDebugInfoYear =
{
    &mspDebugInformation,
    {
        "Год", "Year"
    },
    {
        "Установка года серийного номера",
        "Installation year serial number"
    },
    EmptyFuncBV,
    &set.debug.infoYear, 2015, 2050
};

const Governor mgDebugInfoVersion =
{
    &mspDebugInformation,
    {
        "Версия", "Version"
    },
    {
        "Установка номера версии",
        "Setting the version number"
    },
    EmptyFuncBV,
    &set.debug.infoVer, 10, 100
};

// ОТЛАДКА - ИНФОРМАЦИЯ ////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspDebugInformation =
{
    Item_Page, &pDebug,
    {
        "ИНФОРМАЦИЯ", "INFORMATION",
    },
    {
        "Позволяет ввести информацию для меню СЕРВИС-ИНФОРМАЦИЯ",
        "It allows you to enter information for SERVICE INFORMATION menu"
    },
    EmptyFuncBV, Page_DebugInformation,
    {
        (void*)&mgDebugInfoNumber,
        (void*)&mgDebugInfoYear,
        (void*)&mgDebugInfoVersion
    }
};
*/
