#include "defines.h"
#include "PageCursors.h"
#include "Definition.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Panel/Controls.h"
#include "Display/Colors.h"
#include "Display/Grid.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Sound.h"
#include "Log.h"
#include "Display/Painter.h"
#include <math.h>
#include <string.h>


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageCursors
 *  @{
 */

extern const Page pCursors;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Choice mcShow;                             /// КУРСОРЫ - Показывать

extern const Choice mcTrackingT1U1;                     ///< КУРСОРЫ - Курсоры T1,U1

extern const Choice mcTrackingT2U2;                     ///< КУРСОРЫ - Курсоры T2,U2

extern const Choice mcShowFreq;                         ///< КУРОСРЫ - 1/dT

extern const Page mspSet;                               ///< КУРСОРЫ - УСТАНОВИТЬ
static void OnRotate_RegSet_Set(int angle);             ///< Вращение ручки УСТАНОВКА на странице КУРСОРЫ-УСТАНОВИТЬ
extern const SmallButton sbSetExit;                     ///< КУРСОРЫ - УСТАНОВИТЬ - Выход
static void PressSB_Cursors_Exit();
extern const SmallButton sbSetSource;                   ///< КУРСОРЫ - УСТАНОВИТЬ - Источник
static void PressSB_Cursors_Source();
static void DrawSB_Cursors_Source(int x, int y);
static void DrawSB_Cursors_SourceA(int x, int y);
static void DrawSB_Cursors_SourceB(int x, int y);
extern const SmallButton sbSetU;                        ///< КУРСОРЫ - УСТАНОВИТЬ - Курсоры U . Выбор курсора напряжения - курсор 1, курсор 2, оба курсора или отключены.
static void PressSB_Cursors_U();
static void DrawSB_Cursors_U(int x, int y);
static void DrawSB_Cursors_U_Disable(int x, int y);
static void DrawSB_Cursors_U_Upper(int x, int y);
static void DrawSB_Cursors_U_Lower(int x, int y);
static void DrawSB_Cursors_U_Both_Enable(int x, int y);
static void DrawSB_Cursors_U_Both_Disable(int x, int y);
extern const SmallButton sbSetT;                        ///< КУРСОРЫ - УСТАНОВИТЬ - Курсоры T . Выбор курсора времени - курсор 1, курсор 2, оба курсора или отключены.
static void PressSB_Cursors_T();
static void DrawSB_Cursors_T(int x, int y);
static void DrawSB_Cursors_T_Disable(int x, int y);
static void DrawSB_Cursors_T_Both_Disable(int x, int y);
static void DrawSB_Cursors_T_Left(int x, int y);
static void DrawSB_Cursors_T_Right(int x, int y);
static void DrawSB_Cursors_T_Both_Enable(int x, int y);
extern const SmallButton sbSet100;                      ///< КУРСОРЫ - УСТАНОВИТЬ - 100% . Установка 100 процентов в текущие места курсоров.
static void DrawSB_Cursors_100(int x, int y);
static void PressSB_Cursors_100();
extern const SmallButton sbSetPointsPercents;           ///< КУРСОРЫ - УСТАНОВИТЬ - Перемещение . Переключение шага перемещения курсоров - по пикселям или по процентам.
static void PressSB_Cursors_PointsPercents();
static void DrawSB_Cursors_PointsPercents(int x, int y);
static void DrawSB_Cursors_PointsPercents_Percents(int x, int y);
static void DrawSB_Cursors_PointsPercents_Points(int x, int y);


static void MoveCursUonPercentsOrPoints(int delta);
static void MoveCursTonPercentsOrPoints(int delta);
static void SetShiftCursPosU(Channel chan, int numCur, float delta);    ///< Изменить значение позиции курсора напряжения на delta точек
static void SetShiftCursPosT(Channel chan, int numCurs, float delta);   ///< Изменить значение позиции курсора времени на delta точек
static void SetCursPos100(Channel chan);                                ///< Запомнить позиции курсоров, соответствующие 100%.
static void SetCursSource(Channel chann);                               ///< Установить источник курсорных измерений.
static void IncCursCntrlU(Channel chan);                                ///< Выбрать следующий курсор.
static void IncCursCntrlT(Channel chan);                                ///< Выбрать следующий курсор.
static void SetCursPosU(Channel chan, int numCur, float pos);           ///< Установить позицию курсора напряжения.
static void SetCursPosT(Channel chan, int numCur, float pos);           ///< Установить значение курсора по времени.


#define MAX_POS_U   200
#define MAX_POS_T   280

void *PageCursors::PageSet::pointer = (void *)&mspSet;


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetCursSource(Channel chan)
{
    CURS_SOURCE = chan;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void IncCursCntrlU(Channel chan)
{
    CircleIncreaseInt8((int8*)&CURsU_CNTRL_CH(chan), 0, 3);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void IncCursCntrlT(Channel chan)
{
    CircleIncreaseInt8((int8*)&CURS_CNTRL_T(chan), 0, 3);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Cursors_Update()
{
    Channel source = CURS_SOURCE;
    CursLookMode lookMode0 = CURS_LOOKMODE_0;
    CursLookMode lookMode1 = CURS_LOOKMODE_1;

    int posT0 = 0, posT1 = 0;

    if((lookMode0 == CursLookMode_Voltage || lookMode0 == CursLookMode_Both) && CURS_ACTIVE_IS_T)
    {
        int posU0 = Processing::GetCursU(source, CURS_POS_T0(source));
        SetCursPosU(source, 0, posU0);
    }
    if((lookMode1 == CursLookMode_Voltage || lookMode1 == CursLookMode_Both)  && CURS_ACTIVE_IS_T)
    {
        int posU1 = Processing::GetCursU(source, CURS_POS_T1(source));
        SetCursPosU(source, 1, posU1);
    }
    if((lookMode0 == CursLookMode_Time || lookMode0 == CursLookMode_Both) && CURS_ACTIVE_IS_U)
    {
        int posU0 = CURS_POS_U0(source);
        posT0 = Processing::GetCursT(source, posU0, 0);
        SetCursPosT(source, 0, posT0);
    }
    if((lookMode1 == CursLookMode_Time || lookMode1 == CursLookMode_Both) && CURS_ACTIVE_IS_U)
    {
        int posU1 = CURS_POS_U1(source);
        posT1 = Processing::GetCursT(source, posU1, 1);
        SetCursPosT(source, 1, posT1);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetCursPosU(Channel chan, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_U(chan, numCur) = LimitationFloat(pos, 0, MAX_POS_U);
    }
    else
    {
        CURS_POS_U(chan, numCur) = LimitationFloat(pos, 0, MAX_POS_U);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SetCursPosT(Channel chan, int numCur, float pos)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_T(chan, numCur) = LimitationFloat(pos, 0, MAX_POS_T);
    }
    else
    {
        CURS_POS_T(chan, numCur) = LimitationFloat(pos, 0, MAX_POS_T);
    }
}


extern const Page mainPage;

// КУРСОРЫ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsCursors =
{
    (void*)&mcShow,             // КУРСОРЫ - Показывать
    (void*)&mcTrackingT1U1,     // КУРСОРЫ - Курсоры T1,U1
    (void*)&mcTrackingT2U2,     // КУРСОРЫ - Курсоры T2,U2
    (void*)&mcShowFreq,         // КУРОСРЫ - 1/dT
    (void*)&mspSet              // КУРСОРЫ - УСТАНОВИТЬ
};

static const Page pCursors                ///< КУРСОРЫ
(
    &mainPage, 0,
    "КУРСОРЫ", "CURSORS",
    "Курсорные измерения.",
    "Cursor measurements.",
    Page_Cursors, &itemsCursors
);

void *PageCursors::GetPointer()
{
    return (void *)&pCursors;
}

// КУРСОРЫ - Показывать ------------------------------------------------------------------------------------------------------------------------------
static const Choice mcShow =
{
    Item_Choice, &pCursors, 0,
    {
        "Показывать", "Shown",
        "Включает/отключает курсоры.",
        "Enable/disable cursors."
    },
    {
        {"Нет", "No"},
        {"Да",  "Yes"}
    },
    (int8*)&CURS_SHOW
};


// КУРСОРЫ - Курсоры T1,U1 ---------------------------------------------------------------------------------------------------------------------------
static const Choice mcTrackingT1U1 =
{
    Item_Choice, &pCursors, 0,
    {
        "Слежение \x8e, \x9e",  "Tracking \x8e, \x9e"
        ,
        "Задаёт режим слежения за первым курсором времени и напряжения:\n"
        "1. \"Откл\" - курсор времени и курсор напряжения устанавливаются вручную.\n"
        "2. \"Напряжение\" - при ручном изменении положения курсора времени курсор напряжения автоматически отслеживают изменения сигнала.\n"
        "3. \"Время\" - при ручном изменении положения курсора напряжения курсор времени автоматически отслеживают изменения сигнала.\n"
        "4. \"Напряж и время\" - действует как один из предыдущих режимов, в зависимости от того, на какой курсор производилось последнее воздействие."
        ,
        "Sets the mode tracking for second cursor:\n"                   // WARN Перевод
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"Напряжение",      "Voltage"},
        {"Время",           "Time"},
        {"Напряж и время",  "Volt and time"}
    },
    (int8*)&CURS_LOOKMODE_0
};


// КУРСОРЫ - Курсоры T2,U2 ---------------------------------------------------------------------------------------------------------------------------
static const Choice mcTrackingT2U2 =
{
    Item_Choice, &pCursors, 0,
    {
        "Слежение \x8f, \x9f", "Tracking \x8f, \x9f"
        ,
        "Задаёт режим слежения за вторым курсором времени и напряжения:\n"
        "1. \"Откл\" - курсор времени и курсор напряжения устанавливаются вручную.\n"
        "2. \"Напряжение\" - при ручном изменении положения курсора времени курсор напряжения автоматически отслеживают изменения сигнала.\n"
        "3. \"Время\" - при ручном изменении положения курсора напряжения курсор времени автоматически отслеживают изменения сигнала.\n"
        "4. \"Напряж и время\" - действует как один из предыдущих режимов, в зависимости от того, на какой курсор производилось последнее воздействие."
        ,
        "Sets the mode tracking cursors channel 2:\n"
        "1. \"Disable\" - all cursors are set manually.\n"
        "2. \"Voltage\" - when manually changing the position of the cursor time cursors voltage automatically track changes in the signal.\n"
        "3. \"Time\" - when manually changing the position of the cursor voltage cursors time automatically track changes in the signal.\n"
        "4. \"Volt and time\" - acts as one of the previous modes, depending on which was carried out last effect cursors."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"Напряжение",      "Voltage"},
        {"Время",           "Time"},
        {"Напряж. и время", "Volt. and time"}
    },
    (int8*)&CURS_LOOKMODE_1
};


// КУРОСРЫ - 1/dT ------------------------------------------------------------------------------------------------------------------------------------
static const Choice mcShowFreq =
{
    Item_Choice, &pCursors, 0,
    {
        "1/dT", "1/dT"
        ,
        "Если выбрано \"Вкл\", в правом верхнем углу выводится величина, обратная расстоянию между курсорами времени - частота сигнала, один период "
        "которого равен расстоянию между временными курсорами."
        ,
        "If you select \"Enable\" in the upper right corner displays the inverse of the distance between cursors time - frequency signal, a period "
        "equal to the distance between the time cursors."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&CURSORS_SHOW_FREQ
};


// КУРСОРЫ - УСТАНОВИТЬ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsSet =
{
    (void*)&sbSetExit,              // КУРСОРЫ - УСТАНОВИТЬ - Выход
    (void*)&sbSetSource,            // КУРСОРЫ - УСТАНОВИТЬ - Источник
    (void*)&sbSetU,                 // КУРСОРЫ - УСТАНОВИТЬ - Курсоры U
    (void*)&sbSetT,                 // КУРСОРЫ - УСТАНОВИТЬ - Курсоры T
    (void*)&sbSet100,               // КУРСОРЫ - УСТАНОВИТЬ - 100%
    (void*)&sbSetPointsPercents     // КУРСОРЫ - УСТАНОВИТЬ - Перемещение
};

static const Page mspSet
(
    &pCursors, 0,
    "УСТАНОВИТЬ", "SET",
    "Переход в режим курсорных измерений",
    "Switch to cursor measures",
    Page_SB_Curs, &itemsSet, EmptyFuncVV, EmptyFuncVV, OnRotate_RegSet_Set
);

static void OnRotate_RegSet_Set(int angle)
{
    if(CURS_ACTIVE_IS_U)
    {
        MoveCursUonPercentsOrPoints(angle);
    }
    else
    {
        MoveCursTonPercentsOrPoints(angle);
    }
    Sound::RegulatorShiftRotate();
}

static void MoveCursUonPercentsOrPoints(int delta)
{
    CursCntrl cursCntrl = CURsU_CNTRL;

    float value = delta;

    Channel source = CURS_SOURCE;

    if(CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= DELTA_U100(source) / 100.0f;
    }

    if(cursCntrl == CursCntrl_1 || cursCntrl == CursCntrl_1_2)
    {
        SetShiftCursPosU(source, 0, value);
    }
    if(cursCntrl == CursCntrl_2 || cursCntrl == CursCntrl_1_2)
    {
        SetShiftCursPosU(source, 1, value);
    }
    Cursors_Update();
}

static void SetShiftCursPosU(Channel chan, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_U(chan, numCur) = LimitationFloat(CURS_POS_U(chan, numCur) - delta, 0, MAX_POS_U);
    }
    else
    {
        CURS_POS_U(chan, numCur) = LimitationFloat(CURS_POS_U(chan, numCur) - delta, 0, MAX_POS_U);
    }
}

static void MoveCursTonPercentsOrPoints(int delta)
{
    Channel source = CURS_SOURCE;
    CursCntrl cursCntrl = CURS_CNTRL_T(source);

    float value = delta;

    if(CURS_MOVEMENT_IS_PERCENTS)
    {
        value *= DELTA_T100(source) / 100.0f;
    }

    if(cursCntrl == CursCntrl_1 || cursCntrl == CursCntrl_1_2)
    {
        SetShiftCursPosT(source, 0, value);
    }
    if(cursCntrl == CursCntrl_2 || cursCntrl == CursCntrl_1_2)
    {
        SetShiftCursPosT(source, 1, value);
    }
    Cursors_Update();
}

static void SetShiftCursPosT(Channel chan, int numCur, float delta)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        CURS_POS_T(chan, numCur) = LimitationFloat(CURS_POS_T(chan, numCur) + delta, 0, MAX_POS_T);
    }
    else
    {
        CURS_POS_T(chan, numCur) = LimitationFloat(CURS_POS_T(chan, numCur) + delta, 0, MAX_POS_T);
    }
}


// КУРСОРЫ - УСТАНОВИТЬ - Выход ----------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSetExit
(
    &mspSet,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Cursors_Exit,
    DrawSB_Exit
);

static void PressSB_Cursors_Exit(void)
{
    Display::RemoveAddDrawFunction();
}


// КУРСОРЫ - УСТАНОВИТЬ - Источник ----------------------------------------------------------------------------------------------------------------------
static const arrayHints hintsSetSource =
{
    { DrawSB_Cursors_SourceA, "канал 1", "channel 1" }, { DrawSB_Cursors_SourceB, "канал 2", "channel 2" }
};

static const SmallButton sbSetSource
(
    &mspSet, 0,
    "Источник", "Source",
    "Выбор канала для курсорных измерений",
    "Channel choice for measurements",
    PressSB_Cursors_Source,
    DrawSB_Cursors_Source,
    &hintsSetSource
);

static void PressSB_Cursors_Source(void)
{
    Channel source = CURS_SOURCE_A ? B : A;
    SetCursSource(source);
}

static void DrawSB_Cursors_Source(int x, int y)
{
    CURS_SOURCE_A ? DrawSB_Cursors_SourceA(x, y) : DrawSB_Cursors_SourceB(x, y);
}

static void DrawSB_Cursors_SourceA(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, "1");
}

static void DrawSB_Cursors_SourceB(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, "2");
}


// КУРСОРЫ - УСТАНОВИТЬ - Курсоры U ------------------------------------------------------------------------------------------------------------------
static const arrayHints hintsSetU =
{
    { DrawSB_Cursors_U_Disable,     "курсоры напряжения выключены",
                                    "cursors of tension are switched off" },
    { DrawSB_Cursors_U_Both_Disable,"курсоры напряжения включены",
                                    "cursors of tension are switched on" },
    { DrawSB_Cursors_U_Upper,       "курсоры напряжения включены, управление верхним курсором",
                                    "cursors of tension are switched on, control of the top cursor" },
    { DrawSB_Cursors_U_Lower,       "курсоры напряжения включены, управление нижним курсором",
                                    "cursors of tension are switched on, control of the lower cursor" },
    { DrawSB_Cursors_U_Both_Enable, "курсоры напряжения включены, управление обоими курсорами",
                                    "cursors of tension are switched on, control of both cursors" }
};

static const SmallButton sbSetU
(
    &mspSet, 0,
    "Курсоры U", "Cursors U",
    "Выбор курсоров напряжения для индикации и управления",
    "Choice of cursors of voltage for indication and management",
    PressSB_Cursors_U,
    DrawSB_Cursors_U,
    &hintsSetU
);

static void PressSB_Cursors_U(void)
{
    if (CURS_ACTIVE_IS_U || CURS_CNTRL_U_IS_DISABLE(CURS_SOURCE))
    {
        IncCursCntrlU(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive_U;
}

static void DrawSB_Cursors_U(int x, int y)
{
    CursCntrl cursCntrl = CURsU_CNTRL;
    if (cursCntrl == CursCntrl_Disable)
    {
       DrawSB_Cursors_U_Disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_IS_U)
        {
            DrawSB_Cursors_U_Both_Disable(x, y);
        }
        else
        {
            Channel source = CURS_SOURCE;
            bool condTop = false, condDown = false;
            CalculateConditions(sCursors_GetCursPosU(source, 0), sCursors_GetCursPosU(source, 1), cursCntrl, &condTop, &condDown);
            if (condTop && condDown)
            {
                DrawSB_Cursors_U_Both_Enable(x, y);
            }
            else if (condTop)
            {
                DrawSB_Cursors_U_Upper(x, y);
            }
            else
            {
                DrawSB_Cursors_U_Lower(x, y);
            }
        }
        /*
        if (set.cursors.lookMode[0] == CursLookMode_Voltage || set.cursors.lookMode[0] == CursLookMode_Both ||
            set.cursors.lookMode[1] == CursLookMode_Voltage || set.cursors.lookMode[1] == CursLookMode_Both)
        {
            Painter::SetFont(TypeFont_5);
            Painter::DrawChar(x + 10, y, 'c');
            Painter::SetFont(TypeFont_8);
        }
        */
    }
}

static void DrawSB_Cursors_U_Disable(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, "U");
}

static void DrawSB_Cursors_U_Upper(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, true, false);
}

static void DrawSB_Cursors_U_Lower(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, false, true);
}

static void DrawSB_Cursors_U_Both_Enable(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, true, true);
}

static void DrawSB_Cursors_U_Both_Disable(int x, int y)
{
    DrawMenuCursVoltage(x + 7, y + 5, false, false);
}


// КУРСОРЫ - УСТАНОВИТЬ - Курсоры T ------------------------------------------------------------------------------------------------------------------
static const arrayHints hintsSetT =
{
    { DrawSB_Cursors_T_Disable,         "курсоры времени выключены",
                                        "cursors of time are switched off" },
    { DrawSB_Cursors_T_Both_Disable,    "курсоры времени включены",
                                        "cursors of time are switched on" },
    { DrawSB_Cursors_T_Left,            "курсоры времени включены, управление левым курсором",
                                        "cursors of time are switched on, control of the left cursor" },
    { DrawSB_Cursors_T_Right,           "курсоры времени включены, управление правым курсором",
                                        "cursors of time are switched on, control of the right cursor" },
    { DrawSB_Cursors_T_Both_Enable,     "курсоры времени включены, управление обоими курсорами",
                                        "cursors of time are switched on, control of both cursors" }    
};

static const SmallButton sbSetT
(
    &mspSet, 0,
    "Курсоры T", "Cursors T",
    "Выбор курсоров времени для индикации и управления",
    "Choice of cursors of time for indication and management",
    PressSB_Cursors_T,
    DrawSB_Cursors_T,
    &hintsSetT
);

static void PressSB_Cursors_T(void)
{
    if (CURS_ACTIVE_IS_T || CURS_CNTRL_T_IS_DISABLE(CURS_SOURCE))
    {
        IncCursCntrlT(CURS_SOURCE);
    }
    CURS_ACTIVE = CursActive_T;
}

static void DrawSB_Cursors_T(int x, int y)
{
    Channel source = CURS_SOURCE;
    CursCntrl cursCntrl = CURS_CNTRL_T(source);
    if (cursCntrl == CursCntrl_Disable)
    {
        DrawSB_Cursors_T_Disable(x, y);
    }
    else
    {
        if (!CURS_ACTIVE_IS_T)
        {
            DrawSB_Cursors_T_Both_Disable(x, y);
        }
        else
        {
            bool condLeft = false, condDown = false;
            CalculateConditions(CURS_POS_T0(source), CURS_POS_T1(source), cursCntrl, &condLeft, &condDown);
            if (condLeft && condDown)
            {
                DrawSB_Cursors_T_Both_Enable(x, y);
            }
            else if (condLeft)
            {
                DrawSB_Cursors_T_Left(x, y);
            }
            else
            {
                DrawSB_Cursors_T_Right(x, y);
            }
        }
    }
}

static void DrawSB_Cursors_T_Disable(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, "T");
}

static void DrawSB_Cursors_T_Both_Disable(int x, int y)
{
    DrawMenuCursTime(x, y, false, false);
}

static void DrawSB_Cursors_T_Left(int x, int y)
{
    DrawMenuCursTime(x, y, true, false);
}

static void DrawSB_Cursors_T_Right(int x, int y)
{
    DrawMenuCursTime(x, y, false, true);
}

static void DrawSB_Cursors_T_Both_Enable(int x, int y)
{
    DrawMenuCursTime(x, y, true, true);
}


// КУРСОРЫ - УСТАНОВИТЬ - 100% -----------------------------------------------------------------------------------------------------------------------
static const SmallButton sbSet100
(
    &mspSet, 0,
    "100%", "100%",
    "Используется для процентных измерений. Нажатие помечает расстояние между активными курсорами как 100%",
    "It is used for percentage measurements. Pressing marks distance between active cursors as 100%",
    PressSB_Cursors_100,
    DrawSB_Cursors_100
);

static void PressSB_Cursors_100(void)
{
    SetCursPos100(CURS_SOURCE);
}

static void SetCursPos100(Channel chan)
{
    DELTA_U100(chan) = fabs(CURS_POS_U0(chan) - CURS_POS_U1(chan));
    DELTA_T100(chan) = fabs(CURS_POS_T0(chan) - CURS_POS_T1(chan));
}

static void DrawSB_Cursors_100(int x, int y)
{
    Painter::SetFont(TypeFont_5);
    Painter::DrawText(x + 2, y + 3, "100%");
    Painter::SetFont(TypeFont_8);
}


// КУРСОРЫ - УСТАНОВИТЬ - Перемещение ----------------------------------------------------------------------------------------------------------------
static const arrayHints hintsSetPointsPercents =
{
    { DrawSB_Cursors_PointsPercents_Percents,   "шаг перемещения курсоров кратен одному проценту",
                                                "the step of movement of cursors is multiple to one percent" },
    { DrawSB_Cursors_PointsPercents_Points,     "шаг перемещения курсора кратен одному пикселю",
                                                "the step of movement of the cursor is multiple to one pixel" }
};

static const SmallButton sbSetPointsPercents
(
    &mspSet, 0,
    "Перемещение", "Movement",
    "Выбор шага перемещения курсоров - проценты или точки",
    "Choice of a step of movement of cursors - percent or points",
    PressSB_Cursors_PointsPercents,
    DrawSB_Cursors_PointsPercents,
    &hintsSetPointsPercents
);

static void PressSB_Cursors_PointsPercents(void)
{
    CircleIncreaseInt8((int8*)&CURS_MOVEMENT, 0, 1);
}

static void DrawSB_Cursors_PointsPercents(int x, int y)
{
    if (CURS_MOVEMENT_IS_PERCENTS)
    {
        DrawSB_Cursors_PointsPercents_Percents(x, y);
    }
    else
    {
        DrawSB_Cursors_PointsPercents_Points(x, y);
    }
}

static void DrawSB_Cursors_PointsPercents_Percents(int x, int y)
{
    Painter::DrawText(x + 6, y + 5, "\x83");
}

static void DrawSB_Cursors_PointsPercents_Points(int x, int y)
{
    Painter::SetFont(TypeFont_5);
    Painter::DrawText(x + 4, y + 3, "тчк");
    Painter::SetFont(TypeFont_8);
}


/** @}  @}
 */
