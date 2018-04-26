#include "defines.h"
#include "Log.h"
#include "Settings/SettingsTypes.h"
#include "Menu/MenuItems.h"
#include "Panel/Panel.h"
#include "FPGA/FPGA.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageChannels
 *  @{
 */

extern const Page pChanA;
extern const Page pChanB;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Choice mcInputA;                   ///< КАНАЛ 1 - Вход
void OnChanged_InputA(bool active);
extern const Choice mcCoupleA;                  ///< КАНАЛ 1 - Связь
void OnChanged_CoupleA(bool active);
extern const Choice mcFiltrA;                   ///< КАНАЛ 1 - Фильтр
void OnChanged_FiltrA(bool active);
extern const Choice mcInverseA;                 ///< КАНАЛ 1 - Инверсия
static void OnChanged_InverseA(bool active);
extern const Choice mcMultiplierA;              ///< КАНАЛ 1 - Множитель

extern const Choice mcInputB;                   ///< КАНАЛ 2 - Вход
void OnChanged_InputB(bool active);
extern const Choice mcCoupleB;                  ///< КАНАЛ 2 - Связь
void OnChanged_CoupleB(bool active);
extern const Choice mcFiltrB;                   ///< КАНАЛ 2 - Фильтр
void OnChanged_FiltrB(bool active);
extern const Choice mcInverseB;                 ///< КАНАЛ 2 - Инверсия
static void OnChanged_InverseB(bool active);
extern const Choice mcMultiplierB;              ///< КАНАЛ 2 - Множитель


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const char chanInputRu[] =   "1. \"Вкл\" - выводить сигнал на экран.\n"
                                    "2. \"Откл\" - не выводить сигнал на экран.";
extern const char chanInputEn[] =   "1. \"Enable\" - signal output to the screen.\n"
                                    "2. \"Disable\" - no output to the screen.";

extern const char chanCoupleRu[] =  "Задаёт вид связи с источником сигнала.\n"
                                    "1. \"Пост\" - открытый вход.\n"
                                    "2. \"Перем\" - закрытый вход.\n"
                                    "3. \"Земля\" - вход соединён с землёй.";
extern const char chanCoupleEn[] =  "Sets a type of communication with a signal source.\n"
                                    "1. \"AC\" - open input.\n"
                                    "2. \"DC\" - closed input.\n"
                                    "3. \"Ground\" - input is connected to the ground.";

extern const char chanFiltrRu[] = "Включает/отключает фильтр для ограничения полосы пропускания.";
extern const char chanFiltrEn[] = "Includes/switches-off the filter for restriction of a pass-band.";

extern const char chanInverseRu[] = "При \"Вкл\" сигнал на экране будет симметрично отражён относительно U = 0В.";
extern const char chanInverseEn[] = "When \"Enable\" signal on the screen will be reflected symmetrically with respect to U = 0V.";

extern const char chanMultiplierRu[] = "Ослабление сигнала:\n\"x1\" - сигнал не ослабляется.\n\"x10\" - сигнал ослабляется в 10 раз";
extern const char chanMultiplierEn[] = "Attenuation: \n\"x1\" - the signal is not attenuated.\n\"x10\" - the signal is attenuated by 10 times";


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;


// КАНАЛ 1 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsChanA =
{
    (void*)&mcInputA,       // КАНАЛ 1 - Вход
    (void*)&mcCoupleA,      // КАНАЛ 1 - Связь
    (void*)&mcFiltrA,       // КАНАЛ 1 - Фильтр
    (void*)&mcInverseA,     // КАНАЛ 1 - Инверсия
    (void*)&mcMultiplierA   // КАНАЛ 1 - Множитель
};

const Page pChanA           ///< КАНАЛ 1
(
    &mainPage, 0,
    "КАНАЛ 1", "CHANNEL 1",
    "Содержит настройки канала 1.",
    "Contains settings of the channel 1.",
    Page_Channel0, &itemsChanA
);


// КАНАЛ 1 - Вход ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInputA =
{
    Item_Choice, &pChanA, 0,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_ENABLED_A, OnChanged_InputA
};

void OnChanged_InputA(bool active)
{
    Panel::EnableLEDChannel0(sChannel_Enabled(A));
}


// КАНАЛ 1 - Связь -----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcCoupleA =
{
    Item_Choice, &pChanA, 0,
    {
        "Связь",   "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост",    "AC"},
        {"Перем",   "DC"},
        {"Земля",   "Ground"}
    },
    (int8*)&SET_COUPLE_A, OnChanged_CoupleA
};

void OnChanged_CoupleA(bool active)
{
    FPGA::SetModeCouple(A, SET_COUPLE_A);
}


// КАНАЛ 1 - Фильтр ----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcFiltrA =
{
    Item_Choice, &pChanA, 0,
    {
        "Фильтр", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_A, OnChanged_FiltrA
};

void OnChanged_FiltrA(bool active)
{
    FPGA::EnableChannelFiltr(A, SET_FILTR_A);
}


// КАНАЛ 1 - Инверсия --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInverseA =
{
    Item_Choice, &pChanA, 0,
    {
        "Инверсия",    "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_INVERSE_A, OnChanged_InverseA
};

static void OnChanged_InverseA(bool active)
{
    FPGA::SetRShift(A, SET_RSHIFT_A);
}


// КАНАЛ 1 - Множитель -------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMultiplierA =
{
    Item_Choice, &pChanA, 0,
    {
        "Множитель", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"х1",  "x1"},
        {"x10", "x10"}
    },
    (int8*)&SET_DIVIDER(A)
};



// КАНАЛ 2 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsChanB =
{
    (void*)&mcInputB,       // КАНАЛ 2 - Вход
    (void*)&mcCoupleB,      // КАНАЛ 2 - Связь
    (void*)&mcFiltrB,       // КАНАЛ 2 - Фильтр
    (void*)&mcInverseB,     // КАНАЛ 2 - Инверсия
    (void*)&mcMultiplierB   // КАНАЛ 2 - Множитель
};

const Page pChanB
(
    &mainPage, 0,
    "КАНАЛ 2", "CHANNEL 2",
    "Содержит настройки канала 2.",
    "Contains settings of the channel 2.",
    Page_Channel1, &itemsChanB
);


// КАНАЛ 2 - Вход ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInputB =  ///< КАНАЛ 2
{
    Item_Choice, &pChanB, 0,
    {
        "Вход", "Input",
        chanInputRu,
        chanInputEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_ENABLED_B, OnChanged_InputB
};

void OnChanged_InputB(bool active)
{
    Panel::EnableLEDChannel1(sChannel_Enabled(B));
}


// КАНАЛ 2 - Связь -----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcCoupleB =
{
    Item_Choice, &pChanB, 0,
    {
        "Связь", "Couple",
        chanCoupleRu,
        chanCoupleEn
    },
    {
        {"Пост",    "AC"},
        {"Перем",   "DC"},
        {"Земля",   "Ground"}
    },
    (int8*)&SET_COUPLE_B, OnChanged_CoupleB
};

void OnChanged_CoupleB(bool active)
{
    FPGA::SetModeCouple(B, SET_COUPLE_B);
}

// КАНАЛ 2 - Фильтр ----------------------------------------------------------------------------------------------------------------------------------
static const Choice mcFiltrB =
{
    Item_Choice, &pChanB, 0,
    {
        "Фильтр", "Filtr",
        chanFiltrRu,
        chanFiltrEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_FILTR_B, OnChanged_FiltrB
};

void OnChanged_FiltrB(bool active)
{
    FPGA::EnableChannelFiltr(B, SET_FILTR_B);
}


// КАНАЛ 2 - Инверсия --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcInverseB =
{
    Item_Choice, &pChanB, 0,
    {
        "Инверсия", "Inverse",
        chanInverseRu,
        chanInverseEn
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SET_INVERSE_B, OnChanged_InverseB
};

static void OnChanged_InverseB(bool active)
{
    FPGA::SetRShift(B, SET_RSHIFT_B);
}


// КАНАЛ 2 - Множитель -------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMultiplierB =
{
    Item_Choice, &pChanB, 0,
    {
        "Множитель", "Divider",
        chanMultiplierRu,
        chanMultiplierEn
    },
    {
        {"х1",  "x1"},
        {"x10", "x10"}
    },
    (int8*)&SET_DIVIDER(B)
};


/** @}  @}
 */
