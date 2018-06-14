#include "Settings.h"
#include "Hardware/EPROM.H"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"
#include "Menu/Menu.h"
#include "Log.h"
#include <string.h>


/** @addtogroup Settings
 *  @{
 */


static const Settings defaultSettings =
{
    0,                              // size
    // Display
    {
        5,                          // timeShowLevels
        512 - 140,                  // shiftInMemory
        5,                          // timeMessages
        100,                        // brightness
        20,                         // brightnessGrid
        {
            MAKE_COLOR(0, 0, 0),     // COLOR_BLACK = 0x00,
            MAKE_COLOR(31, 63, 31),  // COLOR_WHITE = 0x01,
            MAKE_COLOR(15, 31, 15),  // COLOR_GRID = 0x02,
            MAKE_COLOR(1, 50, 31),   // COLOR_DATA_A = 0x03,
            MAKE_COLOR(0, 63, 0),    // COLOR_DATA_B = 0x04,
            MAKE_COLOR(26, 54, 26),  // COLOR_MENU_FIELD = 0x05,
            MAKE_COLOR(31, 44, 0),   // COLOR_MENU_TITLE = 0x06,
            MAKE_COLOR(15, 22, 0),   // COLOR_MENU_TITLE_DARK = 0x07,
            MAKE_COLOR(31, 63, 0),   // COLOR_MENU_TITLE_BRIGHT = 0x08,
            MAKE_COLOR(26, 34, 0),   // COLOR_MENU_ITEM = 0x09,
            MAKE_COLOR(13, 17, 0),   // COLOR_MENU_ITEM_DARK = 0x0a,
            MAKE_COLOR(31, 51, 0),   // COLOR_MENU_ITEM_BRIGHT = 0x0b,
            MAKE_COLOR(0, 0, 0),     // COLOR_MENU_SHADOW = 0x0c,
            MAKE_COLOR(0, 0, 0),     // COLOR_EMPTY = 0x0d,
            MAKE_COLOR(1, 40, 25),   // COLOR_EMPTY_A = 0x0e,
            MAKE_COLOR(0, 51, 0)     // COLOR_EMPTY_B = 0x0f,
        },
        ModeDrawSignal_Lines,       // modeDrawSignal
        TypeGrid_1,                 // typeGrid
        NumAccumulation_1,          // numAccumulation
        NumAveraging_1,             // numAveraging
        Averaging_Accurately,       // modeAveraging
        NumMinMax_1,                // enumMinMax
        Smoothing_Disable,          // smoothing
        NumSignalsInSec_25,         // num signals in one second
        A,                          // lastAffectedChannel
        ModeAccumulation_NoReset,   // modeAccumulation
        AM_Show,                    // altMarkers
        MenuAutoHide_None,          // menuAutoHide
        true,                       // showFullMemoryWindow
        false,                      // showStringNavigation
        LinkingRShift_Voltage       // linkingRShift
    },
    // channels
    {
        {
            1.0f,                   // коэффициент калибровки
            RShiftZero,
            {{0}, {0}},             // rShiftAdd
            ModeCouple_AC,          // ModCouple
            Divider_1,           // Divider
            Range_500mV,            // range
            true,                   // enable
            false,                  // inverse
            false,                  // filtr
			0                       // addShiftADC
        },
        {
            1.0f,                   // коэффициент калибровки
            RShiftZero,
            {{0}, {0}},             // rShiftAdd
            ModeCouple_AC,          // ModCouple
            Divider_1,           // Divider
            Range_500mV,            // range
            true,                   // enable
            false,                  // inverse
            false,                  // filtr
			5                       // addShiftADC
        }
        
    },
    // trig
    {
        StartMode_Auto,
        TrigSource_ChannelA,
        TrigPolarity_Front,
        TrigInput_Full,
        {TrigLevZero, TrigLevZero, TrigLevZero},
        ModeLongPressTrig_Auto,
        TrigModeFind_Hand
    },
    // time
    {
        TBase_200us,
        0,                          // set.time.tShiftRel
        FunctionTime_Time,
        TPos_Center,
        SampleType_Equal,
        PeackDet_Disable,
        false,
        FNP_1024
    },
    // cursors
    {
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl U
        { CursCntrl_Disable, CursCntrl_Disable },   // CursCntrl T
        A,                                      // source
        { 60.0f,  140.0f, 60.0f, 140.0f },          // posCur U
        { 80.0f,  200.0f, 80.0f, 200.0f },          // posCur T
        { 80.0f,  80.0f },                          // расстояние между курсорами напряжения для 100%
        { 120.0f, 120.0f },                         // расстояние между курсорами времени для 100%
        CursMovement_Points,                        // CursMovement
        CursActive_None,                            // CursActive
        { CursLookMode_None, CursLookMode_None },   // Режим слежения курсоров.
        false,                                      // showFreq
        false                                       // showCursors
    },
    // memory
    {
        FNP_1024,
        ModeWork_Direct,
        FileNamingMode_Mask,
        "Signal_\x07\x03",      // \x07 - означает, что здесь должен быть очередной номер, \x03 - минимум на три знакоместа
        "Signal01",
        0,
        {
            false
        },
        ModeShowIntMem_Both,    // modeShowIntMem
        false,                  // flashAutoConnect
        ModeBtnMemory_Menu,
        ModeSaveSignal_BMP
    },
    // measures
    {
        MN_1,                       // measures number
        A_B,                        // source
        ModeViewSignals_AsIs,       // mode view signal
        {
            Measure_VoltageMax,     Measure_VoltageMin,     Measure_VoltagePic,         Measure_VoltageMaxSteady,   Measure_VoltageMinSteady,
            Measure_VoltageAverage, Measure_VoltageAmpl,    Measure_VoltageRMS,         Measure_VoltageVybrosPlus,  Measure_VoltageVybrosMinus,
            Measure_Period,         Measure_Freq,           Measure_TimeNarastaniya,    Measure_TimeSpada,          Measure_DurationPlus
        },
        false,                      // show
        MeasuresField_Screen,       // брать для измерений значения, видимые на экране
        //{0, 200},                 // начальные значения курсоров напряжения для зоны измерений
        //{372, 652},               // начальные значения курсоров времени для зоны измерений
        {50, 150},                  // начальные значения курсоров напряжения для зоны измерений
        {422, 602},                 // начальные значения курсоров времени для зоны измерений
        CursCntrl_1,                // Управляем первым курсором напряжения
        CursCntrl_1,                // Управляем первым курсором времени
        CursActive_T,               // Активны курсоры времени.
        Measure_None                // marked Measure
    },
    // math
    {
        ScaleFFT_Log,
        SourceFFT_A,
        WindowFFT_Rectangle,
        FFTmaxDB_60,
        Function_Sum,
        0,
        {100, 256 - 100},
        1,
        1,
        1,
        1,
        false,
        ModeDrawMath_Disable,       // modeDrawMath
        ModeRegSet_Range,           // modeRegSet
        Range_50mV,
        Divider_1,
        RShiftZero                  // rShift
    },
    // service
    {
        true,                       // screenWelcomeEnable
        true,                       // soundEnable
        Calibrator_Freq,            // calibratorEnable
        0,                          // IP-адрес (временно)  WARN
        ColorScheme_WhiteLetters    // colorScheme
    },
    {
        0x8b, 0x2e, 0xaf, 0x8f, 0x13, 0x00, // mac
        192, 168, 1, 200,                   // ip
        7,
        255, 255, 255, 0,                   // netmask
        192, 168, 1, 1                      // gateway
    },
    // common
    {
        0
    },
    // menu
    {
        {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f},
        {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
        true,
        0
    },
    // debug
    {
        0,                      // numStrings
        0,                      // размер шрифта - 5
        0,                      // consoleInPause
        BalanceADC_Settings,    // balanceADC
        {0, 5},                 // shiftADC
        StretchADC_Settings,    // stretch
        {128, 128},             // stretchADC
//      RShiftADC_Settings,
        1000,                   // numMeasuresForGates
        0,                      // shiftT0
        false,                  // showStats
        6,                      // numAveForRand
        false,                  // view altera writting data
        false,                  // view all altera writting data
        0,                      // alt shift
        {
            false,              // показывать ли флаг готовности
            false,              // rShift0
            false,              // rShift1
            false               // trigLev
        }
    }
};

Settings set;


void Settings::Load(bool _default)
{   
    if (!_default)
    {
        _default = !EPROM::LoadSettings();
        
    }
    
    if(_default)
    {
        uint16 rShiftAddA[RangeSize][2];
        uint16 rshiftAddB[RangeSize][2];

        memcpy((void *)rShiftAddA, (void *)&RSHIFT_ADD(A, 0, 0), 2 * RangeSize * 2); // Сначала сохраняем несбрасываемые настройки
        memcpy((void *)rshiftAddB, (void *)&RSHIFT_ADD(B, 0, 0), 2 * RangeSize * 2);

        int16  balanceADC0 = BALANCE_ADC_A;
        int16  balanceADC1 = BALANCE_ADC_B;
        int16  numAverageForRand = NUM_AVE_FOR_RAND;
        BalanceADCtype balanceType = BALANCE_ADC_TYPE;

        memcpy((void*)&set, (void*)(&defaultSettings), sizeof(set));                // Потом заполняем значениями по умолчанию

        memcpy((void *)&RSHIFT_ADD(A, 0, 0), (void *)rShiftAddA, 2 * RangeSize * 2);  // И восстанавливаем несбрасываемые настройки
        memcpy((void *)&RSHIFT_ADD(B, 0, 0), (void *)rshiftAddB, 2 * RangeSize * 2);

        BALANCE_ADC_A = balanceADC0;
        BALANCE_ADC_B = balanceADC1;
        NUM_AVE_FOR_RAND = numAverageForRand;
        BALANCE_ADC_TYPE = balanceType;
    }

    Painter::LoadPalette();

    Panel::EnableLEDChannel0(sChannel_Enabled(A));
    Panel::EnableLEDChannel1(sChannel_Enabled(B));
    Menu::SetAutoHide(true);
    Display::ChangedRShiftMarkers();

    SETTINGS_IS_LOADED = 1;
}

void Settings::Save()
{
    EPROM::SaveSettings();
}

bool Settings::DebugModeEnable()
{
    return true;
}

int8 MenuPosActItem(NamePage namePage)
{
    return set.menu.posActItem[namePage];
}

void SetMenuPosActItem(NamePage namePage, int8 pos)
{
    set.menu.posActItem[namePage] = pos;
}

int8 MenuCurrentSubPage(NamePage namePage)
{
    return set.menu.currentSubPage[namePage];
}

void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage)
{
    set.menu.currentSubPage[namePage] = posSubPage;
}

bool MenuIsShown()
{
    return set.menu.isShown;
}

void ShowMenu(bool show)
{
    set.menu.isShown = show ? 1U : 0U;
    Menu::SetAutoHide(true);
}

bool MenuIsMinimize()
{
    return Menu::TypeOpenedItem() == Item_Page && Menu::GetNamePage((const Page *)Menu::OpenedItem()) >= Page_SB_Curs;
}

bool MenuPageDebugIsActive()
{
    return set.menu.pageDebugActive;
}

void SetMenuPageDebugActive(bool active)
{
    set.menu.pageDebugActive = active;
}

void CurrentPageSBregSet(int angle)
{
    Page *page = (Page *)Menu::OpenedItem();
    if (page->funcRegSetSB)
    {
        page->funcRegSetSB(angle);
    }
}

const SmallButton* GetSmallButton(PanelButton button)
{
    if(MenuIsMinimize() && button >= B_Menu && button <= B_F5)
    {
        Page *page = (Page *)Menu::OpenedItem();
        SmallButton *sb = (SmallButton *)(*page->items)[button - B_Menu];
        return sb;
    }
    return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Range& operator++(Range &range)
{
    range = (Range)((int)range + 1);
    return range;

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Range& operator--(Range &range)
{
    range = (Range)((int)range - 1);
    return range;
}

/** @}
 */
