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
#include "Hardware/FLASH.h"
#include "Menu/MenuFunctions.h"
#include "Log.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageDebug
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice     mcStats;                            ///< ÎÒËÀÄÊÀ - Ñòàòèñòèêà
static const Page       mpConsole;                          ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ
static const Governor   mgConsole_NumStrings;               ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ×èñëî ñòðîê
static const Choice     mcConsole_SizeFont;                 ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - Ðàçìåð øðèôòà
static const Page       mpConsole_Registers;                ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ
static const Choice     mcConsole_Registers_ShowAll;        ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïîêàçûâàòü âñå
static const Choice     mcConsole_Registers_RD_FL;          ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - RD_FL
static bool      IsActive_Console_Registers_RD_FL(void);
static const Choice     mcConsole_Registers_RShiftA;        ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 1ê
static const Choice     mcConsole_Registers_RShiftB;        ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 2ê
static const Choice     mcConsole_Registers_TrigLev;        ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñèíõð.
static const Choice     mcConsole_Registers_RangeA;         ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 1
static const Choice     mcConsole_Registers_RangeB;         ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 2
static const Choice     mcConsole_Registers_TrigParam;      ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. ñèíõð.
static const Choice     mcConsole_Registers_ChanParamA;     ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 1
static const Choice     mcConsole_Registers_ChanParamB;     ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 2
static const Choice     mcConsole_Registers_TBase;          ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÐÅÌß/ÄÅË
static const Choice     mcConsole_Registers_tShift;         ///< ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ò ñì.
static const Page       mpADC;                              ///< ÎÒËÀÄÊÀ - ÀÖÏ
static const Page       mpADC_Balance;                      ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ
static const Choice     mcADC_Balance_Mode;                 ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ðåæèì
static void     OnChanged_ADC_Balance_Mode(bool active);
static void          Draw_ADC_Balance_Mode(int x, int y);
static const Governor   mgADC_Balance_ShiftA;               ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 1
static bool      IsActive_ADC_Balance_Shift(void);
static void     OnChanged_ADC_Balance_ShiftA(void);
static const Governor   mgADC_Balance_ShiftB;               ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 2
static void     OnChanged_ADC_Balance_ShiftB(void);
static const Page       mpADC_Stretch;                      ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ
static const Choice     mcADC_Stretch_Mode;                 ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Ðåæèì
static void     OnChanged_ADC_Stretch_Mode(bool active);
void LoadStretchADC(Channel chan);
static const Governor   mgADC_Stretch_ADC_A;                ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 1ê
static bool      IsActive_ADC_Stretch_ADC(void);
static void     OnChanged_ADC_Stretch_ADC_A(void);
static const Governor   mgADC_Stretch_ADC_B;                ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 2ê
static void     OnChanged_ADC_Stretch_ADC_B(void);
static const Page       mpADC_AltRShift;                    ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ
static const Button     mbADC_AltRShift_Reset;              ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñáðîñ
static void       OnPress_ADC_AltRShift_Reset(void);
static const Governor   mbADC_AltRShift_2mV_DC_A;           ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 2ìÂ ïîñò
static void     OnChanged_ADC_AltRShift_A(void);
static const Governor   mbADC_AltRShift_2mV_DC_B;           ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 2ìÂ ïîñò
static void     OnChanged_ADC_AltRShift_B(void);
static const Governor   mbADC_AltRShift_5mV_DC_A;           ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 5ìÂ ïîñò
static const Governor   mbADC_AltRShift_5mV_DC_B;           ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 5ìÂ ïîñò
static const Governor   mbADC_AltRShift_10mV_DC_A;          ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 10ìÂ ïîñò
static const Governor   mbADC_AltRShift_10mV_DC_B;          ///< ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 10ìÂ ïîñò
static const Page       mpRandomizer;                       ///< ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ
static const Governor   mgRandomizer_SamplesForGates;       ///< ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Âûá-ê/âîðîòà
static void     OnChanged_Randomizer_SamplesForGates(void);
static const Governor   mgRandomizer_AltTShift0;            ///< ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - tShift äîï.
static void     OnChanged_Randomizer_AltTShift0(void);
static const Governor   mgRandomizer_Average;               ///< ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Óñðåäí.
static const Choice     mcSizeSettings;                     ///< ÎÒËÀÄÊÀ - Ðàçìåð íàñòðîåê
static void        OnDraw_SizeSettings(int x, int y);
static const Button     mbSaveFirmware;                     ///< ÎÒËÀÄÊÀ - Ñîõð. ïðîøèâêó
static bool      IsActive_SaveFirmware(void);
static void       OnPress_SaveFirmware(void);
static const Page       mpSerialNumber;                     ///< ÎÒËÀÄÊÀ - Ñ/Í
static void       OnPress_SerialNumber(void);
static void          Draw_EnterSerialNumber(void);
static void      OnRegSet_SerialNumber(int);
static const SmallButton sbSerialNumber_Exit;               ///< ÎÒËÀÄÊÀ - Ñ/Í - Âûõîä
static void        OnPress_SerialNumber_Exit(void);
static const SmallButton sbSerialNumber_Delete;             ///< ÎÒËÀÄÊÀ - Ñ/Í - Óäàëèòü
static void        OnPress_SerialNumber_Delete(void);
static void           Draw_SerialNumber_Delete(int, int);
static const SmallButton sbSerialNumber_Backspace;          ///< ÎÒËÀÄÊÀ - Ñ/Í - Backspace
static void        OnPress_SerialNumber_Backspace(void);
static void           Draw_SerialNumber_Backspace(int, int);
static const SmallButton sbSerialNumber_Insert;             ///< ÎÒËÀÄÊÀ - Ñ/Í - Âñòàâèòü
static void        OnPress_SerialNumber_Insert(void);
static void           Draw_SerialNumber_Insert(int, int);
static const SmallButton sbSerialNumber_Save;               ///< ÎÒËÀÄÊÀ - Ñ/Í - Ñîõðàíèòü
static void        OnPress_SerialNumber_Save(void);
static void           Draw_SerialNumber_Save(int, int);

#define LENGTH_SN 15
static char stringSN[LENGTH_SN] = "";   ///< Çäåñü õðàíèòñÿ ñåðèéíûé íîìåð äëÿ çàïèñè â OTP.

extern Page mainPage;


// ÎÒËÀÄÊÀ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpDebug =
{
    Item_Page, &mainPage, 0,
    {
        "ÎÒËÀÄÊÀ", "DEBUG",
        "",
        ""
    },
    Page_Debug,
    {
        (void*)&mcStats,            // ÎÒËÀÄÊÀ - Ñòàòèñòèêà
        (void*)&mpConsole,          // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ
        (void*)&mpADC,              // ÎÒËÀÄÊÀ - ÀÖÏ
        (void*)&mpRandomizer,       // ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ
        (void*)&mcSizeSettings,     // ÎÒËÀÄÊÀ - Ðàçìåð íàñòðîåê
        (void*)&mbSaveFirmware,     // ÎÒËÀÄÊÀ - Ñîõð. ïðîøèâêó
        (void*)&mpSerialNumber      // ÎÒËÀÄÊÀ - Ñ/Í
        //(void*)&mspDebugInformation
    }
};

// ÎÒËÀÄÊÀ - Ñòàòèñòèêà ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcStats =
{
    Item_Choice, &mpDebug, 0,
    {
        "Ñòàòèñòèêà", "Statistics",
        "Ïîêàçûâàòü/íå ïîêàçûâàòü âðåìÿ/êàäð, êàäðîâ â ñåêóíäó, êîëè÷åñòâî ñèãíàëîâ ñ ïîñëåäíèìè íàñòðîéêàìè â ïàìÿòè/êîëè÷åñòâî ñîõðàíÿåìûõ â ïàìÿòè ñèãíàëîâ",
        "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory"
    },
    {
        {"Íå ïîêàçûâàòü",   "Hide"},
        {"Ïîêàçûâàòü",      "Show"}
    },
    (int8*)&SHOW_STATS
};


// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpConsole =
{
    Item_Page, &mpDebug, 0,
    {
        "ÊÎÍÑÎËÜ", "CONSOLE",
        "",
        ""
    },
    Page_DebugConsole,
    {
        (void*)&mgConsole_NumStrings,   // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ×èñëî ñòðîê
        (void*)&mcConsole_SizeFont,     // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - Ðàçìåð øðèôòà
        (void*)&mpConsole_Registers     // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ
    }
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ×èñëî ñòðîê -------------------------------------------------------------------------------------------------------------------
static const Governor mgConsole_NumStrings =
{
    Item_Governor, &mpConsole, 0,
    {
        "×èñëî ñòðîê", "Number strings"
        "",
        ""
    },
    &NUM_STRINGS,  0, 33
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - Ðàçìåð øðèôòà -----------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_SizeFont =
{
    Item_Choice, &mpConsole, 0,
    {
        "Ðàçìåð øðèôòà", "Size font",
        "",
        ""
    },
    {
        {"5",   "5"},
        {"8",   "8"}
    },
    (int8*)&SIZE_FONT_CONSOLE
};


// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpConsole_Registers =
{
    Item_Page, &mpConsole, 0,
    {
        "ÐÅÃÈÑÒÐÛ", "REGISTERS",
        "",
        ""
    },
    Page_DebugShowRegisters,
    {
        (void*)&mcConsole_Registers_ShowAll,    // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïîêàçûâàòü âñå
        (void*)&mcConsole_Registers_RD_FL,      // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - RD_FL
        (void*)&mcConsole_Registers_RShiftA,    // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 1ê
        (void*)&mcConsole_Registers_RShiftB,    // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 2ê
        (void*)&mcConsole_Registers_TrigLev,    // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñèíõð.
        (void*)&mcConsole_Registers_RangeA,     // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 1
        (void*)&mcConsole_Registers_RangeB,     // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 2
        (void*)&mcConsole_Registers_TrigParam,  // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. ñèíõð.
        (void*)&mcConsole_Registers_ChanParamA, // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 1
        (void*)&mcConsole_Registers_ChanParamB, // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 2
        (void*)&mcConsole_Registers_TBase,      // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÐÅÌß/ÄÅË
        (void*)&mcConsole_Registers_tShift      // ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ò ñì.
    }
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïîêàçûâàòü âñå -----------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ShowAll =
{
    Item_Choice, &mpConsole_Registers, 0,
    {
        "Ïîêàçûâàòü âñå", "Show all",
        "Ïîêàçûâàòü âñå çíà÷åíèÿ, çàñûëàåìûå â ðåãèñòðû",
        "To show all values transferred in registers"
    },
    {
        {"Íåò", "No"},
        {"Äà",  "Yes"}
    },
    (int8*)&set.debug.showRegisters.all
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - RD_FL --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RD_FL =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "RD_FL", "RD_FL",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.flag
};

static bool IsActive_Console_Registers_RD_FL(void)
{
    return !set.debug.showRegisters.all;
}

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 1ê -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U ñì. 1ê", "U shift 1ch",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.rShiftA
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñì. 2ê -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RShiftB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U ñì. 2ê", "U shift 2ch",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.rShiftB
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - U ñèíõð. -----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigLev =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "U ñèíõð.", "U trig.",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.trigLev
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 1 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ÂÎËÜÒ/ÄÅË 1", "Range 1",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.range[0]
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÎËÜÒ/ÄÅË 2 --------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_RangeB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ÂÎËÜÒ/ÄÅË 2", "Range 2",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.range[1]
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. ñèíõð. ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TrigParam =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Ïàðàì. ñèíõð.", "Trig param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.trigParam
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 1 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamA =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Ïàðàì. êàí. 1",  "Chan 1 param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.chanParam[0]
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ïàðàì. êàí. 2 ------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_ChanParamB =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Ïàðàì. êàí. 2", "Chan 2 param",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.chanParam[1]
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - ÂÐÅÌß/ÄÅË ----------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_TBase =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "ÂÐÅÌß/ÄÅË", "TBase",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.tBase
};

// ÎÒËÀÄÊÀ - ÊÎÍÑÎËÜ - ÐÅÃÈÑÒÐÛ - Ò ñì. --------------------------------------------------------------------------------------------------------------
static const Choice mcConsole_Registers_tShift =
{
    Item_Choice, &mpConsole_Registers, IsActive_Console_Registers_RD_FL,
    {
        "Ò ñì.", "tShift",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&set.debug.showRegisters.tShift
};


// ÎÒËÀÄÊÀ - ÀÖÏ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpADC =
{
    Item_Page, &mpDebug, 0,
    {
        "ÀÖÏ", "ADC",
        "",
        ""
    },
    Page_DebugADC,
    {
        (void*)&mpADC_Balance,      // ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ
        (void*)&mpADC_Stretch,      // ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ
        (void*)&mpADC_AltRShift     // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ
        //, (void*)&mspDebugADCaltShift
    }
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpADC_Balance =
{
    Item_Page, &mpADC, 0,
    {
        "ÁÀËÀÍÑ", "BALANCE",
        "",
        ""
    },
    Page_DebugADCbalance,
    {
        (void*)&mcADC_Balance_Mode,     // ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ðåæèì
        (void*)&mgADC_Balance_ShiftA,   // ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 1
        (void*)&mgADC_Balance_ShiftB    // ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 2
    }
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ðåæèì --------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Balance_Mode =
{
    Item_Choice, &mpADC_Balance, 0,
    {
        "Ðåæèì", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"Ðåàëüíûé",    "Real"},
        {"Ðó÷íîé",      "Manual"}
    },
    (int8*)&BALANCE_ADC_TYPE, OnChanged_ADC_Balance_Mode, Draw_ADC_Balance_Mode
};

static int16 shiftADCA;
static int16 shiftADCB;

static void OnChanged_ADC_Balance_Mode(bool active)
{
    Draw_ADC_Balance_Mode(0, 0);

    FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, shiftADCA, false);
    FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, shiftADCB, false);
}

static void Draw_ADC_Balance_Mode(int x, int y)
{
    int8 shift[2][3] =
    {
        {0, BALANCE_SHIFT_ADC_A, BALANCE_ADC_A},
        {0, BALANCE_SHIFT_ADC_B, BALANCE_ADC_B}
    };

    shiftADCA = shift[0][BALANCE_ADC_TYPE];
    shiftADCB = shift[1][BALANCE_ADC_TYPE];
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 1 ---------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftA =
{
    Item_Governor, &mpADC_Balance, IsActive_ADC_Balance_Shift,
    {
        "Ñìåùåíèå 1", "Offset 1",
        "",
        ""
    },
    &shiftADCA, -125, 125, OnChanged_ADC_Balance_ShiftA
};

static void OnChanged_ADC_Balance_ShiftA(void)
{
    BALANCE_ADC_A = shiftADCA;
    FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC1, BALANCE_ADC_A, false);
}

static bool IsActive_ADC_Balance_Shift(void)
{
    return BALANCE_ADC_TYPE_IS_HAND;
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÁÀËÀÍÑ - Ñìåùåíèå 2----------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Balance_ShiftB =
{
    Item_Governor, &mpADC_Balance, IsActive_ADC_Balance_Shift,
    {
        "Ñìåùåíèå 2", "Offset 2",
        "",
        ""
    },
    &shiftADCB, -125, 125, OnChanged_ADC_Balance_ShiftB
};

static void OnChanged_ADC_Balance_ShiftB(void)
{
    BALANCE_ADC_B = shiftADCB;
    FPGA_WriteToHardware(WR_ADD_RSHIFT_DAC2, BALANCE_ADC_B, false);
}


// ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpADC_Stretch =
{
    Item_Page, &mpADC, 0,
    {
        "ÐÀÑÒßÆÊÀ", "STRETCH",
        "",
        ""
    },
    Page_DebugADCstretch,
    {
        (void*)&mcADC_Stretch_Mode,     // ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Ðåæèì
        (void*)&mgADC_Stretch_ADC_A,    // ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 1ê
        (void*)&mgADC_Stretch_ADC_B     // ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 2ê
    }
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Ðåæèì ------------------------------------------------------------------------------------------------------------------
static const Choice mcADC_Stretch_Mode =
{
    Item_Choice, &mpADC_Stretch, 0,
    {
        "Ðåæèì", "Mode",
        "",
        ""
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"Ðåàëüíûé",    "Real"},
        {"Ðó÷íîé",      "Manual"}
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
        FPGA_WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, 0x80, true);
    }
    else if(DEBUG_STRETCH_ADC_TYPE_IS_HAND)
    {
        FPGA_WriteToHardware(chan == A ? WR_CAL_A : WR_CAL_B, (uint8)DEBUG_STRETCH_ADC(chan), true);
    }
    else if(DEBUG_STRETCH_ADC_TYPE_IS_SETTINGS)
    {
        FPGA_LoadKoeffCalibration(chan);
    }
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 1ê --------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_ADC_A =
{
    Item_Governor, &mpADC_Stretch, IsActive_ADC_Stretch_ADC,
    {
        "Êîýôô. 1ê", "Koeff. 1ch",
        "",
        ""
    },
    &DEBUG_STRETCH_ADC_A, 0, 255, OnChanged_ADC_Stretch_ADC_A
};

static bool IsActive_ADC_Stretch_ADC(void)
{
    return DEBUG_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Stretch_ADC_A(void)
{
    FPGA_WriteToHardware(WR_CAL_A, (uint8)DEBUG_STRETCH_ADC_A, true);
}


// ÎÒËÀÄÊÀ - ÀÖÏ - ÐÀÑÒßÆÊÀ - Êîýôô. 2ê --------------------------------------------------------------------------------------------------------------
static const Governor mgADC_Stretch_ADC_B =
{
    Item_Governor, &mpADC_Stretch, IsActive_ADC_Stretch_ADC,
    {
        "Êîýôô. 2ê", "Koeff. 2ch",
        "",
        ""
    },
    &DEBUG_STRETCH_ADC_B, 0, 255, OnChanged_ADC_Stretch_ADC_B
};

static void OnChanged_ADC_Stretch_ADC_B(void)
{
    FPGA_WriteToHardware(WR_CAL_B, (uint8)DEBUG_STRETCH_ADC_B, true);
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpADC_AltRShift =
{
    Item_Page, &mpADC, 0,
    {
        "ÄÎÏ ÑÌÅÙ", "ADD RSHFIT",
        "",
        ""
    },
    Page_DebugADCrShift,
    {
        (void*)&mbADC_AltRShift_Reset,          // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñáðîñ
        (void*)&mbADC_AltRShift_2mV_DC_A,       // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 2ìÂ ïîñò
        (void*)&mbADC_AltRShift_2mV_DC_B,       // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 2ìÂ ïîñò
        (void*)&mbADC_AltRShift_5mV_DC_A,       // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 5ìÂ ïîñò
        (void*)&mbADC_AltRShift_5mV_DC_B,       // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 5ìÂ ïîñò
        (void*)&mbADC_AltRShift_10mV_DC_A,      // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 10ìÂ ïîñò
        (void*)&mbADC_AltRShift_10mV_DC_B       // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 10ìÂ ïîñò
    }
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñáðîñ ------------------------------------------------------------------------------------------------------------------
static const Button mbADC_AltRShift_Reset =
{
    Item_Button, &mpADC_AltRShift, 0,
    {
        "Ñáðîñ", "Reset",
        "",
        ""
    },
    OnPress_ADC_AltRShift_Reset
};

static void OnPress_ADC_AltRShift_Reset(void)
{
    for(int chan = 0; chan < 2; chan++)
    {
        for(int mode = 0; mode < 2; mode++)
        {
            for(int range = 0; range < RangeSize; range++)
            {
                set.chan[chan].rShiftAdd[range][mode] = 0;
            }
        }
    }
    FPGA_SetRShift(A, RSHIFT_A);
    FPGA_SetRShift(B, RSHIFT_B);
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 2ìÂ ïîñò ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_2mV_DC_A =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 1ê 2ìÂ ïîñò", "Shift 1ch 2mV DC",
        "",
        ""
    },
    &set.chan[A].rShiftAdd[Range_2mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_A
};

static void OnChanged_ADC_AltRShift_A(void)
{
    FPGA_SetRShift(A, RSHIFT_A);
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 2ìÂ ïîñò ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_2mV_DC_B =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 2ê 2ìÂ ïîñò", "Shift 2ch 2mV DC",
        "",
        ""
    },
    &set.chan[B].rShiftAdd[Range_2mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_B
};

static void OnChanged_ADC_AltRShift_B(void)
{
    FPGA_SetRShift(B, RSHIFT_B);
}

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 5ìÂ ïîñò ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_5mV_DC_A =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 1ê 5ìÂ ïîñò", "Shift 1ch 5mV DC",
        "",
        ""
    },
    &set.chan[A].rShiftAdd[Range_5mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_A
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 5ìÂ ïîñò ---------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_5mV_DC_B =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 2ê 5ìÂ ïîñò", "Shift 2ch 5mV DC",
        "",
        ""
    },
    &set.chan[B].rShiftAdd[Range_5mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_B
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 1ê 10ìÂ ïîñò --------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_10mV_DC_A =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 1ê 10ìÂ ïîñò", "Shift 1ch 10mV DC",
        "",
        ""
    },
    &set.chan[A].rShiftAdd[Range_10mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_A
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ - Ñì 2ê 10ìÂ ïîñò --------------------------------------------------------------------------------------------------------
static const Governor mbADC_AltRShift_10mV_DC_B =
{
    Item_Governor, &mpADC_AltRShift, 0,
    {
        "Ñì 2ê 10ìÂ ïîñò", "Shift 2ch 10mV DC",
        "",
        ""
    },
    &set.chan[B].rShiftAdd[Range_10mV][ModeCouple_DC], -100, 100, OnChanged_ADC_AltRShift_B
};


// ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpRandomizer =
{
    Item_Page, &mpDebug, 0,
    {
        "ÐÀÍÄ-ÒÎÐ", "RANDOMIZER",
        "",
        ""
    },
    Page_DebugRandomizer,
    {
        (void*)&mgRandomizer_SamplesForGates,   // ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Âûá-ê/âîðîòà
        (void*)&mgRandomizer_AltTShift0,        // ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - tShift äîï.
        (void*)&mgRandomizer_Average            // ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Óñðåäí.
    }
};

// ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Âûá-ê/âîðîòà -----------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_SamplesForGates =
{
    Item_Governor, &mpRandomizer, 0,
    {
        "Âûá-ê/âîðîòà", "Samples/gates",
        "",
        ""
    },
    &NUM_MEAS_FOR_GATES, 1, 2500, OnChanged_Randomizer_SamplesForGates
};

static void OnChanged_Randomizer_SamplesForGates(void)
{
    FPGA_SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
}

// ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - tShift äîï. ------------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_AltTShift0 =
{
    Item_Governor, &mpRandomizer, 0,
    {
        "tShift äîï.", "tShift alt.",
        "",
        ""
    },
    &ADD_SHIFT_T0, 0, 510, OnChanged_Randomizer_AltTShift0
};

static void OnChanged_Randomizer_AltTShift0(void)
{
    FPGA_SetDeltaTShift(ADD_SHIFT_T0);
}

// ÎÒËÀÄÊÀ - ÐÀÍÄ-ÒÎÐ - Óñðåäí. ----------------------------------------------------------------------------------------------------------------------
static const Governor mgRandomizer_Average =
{
    Item_Governor, &mpRandomizer, 0,
    {
        "Óñðåäí.", "Average",
        "",
        ""
    },
    &NUM_AVE_FOR_RAND, 1, 32
};


// ÎÒËÀÄÊÀ - Ðàçìåð íàñòðîåê  ------------------------------------------------------------------------------------------------------------------------
static const Choice mcSizeSettings =
{
    Item_Choice, &mpDebug, 0,
    {
        "Ðàçìåð íàñòðîåê", "Size settings",
        "Âûâîä ðàçìåðà ñòðóêòóðû Settings",
        "Show size of struct Settings"
    },
    {
        {"Ðàçìåð", "Size"},
        {"Ðàçìåð", "Size"}
    },
    0, 0, OnDraw_SizeSettings
};

static void OnDraw_SizeSettings(int x, int y)
{
    Painter_DrawFormText(x + 5, y + 21, COLOR_BLACK, "Ðàçìåð %d", sizeof(Settings));
}


// ÎÒËÀÄÊÀ - Ñîõð. ïðîøèâêó --------------------------------------------------------------------------------------------------------------------------
static const Button mbSaveFirmware =
{
    Item_Button, &mpDebug, IsActive_SaveFirmware,
    {
        "Ñîõð. ïðîøèâêó", "Save firmware",
        "Ñîõðàíåíèå ïðîøèâêè - ñåêòîðîâ 5, 6, 7 îáùèì îáú¸ìîì 3 õ 128 êÁ, ãäå õðàíèòñÿ ïðîãðàììà",
        "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored"
    },
    OnPress_SaveFirmware
};

static bool IsActive_SaveFirmware(void)
{
    return gBF.flashDriveIsConnected;
}

static void OnPress_SaveFirmware(void)
{
    StructForWrite structForWrite;

    FDrive_OpenNewFileForWrite("S8-53.bin", &structForWrite);

    uint8 *address = (uint8*)0x08020000;
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FDrive_WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FDrive_CloseFile(&structForWrite);

    Display_ShowWarningGood(FirmwareSaved);
}


// ÎÒËÀÄÊÀ - Ñ/Í /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpSerialNumber =
{
    Item_Page, &mpDebug, 0,
    {
        "Ñ/Í", "S/N",
        "Çàïèñü ñåðèéíîãî íîìåðà â îäíîðàçîâî ïðîãðàììèðóåìóþ ïàìÿòü (OTP memory)",
        "Serial number recording in one-time programmable memory (OTP)"
    },
    Page_SB_SerialNumber,
    {
        (void*)&sbSerialNumber_Exit,         // ÎÒËÀÄÊÀ - Ñ/Í - Âûõîä
        (void*)&sbSerialNumber_Delete,       // ÎÒËÀÄÊÀ - Ñ/Í - Óäàëèòü
        (void*)&sbSerialNumber_Backspace,    // ÎÒËÀÄÊÀ - Ñ/Í - Backspace
        (void*)&sbSerialNumber_Insert,       // ÎÒËÀÄÊÀ - Ñ/Í - Âñòàâèòü
        (void*)0,
        (void*)&sbSerialNumber_Save          // ÎÒËÀÄÊÀ - Ñ/Í - Ñîõðàíèòü
    },
    EmptyFuncVV, OnPress_SerialNumber, OnRegSet_SerialNumber
};

static void OnPress_SerialNumber(void)
{
    OpenPageAndSetItCurrent(Page_SB_SerialNumber);
    Display_SetAddDrawFunction(Draw_EnterSerialNumber);
}

static void Draw_EnterSerialNumber(void)
{
    int x0 = GridLeft() + 40;
    int y0 = GRID_TOP + 20;
    int width = GridWidth() - 80;
    int height = 160;

    Painter_DrawRectangleC(x0, y0, width, height, COLOR_FILL);
    Painter_FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, COLOR_BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Ðèñóåì áîëüøèå áóêâû àíãëèéñêîãî àëôàâèòà
    while (symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Òåïåðü ðèñóåì öèôðû è ïðîáåë
    position = 0;
    while (symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    int x = Painter_DrawTextC(x0 + deltaX, y0 + 65, stringSN, COLOR_FILL);
    Painter_FillRegionC(x, y0 + 65, 5, 8, COLOR_FLASH_10);

    // Òåïåðü âûâåäåì èíôîðìàöèþ îá îñòàâøåìñÿ ìåñòå â OTP-ïàìÿòè äëÿ çàïèñè
    
    char buffer[20];
    
    int allShots = OTP_GetSerialNumber(buffer);
    
    Painter_DrawFormText(x0 + deltaX, y0 + 130, COLOR_FILL, "Òåêóùèé ñîõðàí¸ííûé íîìåð %s", buffer);

    Painter_DrawFormText(x0 + deltaX, y0 + 100, COLOR_FILL, "Îñòàëîñü ìåñòà äëÿ %d ïîïûòîê", allShots);
}

static void OnRegSet_SerialNumber(int angle)
{
extern void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);

    OnMemExtSetMaskNameRegSet(angle, 0x27);
}


// ÎÒËÀÄÊÀ - Ñ/Í - Âûõîä -----------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSerialNumber_Exit =
{
    Item_SmallButton, &mpSerialNumber,
    COMMON_BEGIN_SB_EXIT,
    OnPress_SerialNumber_Exit,
    DrawSB_Exit
};

static void OnPress_SerialNumber_Exit(void)
{
    Display_RemoveAddDrawFunction();
}

// ÎÒËÀÄÊÀ - Ñ/Í - Óäàëèòü ---------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSerialNumber_Delete =
{
    Item_SmallButton, &mpSerialNumber, 0,
    {
        "Óäàëèòü", "Delete",
        "Óäàëÿåò âñå ââåä¸ííûå ñèìâîëû",
        "Deletes all entered symbols"
    },
    OnPress_SerialNumber_Delete,
    Draw_SerialNumber_Delete
};

static void OnPress_SerialNumber_Delete(void)
{
    stringSN[0] = 0;
}

static void Draw_SerialNumber_Delete(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter_SetFont(TypeFont_8);
}

// ÎÒËÀÄÊÀ - Ñ/Í - Backspace -------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSerialNumber_Backspace =
{
    Item_SmallButton, &mpSerialNumber, 0,
    {
        "Backspace", "Backspace",
        "Óäàëÿåò ïîñëåäíèé ââåä¸ííûé ñèìâîë",
        "Delete the last entered symbol"
    },
    OnPress_SerialNumber_Backspace,
    Draw_SerialNumber_Backspace
};

static void OnPress_SerialNumber_Backspace(void)
{
    int index = strlen(stringSN);
    if (index)
    {
        stringSN[index - 1] = 0;
    }
}

static void Draw_SerialNumber_Backspace(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter_SetFont(TypeFont_8);
}

// ÎÒËÀÄÊÀ - Ñ/Í - Âñòàâèòü --------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSerialNumber_Insert =
{
    Item_SmallButton, &mpSerialNumber, 0,
    {
        "Âñòàâèòü", "Insert",
        "Âñòàâëÿåò âûáðàíûé ñèìâîë",
        "Inserts the chosen symbol"
    },
    OnPress_SerialNumber_Insert,
    Draw_SerialNumber_Insert
};

static void OnPress_SerialNumber_Insert(void)
{
    int size = strlen(stringSN);
    if (size < LENGTH_SN - 1)
    {
        stringSN[size] = symbolsAlphaBet[INDEX_SYMBOL][0];
        stringSN[size + 1] = '\0';
    }
}

static void Draw_SerialNumber_Insert(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Painter_SetFont(TypeFont_8);
}

// ÎÒËÀÄÊÀ - Ñ/Í - Ñîõðàíèòü -------------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSerialNumber_Save =
{
    Item_SmallButton, &mpSerialNumber, 0,
    {
        "Ñîõðàíèòü", "Save",
        "Çàïèñûâàåò ñåðèéíûé íîìåð â OTP",
        "Records the serial number in OTP"
    },
    OnPress_SerialNumber_Save,
    Draw_SerialNumber_Save
};

static void OnPress_SerialNumber_Save(void)
{
    if (!OTP_SaveSerialNumber(stringSN))
    {
        Display_ShowWarningBad(FullyCompletedOTP);
    }
}

static void Draw_SerialNumber_Save(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter_SetFont(TypeFont_8);
}


/** @}  @}
 */

        /*
        static void OnChangeRShiftADC(bool active)
        {
            FPGA_SetRShift(A, RSHIFT_A);
            FPGA_SetRShift(B, RSHIFT_B);
        }

        const Choice mcDebugADCrShiftMode =
        {
            Item_Choice, &mpADC_AltRShift, {"Ðåæèì",      "Mode"},
            {
                "",
                ""
            },
            EmptyFuncBV,
            {                                    {DISABLE_RU,   DISABLE_EN},
                                                 {"Ðåàëüíûé",   "Real"},
                                                 {"Ðó÷íîé",     "Hand"}
            },
            (int8*)&set.debug.rShiftADCtype, OnChangeRShiftADC
        };

        // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ. ÑÌÅÙÅÍÈÅ - Ñìåùåíèå 1ê 2ìÂ
        static TimeStructGovernor tsRShiftADC;
        static bool IsActiveRShiftADC(void)
        {
            return set.debug.rShiftADCtype == RShiftADC_Hand;
        }

        static void OnChangeRShiftADC0(void)
        {
            FPGA_SetRShift(A, RSHIFT_A);
        }

        const Governor mgDebugRShiftADC00 =
        {
            Item_Governor, &mpADC_AltRShift,
            {
                "Ñìåù 1ê 2ìÂ", "Shift 1ch"
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
            Item_Governor, &mpADC_AltRShift,
            {
                "Ñìåù 1ê 500ìÂ", "Shift 1ch 500mV"
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
            FPGA_SetRShift(B, RSHIFT_B);
        }

        // ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ. ÑÌÅÙ. ÏÀÌ.
const Page mspDebugADCaltShift;

/// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ. ÑÌÅÙ. ÏÀÌ. - Âåëè÷èíà
const Governor mgDebugADCaltShift =
{
    Item_Governor, &mspDebugADCaltShift, 0,
    {
        "Âåëè÷èíà", "Value",
        "",
        ""
    },
    &set.debug.altShift, -2, 2
};

// ÎÒËÀÄÊÀ - ÀÖÏ - ÄÎÏ ÑÌÅÙ ÏÀÌ /////////////////////////////////////////////////////////////////////////////////////////////
const Page mspDebugADCaltShift =
{
    Item_Page, &mpADC, 0,
    {
        "ÄÎÏ ÑÌÅÙ ÏÀÌ", "ALT SHIFT MEM",
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
    Item_Governor, &mspDebugInformation,
    {
        "Íîìåð", "Number"
    },
    {
        "Óñòàíîâêà íîìåðà ñåðèéíîãî íîìåðà",
        "Installation numbers of the serial number"
    },
    EmptyFuncBV,
    &set.debug.infoNumber, 0, 100
};

const Governor mgDebugInfoYear =
{
    Item_Governor, &mspDebugInformation,
    {
        "Ãîä", "Year"
    },
    {
        "Óñòàíîâêà ãîäà ñåðèéíîãî íîìåðà",
        "Installation year serial number"
    },
    EmptyFuncBV,
    &set.debug.infoYear, 2015, 2050
};

const Governor mgDebugInfoVersion =
{
    Item_Governor, &mspDebugInformation,
    {
        "Âåðñèÿ", "Version"
    },
    {
        "Óñòàíîâêà íîìåðà âåðñèè",
        "Setting the version number"
    },
    EmptyFuncBV,
    &set.debug.infoVer, 10, 100
};

// ÎÒËÀÄÊÀ - ÈÍÔÎÐÌÀÖÈß ////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspDebugInformation =
{
    Item_Page, &mpDebug,
    {
        "ÈÍÔÎÐÌÀÖÈß", "INFORMATION",
    },
    {
        "Ïîçâîëÿåò ââåñòè èíôîðìàöèþ äëÿ ìåíþ ÑÅÐÂÈÑ-ÈÍÔÎÐÌÀÖÈß",
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
