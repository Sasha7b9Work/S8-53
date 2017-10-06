#include "defines.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "FPGA/FPGA_Types.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageTrig
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Choice mcMode;                     ///< СИНХР - Режим
void          OnPress_Mode(bool active);
static const Choice mcSource;                   ///< СИНХР - Источник
static void OnChanged_Source(bool active);
static const Choice mcPolarity;                 ///< СИНХР - Полярность
static void OnChanged_Polarity(bool active);
static const Choice mcInput;                    ///< СИНХР - Вход
static void OnChanged_Input(bool active);
static const Page   mpAutoFind;                 ///< СИНХР - ПОИСК
static const Choice mcAutoFind_Mode;            ///< СИНХР - ПОИСК - Режим
static const Button mbAutoFind_Search;          ///< СИНХР - ПОИСК - Найти
static bool  IsActive_AutoFind_Search(void);
static void   OnPress_AutoFind_Search(void);

extern const Page mainPage;

// СИНХР /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpTrig =             ///< СИНХР
{
    Item_Page, &mainPage, 0,
    {
        "СИНХР", "TRIG",
        "Содержит настройки синхронизации.",
        "Contains synchronization settings."
    },
    Page_Trig,
    {
        (void*)&mcMode,         // СИНХР - Режим
        (void*)&mcSource,       // СИНХР - Источник
        (void*)&mcPolarity,     // СИНХР - Полярность
        (void*)&mcInput,        // СИНХР - Вход
        (void*)&mpAutoFind      // СИНХР - ПОИСК
    }
};


// СИНХР - Режим -------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMode =
{
    Item_Choice, &mpTrig, 0,
    {
        "Режим", "Mode"
        ,
        "Задаёт режим запуска:\n"
        "1. \"Авто\" - запуск происходит автоматически.\n"
        "2. \"Ждущий\" - запуск происходит по уровню синхронизации.\n"
        "3. \"Однократный\" - запуск происходит по достижении уровня синхронизации один раз. Для следующего измерения нужно нажать кнопку ПУСК/СТОП."
        ,
        "Sets the trigger mode:\n"
        "1. \"Auto\" - start automatically.\n"
        "2. \"Standby\" - the launch takes place at the level of synchronization.\n"
        "3. \"Single\" - the launch takes place on reaching the trigger levelonce. For the next measurement is necessary to press the START/STOP."
    },
    {
        {"Авто ",       "Auto"},
        {"Ждущий",      "Wait"},
        {"Однократный", "Single"}
    },
    (int8*)&START_MODE, OnPress_Mode
};

void OnPress_Mode(bool active)
{
    FPGA_Stop(false);
    if (!START_MODE_IS_SINGLE)
    {
        FPGA_Start();
    }
}


// СИНХР - Источник ----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcSource =
{
    Item_Choice, &mpTrig, 0,
    {
        "Источник", "Source",
        "Выбор источника сигнала синхронизации.",
        "Synchronization signal source choice."
    },
    {
        {"Канал 1", "Channel 1"},
        {"Канал 2", "Channel 2"},
        {"Внешний", "External"}
    },
    (int8*)&TRIG_SOURCE, OnChanged_Source
};

static void OnChanged_Source(bool active)
{
    FPGA_SetTrigSource(TRIG_SOURCE);
}


// СИНХР - Полярность --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcPolarity =
{
    Item_Choice, &mpTrig, 0,
    {
        "Полярность", "Polarity"
        ,
        "1. \"Фронт\" - запуск происходит по фронту синхроимпульса.\n"
        "2. \"Срез\" - запуск происходит по срезу синхроимпульса."
        ,
        "1. \"Front\" - start happens on the front of clock pulse.\n"
        "2. \"Back\" - start happens on a clock pulse cut."
    },
    {
        {"Фронт",   "Front"},
        {"Срез",    "Back"}
    },
    (int8*)&TRIG_POLARITY, OnChanged_Polarity
};

static void OnChanged_Polarity(bool active)
{
    FPGA_SetTrigPolarity(TRIG_POLARITY);
}


// СИНХР - Вход --------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInput =
{
    Item_Choice, &mpTrig, 0,
    {
        "Вход", "Input"
        ,
        "Выбор связи с источником синхронизации:\n"
        "1. \"ПС\" - полный сигнал.\n"
        "2. \"АС\" - закрытый вход.\n"
        "3. \"ФНЧ\" - фильтр низких частот.\n"
        "4. \"ФВЧ\" - фильтр высоких частот."
        ,
        "The choice of communication with the source of synchronization:\n"
        "1. \"SS\" - a full signal.\n"
        "2. \"AS\" - a gated entrance.\n"
        "3. \"LPF\" - low-pass filter.\n"
        "4. \"HPF\" - high-pass filter frequency."
    },
    {
        {"ПС",  "Full"},
        {"АС",  "AC"},
        {"ФНЧ", "LPF"},
        {"ФВЧ", "HPF"}
    },
    (int8*)&TRIG_INPUT, OnChanged_Input
};

static void OnChanged_Input(bool active)
{
    FPGA_SetTrigInput(TRIG_INPUT);
}


// СИНХР - ПОИСК -------------------------------------------------------------------------------------------------------------------------------------
static const Page mpAutoFind =
{
    Item_Page, &mpTrig, 0,
    {
        "ПОИСК", "SEARCH",
        "Управление автоматическим поиском уровня синхронизации.",
        "Office of the automatic search the trigger level."
    },
    Page_TrigAuto,
    {
        (void*)&mcAutoFind_Mode,     // СИНХР - ПОИСК - Режим
        (void*)&mbAutoFind_Search     // СИНХР - ПОИСК - Найти
    }
};


// СИНХР - ПОИСК - Режим -----------------------------------------------------------------------------------------------------------------------------
static const Choice mcAutoFind_Mode =
{
    Item_Choice, &mpAutoFind, 0,
    {
        "Режим", "Mode"
        ,
        "Выбор режима автоматического поиска синхронизации:\n"
        "1. \"Ручной\" - поиск производится по нажатию кнопки \"Найти\" или по удержанию в течение 0.5с кнопки СИНХР, если установлено \"СЕРВИС\x99Реж длит СИНХР\x99Автоуровень\".\n"
        "2. \"Автоматический\" - поиск производится автоматически."
        ,
        "Selecting the automatic search of synchronization:\n"
#pragma push
#pragma diag_suppress 192
        "1. \"Hand\" - search is run on pressing of the button \"Find\" or on deduction during 0.5s the СИНХР button if it is established \"SERVICE\x99Mode long СИНХР\x99\Autolevel\".\n"
#pragma pop
        "2. \"Auto\" - the search is automatically."
    },
    {
        {"Ручной",          "Hand"},
        {"Автоматический",  "Auto"}
    },
    (int8*)&TRIG_MODE_FIND
};


// СИНХР - ПОИСК - Найти -----------------------------------------------------------------------------------------------------------------------------
static const Button mbAutoFind_Search =
{
    Item_Button, &mpAutoFind, IsActive_AutoFind_Search,
    {
        "Найти", "Search",
        "Производит поиск уровня синхронизации.",
        "Runs for search synchronization level."
    },
    OnPress_AutoFind_Search
};

static bool IsActive_AutoFind_Search(void)
{
    return TRIG_MODE_FIND_IS_HAND;
}

static void OnPress_AutoFind_Search(void)
{
    FPGA_FindAndSetTrigLevel();
}


/** @}  @}
 */
