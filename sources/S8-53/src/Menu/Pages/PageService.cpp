#include "Definition.h"
#include "Menu/Pages/PageService.h"
#include "Display/Symbols.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Panel/Panel.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageService
 *  @{
 */

extern const Page pService;


extern const Button       bResetSettings;               ///< СЕРВИС - Сброс настроек
static void        OnPress_ResetSettings();
static void FuncDraw();
static void OnTimerDraw();
extern const Button       bAutoSearch;                  ///< СЕРВИС - Поиск сигнала
static void        OnPress_AutoSearch();
extern const Page        ppCalibrator;                  ///< СЕРВИС - КАЛИБРАТОР
extern const Choice       cCalibrator_Mode;             ///< СЕРВИС - КАЛИБРАТОР - Калибратор
static void      OnChanged_Calibrator_Mode(bool active);
extern const Button       cCalibrator_Calibrate;        ///< СЕРВИС - КАЛИБРАТОР - Калибровать
static void        OnPress_Calibrator_Calibrate();
extern const Page        ppMath;                        ///< СЕРВИС - МАТЕМАТИКА
extern const Page       pppMath_Function;               ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
static bool       IsActive_Math_Function();
static void        OnPress_Math_Function();
static void       OnRegSet_Math_Function(int delta);
extern const SmallButton sbMath_Function_Exit;          ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Выход
extern const SmallButton sbMath_Function_ModeDraw;      ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Экран
static void        OnPress_Math_Function_ModeDraw();
static void           Draw_Math_Function_ModeDraw(int x, int y);
static void           Draw_Math_Function_ModeDraw_Disable(int x, int y);
static void           Draw_Math_Function_ModeDraw_Separate(int x, int y);
static void           Draw_Math_Function_ModeDraw_Together(int x, int y);
extern const SmallButton sbMath_Function_Type;          ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Вид
static void        OnPress_Math_Function_Type();
static void           Draw_Math_Function_Type(int x, int y);
static void           Draw_Math_Function_Type_Sum(int x, int y);
static void           Draw_Math_Function_Type_Mul(int x, int y);
extern const SmallButton sbMath_Function_ModeRegSet;    ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Режим ручки УСТАНОВКА
static void        OnPress_Math_Function_ModeRegSet();
static void           Draw_Math_Function_ModeRegSet(int x, int y);
static void           Draw_Math_Function_ModeRegSet_Range(int x, int y);
static void           Draw_Math_Function_ModeRegSet_RShift(int x, int y);
extern const SmallButton sbMath_Function_RangeA;        ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 1-го канала
static void        OnPress_Math_Function_RangeA();
static void           Draw_Math_Function_RangeA(int x, int y);
extern const SmallButton sbMath_Function_RangeB;        ///< СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 2-го канала
static void        OnPress_Math_Function_RangeB();
static void           Draw_Math_Function_RangeB(int x, int y);
extern const Page       pppMath_FFT;                    ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР
static void        OnPress_Math_FFT();
static bool       IsActive_Math_FFT();
extern const Choice       cMath_FFT_Enable;             ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - Отображение
extern const Choice       cMath_FFT_Scale;              ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - Шкала
extern const Choice       cMath_FFT_Source;             ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - Источник
extern const Choice       cMath_FFT_Window;             ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - Окно
extern const Page      ppppMath_FFT_Cursors;            ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ
static bool       IsActive_Math_FFT_Cursors();
static void       OnRegSet_Math_FFT_Cursors(int angle);
extern const SmallButton  cMath_FFT_Cursors_Exit;        ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Выход
static void        OnPress_Math_FFT_Cursors_Exit();
extern const SmallButton  cMath_FFT_Cursors_Source;      ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Источник
static void        OnPress_Math_FFT_Cursors_Source();
static void           Draw_Math_FFT_Cursors_Source(int x, int y);
extern const Choice       cMath_FFT_Limit;              ///< СЕРВИС - МАТЕМАТИКА - СПЕКТР - Диапазон
static bool       IsActive_Math_FFT_Limit();
extern const Page        ppEthernet;                    ///< СЕРВИС - ETHERNET
extern const Choice       cEthernet_Enable;             ///< СЕРВИС - ETHERNET - Ethernet
static void      OnChanged_Ethernet_Enable(bool active);
extern const IPaddress   ipEthernet_IP;                 ///< СЕРВИС - ETHERNET - IP адрес
extern const IPaddress   ipEthernet_Mask;               ///< СЕРВИС - ETHERNET - Маска подсети
extern const IPaddress   ipEthernet_Gateway;            ///< СЕРВИС - ETHERNET - Шлюз
extern const MACaddress macEthernet_MAC;                ///< СЕРВИС - ETHERNET - Физ адрес
extern const Choice       cSound;                       ///< СЕРВИС - Звук
extern const Choice       cLang;                        ///< СЕРВИС - Язык
extern const Time         tTime;                        ///< СЕРВИС - Время
extern const Choice       cModeLongPressButtonTrig;     ///< СЕРВИС - Реж длит СИНХР
extern const Page        ppInformation;                 ///< СЕРВИС - ИНФОРМАЦИЯ
static void        OnPress_Information();
static void Information_Draw();
extern const SmallButton sbInformation_Exit;             ///< СЕРВИС - ИНФОРМАЦИЯ - Выход
static void        OnPress_Information_Exit();

extern Page mainPage;

// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsService =
{
    (void*)&bResetSettings,             // СЕРВИС - Сброс настроек
    (void*)&bAutoSearch,                // СЕРВИС - Поиск сигнала
    (void*)&ppCalibrator,               // СЕРВИС - КАЛИБРАТОР
    (void*)&ppMath,                     // СЕРВИС - МАТЕМАТИКА
    (void*)&ppEthernet,                 // СЕРВИС - ETHERNET
    (void*)&cSound,                     // СЕРВИС - Звук
    (void*)&cLang,                      // СЕРВИС - Язык
    (void*)&tTime,                      // СЕРВИС - Время
    (void*)&cModeLongPressButtonTrig,   // СЕРВИС - Реж длит СИНХР
    (void*)&ppInformation               // СЕРВИС - ИНФОРМАЦИЯ
};

const Page pService                     ///< СЕРВИС
(
    &mainPage, 0,
    "СЕРВИС", "SERVICE",
    "Дополнительные настройки, калибровка, поиск сигнала, математические функции",
    "Additional settings, calibration, signal search, mathematical functions",
    Page_Service, &itemsService
);

// СЕРВИС - Сброс настроек ---------------------------------------------------------------------------------------------------------------------------
static const Button bResetSettings
(
    &pService, 0,
    "Сброс настроек", "Reset settings",
    "Сброс настроек на настройки по умолчанию",
    "Reset to default settings",
    OnPress_ResetSettings
);

static void OnPress_ResetSettings(void)
{
    Panel::Disable();
    Display::SetDrawMode(DrawMode_Hand, FuncDraw);
    Timer::Enable(kTimerDrawHandFunction, 100, OnTimerDraw);

    if (Panel::WaitPressingButton() == B_Start)
    {
        Settings::Load(true);
        FPGA::Init();
    }

    Timer::Disable(kTimerDrawHandFunction);
    Display::SetDrawMode(DrawMode_Auto, 0);
    Panel::Enable();
}

static void FuncDraw(void)
{
    Painter::BeginScene(COLOR_BACK);

    Painter::DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                         "Нажмите любую другую кнопку, если сброс не нужен.", COLOR_FILL);

    Painter::EndScene();
}

static void OnTimerDraw(void)
{
    Display::Update();
}


// СЕРВИС - Поиск сигнала ----------------------------------------------------------------------------------------------------------------------------
static const Button bAutoSearch
(
    &pService, 0,
    "Поиск сигнала", "Find signal",
    "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
    "Sets optimal settings for the oscilloscope signal on channel 1",
    OnPress_AutoSearch
);

static void OnPress_AutoSearch(void)
{
    FPGA::StartAutoFind();
};

// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsCalibrator =
{
    (void*)&cCalibrator_Mode,       // СЕРВИС - КАЛИБРАТОР - Калибратор
    (void*)&cCalibrator_Calibrate   // СЕРВИС - КАЛИБРАТОР - Калибровать
};

static const Page ppCalibrator
(
    &pService, 0,
    "КАЛИБРАТОР", "CALIBRATOR",
    "Управлением калибратором и калибровка осциллографа",
    "Control of the calibrator and calibration of an oscillograph",
    Page_ServiceCalibrator, &itemsCalibrator
);

// СЕРВИС - КАЛИБРАТОР - Калибратор ------------------------------------------------------------------------------------------------------------------
static const Choice cCalibrator_Mode =
{
    Item_Choice, &ppCalibrator, 0,
    {
        "Калибратор",  "Calibrator",
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    {
        {"Перем",   "DC"},
        {"Пост",    "AC"},
        {"0В",      "OV"}
    },
    (int8*)&CALIBRATOR, OnChanged_Calibrator_Mode
};

static void OnChanged_Calibrator_Mode(bool active)
{
    FPGA::SetCalibratorMode(CALIBRATOR);
}

// СЕРВИС - КАЛИБРАТОР - Калибровать -----------------------------------------------------------------------------------------------------------------
static const Button cCalibrator_Calibrate
(
    &ppCalibrator, 0,
    "Калибровать", "Calibrate",
    "Запуск процедуры калибровки",
    "Running the calibration procedure",
    OnPress_Calibrator_Calibrate
);

static void OnPress_Calibrator_Calibrate(void)
{
    gStateFPGA.needCalibration = true;
}

// СЕРВИС - МАТЕМАТИКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsMath =
{
    (void*)&pppMath_Function,     // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
    (void*)&pppMath_FFT           // СЕРВИС - МАТЕМАТИКА - СПЕКТР
};

static const Page ppMath
(
    &pService, 0,
    "МАТЕМАТИКА", "MATH",
    "Математические функции и БПФ",
    "Mathematical functions and FFT",
    Page_Math, &itemsMath
);

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsMath_Function =
{
    (void*)&sbMath_Function_Exit,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Выход
    (void*)&sbMath_Function_ModeDraw,   // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Экран
    (void*)&sbMath_Function_Type,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Вид
    (void*)&sbMath_Function_ModeRegSet, // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Режим ручки УСТАНОВКА
    (void*)&sbMath_Function_RangeA,     // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 1-го канала
    (void*)&sbMath_Function_RangeB      // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 2-го канала    
};

static const Page pppMath_Function
(
    &ppMath, IsActive_Math_Function,
    "ФУНКЦИЯ", "FUNCTION",
    "Установка и выбор математической функции - сложения или умножения",
    "Installation and selection of mathematical functions - addition or multiplication",
    Page_SB_MathFunction, &itemsMath_Function, OnPress_Math_Function, EmptyFuncVV, OnRegSet_Math_Function
);

void *PageService::Math::Function::GetPointer()
{
    return (void *)&pppMath_Function;
}

static bool IsActive_Math_Function(void)
{
    return !ENABLED_FFT;
}

static void OnPress_Math_Function(void)
{
    if (ENABLED_FFT)
    {
        Display::ShowWarningBad(ImpossibleEnableMathFunction);
    }
}

static void OnRegSet_Math_Function(int delta)
{
    if (DISABLED_DRAW_MATH)
    {
        return;
    }

    if (MATH_MODE_REG_SET_IS_RSHIFT)
    {
        int16 prevRShift = SET_RSHIFT_MATH;
        int16 rShift = prevRShift;
        if (delta > 0)
        {
            if (rShift < RShiftMax)
            {
                rShift += 4 * STEP_RSHIFT;
                LIMIT_ABOVE(rShift, RShiftMax);
                if (prevRShift < RShiftZero && rShift > RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound::RegulatorShiftRotate();
                SET_RSHIFT_MATH = rShift;
            }
        }
        else if (delta < 0)
        {
            if (rShift > RShiftMin)
            {
                rShift -= 4 * STEP_RSHIFT;
                LIMIT_BELOW(rShift, RShiftMin);
                if (prevRShift > RShiftZero && rShift < RShiftZero)
                {
                    rShift = RShiftZero;
                }
                Sound::RegulatorShiftRotate();
                SET_RSHIFT_MATH = rShift;
            }
        }
    }
    if (MATH_MODE_REG_SET_IS_RANGE)
    {
        static int sum = 0;
        sum -= delta;

        float rShiftAbs = RSHIFT_2_ABS(SET_RSHIFT_MATH, SET_RANGE_MATH);

        if (sum > 2)
        {
            if (SET_RANGE_MATH < RangeSize - 1)
            {
                ++SET_RANGE_MATH;
                SET_RSHIFT_MATH = (int16)Math_RShift2Rel(rShiftAbs, SET_RANGE_MATH);
                Sound::RegulatorSwitchRotate();
            }
            sum = 0;
        }
        else if (sum < -2)
        {
            if (SET_RANGE_MATH > 0)
            {
                --SET_RANGE_MATH;
                SET_RSHIFT_MATH = (int16)Math_RShift2Rel(rShiftAbs, SET_RANGE_MATH);
                Sound::RegulatorSwitchRotate();
            }
            sum = 0;
        }
    }
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Выход -------------------------------------------------------------------------------------------------------------
static const SmallButton sbMath_Function_Exit
(
    &pppMath_Function,
    COMMON_BEGIN_SB_EXIT,
    EmptyFuncVV,
    DrawSB_Exit
);

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Экран -------------------------------------------------------------------------------------------------------------
static const arrayHints hintsMath_Function_ModeDraw =
{
    {Draw_Math_Function_ModeDraw_Disable,  "Вывод математической функции отключён",
                                            "The conclusion of mathematical function is disconnected"},
    {Draw_Math_Function_ModeDraw_Separate, "Сигналы и математическая функция выводятся в разных окнах",
                                            "Signals and mathematical function are removed in different windows"},
    {Draw_Math_Function_ModeDraw_Together, "Сигналы и математическая функция выводятся в одном окне",
                                            "Signals and mathematical function are removed in one window"}
};

static const SmallButton sbMath_Function_ModeDraw
(
    &pppMath_Function, 0,
    "Экран", "Display",
    "Выбирает режим отображения математического сигнала",
    "Chooses the mode of display of a mathematical signal",
    OnPress_Math_Function_ModeDraw,
    Draw_Math_Function_ModeDraw,
    &hintsMath_Function_ModeDraw
);

static void OnPress_Math_Function_ModeDraw(void)
{
    if (ENABLED_FFT)
    {
        Display::ShowWarningBad(ImpossibleEnableMathFunction);
    }
    else
    {
        CircleIncreaseInt8((int8*)&MODE_DRAW_MATH, 0, 2);
    }
}

static void Draw_Math_Function_ModeDraw(int x, int y)
{
    static const pFuncVII funcs[3] =
    {
        Draw_Math_Function_ModeDraw_Disable,
        Draw_Math_Function_ModeDraw_Separate,
        Draw_Math_Function_ModeDraw_Together
    };
    funcs[MODE_DRAW_MATH](x, y);
}

static void Draw_Math_Function_ModeDraw_Disable(int x, int y)
{
    Painter::DrawText(x + 2 + (set.common.lang == English ? 2 : 0), y + 5, set.common.lang == Russian ? "Вык" : "Dis");
}

static void Draw_Math_Function_ModeDraw_Separate(int x, int y)
{
    Painter::DrawRectangle(x + 3, y + 5, 13, 9);
    Painter::DrawHLine(y + 9, x + 3, x + 16);
    Painter::DrawHLine(y + 10, x + 3, x + 16);
}

static void Draw_Math_Function_ModeDraw_Together(int x, int y)
{
    Painter::DrawRectangle(x + 3, y + 5, 13, 9);
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Вид ---------------------------------------------------------------------------------------------------------------
static const arrayHints hintsMath_Function_Type =
{
    { Draw_Math_Function_Type_Sum,      "Сложение",     "Addition"       },
    { Draw_Math_Function_Type_Mul,      "Умножение",    "Multiplication" }
};

static const SmallButton sbMath_Function_Type
(
    &pppMath_Function, 0,
    "Вид", "Type",
    "Выбор математической функции",
    "Choice of mathematical function",
    OnPress_Math_Function_Type,
    Draw_Math_Function_Type,
    &hintsMath_Function_Type
);

static void OnPress_Math_Function_Type(void)
{
    CircleIncreaseInt8((int8*)&MATH_FUNC, 0, 1);
}

static void Draw_Math_Function_Type(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_Type_Sum, Draw_Math_Function_Type_Mul};
    funcs[MATH_FUNC](x, y);
}

static void Draw_Math_Function_Type_Sum(int x, int y)
{
    Painter::DrawHLine(y + 9, x + 4, x + 14);
    Painter::DrawVLine(x + 9, y + 4, y + 14);
}

static void Draw_Math_Function_Type_Mul(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 4, y + 3, SYMBOL_MATH_FUNC_MUL);
    Painter::SetFont(TypeFont_8);
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Режим ручки УСТАНОВКА ---------------------------------------------------------------------------------------------
static const arrayHints hintsMath_Function_ModeRegSet =
{
    {Draw_Math_Function_ModeRegSet_Range,  "Управление масштабом", "Management of scale"},
    {Draw_Math_Function_ModeRegSet_RShift, "Управление смещением", "Management of shift"}
};

static const SmallButton sbMath_Function_ModeRegSet
(
    &pppMath_Function, 0,
    "Режим ручки УСТАНОВКА", "Mode regulator SET",
    "Выбор режима ручки УСТАНОВКА - управление масштабом или смещением",
    "Choice mode regulcator УСТАНОВКА - management of scale or shift",
    OnPress_Math_Function_ModeRegSet,
    Draw_Math_Function_ModeRegSet,
    &hintsMath_Function_ModeRegSet
);

static void OnPress_Math_Function_ModeRegSet(void)
{
    CircleIncreaseInt8((int8*)&MATH_MODE_REG_SET, 0, 1);
}

static void Draw_Math_Function_ModeRegSet(int x, int y)
{
    const pFuncVII funcs[2] = {Draw_Math_Function_ModeRegSet_Range, Draw_Math_Function_ModeRegSet_RShift};
    funcs[MATH_MODE_REG_SET](x, y);
}

static void Draw_Math_Function_ModeRegSet_Range(int x, int y)
{
    Painter::DrawChar(x + 7, y + 5, set.common.lang == Russian ? 'M' : 'S');
}

static void Draw_Math_Function_ModeRegSet_RShift(int x, int y)
{
    Painter::DrawText(x + 5 - (set.common.lang == English ? 3 : 0), y + 5, set.common.lang == Russian ? "См" : "Shif");
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 1-го канала -----------------------------------------------------------------------------------------------
static const SmallButton sbMath_Function_RangeA
(
    &pppMath_Function, 0,
    "Масштаб 1-го канала", "Scale of the 1st channel",
    "Берёт масштаб для математического сигнала из первого канала",
    "Takes scale for a mathematical signal from the first channel",
    OnPress_Math_Function_RangeA,
    Draw_Math_Function_RangeA
);

static void OnPress_Math_Function_RangeA(void)
{
    SET_RANGE_MATH = SET_RANGE_A;
    MATH_MULTIPLIER = SET_DIVIDER_A;
}

static void Draw_Math_Function_RangeA(int x, int y)
{
    Painter::DrawChar(x + 8, y + 5, '1');
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ - Масштаб 2-го канала -----------------------------------------------------------------------------------------------
static const SmallButton sbMath_Function_RangeB
(
    &pppMath_Function, 0,
    "Масштаб 2-го канала", "Scale of the 2nd channel",
    "Берёт масштаб для математического сигнала из второго канала",
    "Takes scale for a mathematical signal from the second channel",
    OnPress_Math_Function_RangeB,
    Draw_Math_Function_RangeB
);

static void OnPress_Math_Function_RangeB(void)
{
    SET_RANGE_MATH = SET_RANGE_B;
    MATH_MULTIPLIER = SET_DIVIDER_B;
}

static void Draw_Math_Function_RangeB(int x, int y)
{
    Painter::DrawChar(x + 8, y + 5, '2');
}

// СЕРВИС - МАТЕМАТИКА - СПЕКТР //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsMath_FFT =
{
    (void*)&cMath_FFT_Enable,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Отображение
    (void*)&cMath_FFT_Scale,        // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Шкала
    (void*)&cMath_FFT_Source,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Источник
    (void*)&cMath_FFT_Window,       // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Окно
    (void*)&ppppMath_FFT_Cursors,   // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ
    (void*)&cMath_FFT_Limit         // СЕРВИС - МАТЕМАТИКА - СПЕКТР - Диапазон
};

static const Page pppMath_FFT
(
    &ppMath, IsActive_Math_FFT,
    "СПЕКТР", "SPECTRUM",
    "Отображение спектра входного сигнала",
    "Mapping the input signal spectrum",
    Page_MathFFT, &itemsMath_FFT, OnPress_Math_FFT
);

static bool IsActive_Math_FFT(void)
{
    return DISABLED_DRAW_MATH;
}

static void OnPress_Math_FFT(void)
{
    if (!IsActive_Math_FFT())
    {
        Display::ShowWarningBad(ImpossibleEnableFFT);
    }
}

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - Отображение --------------------------------------------------------------------------------------------------------
static const Choice cMath_FFT_Enable =
{
    Item_Choice, &pppMath_FFT, 0,
    {
        "Отображение", "Display",
        "Включает и выключает отображение спектра",
        "Enables or disables the display of the spectrum"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&ENABLED_FFT
};

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - Шкала --------------------------------------------------------------------------------------------------------------
static const Choice cMath_FFT_Scale =
{
    Item_Choice, &pppMath_FFT, 0,
    {
        "Шкала", "Scale",
        "Задаёт масштаб вывода спектра - линейный или логарифмический",
        "Sets the scale of the output spectrum - linear or logarithmic"
    },
    {
        {"Логарифм",    "Log"},
        {"Линейная",    "Linear"}
    },
    (int8*)&SCALE_FFT
};

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - Источник -----------------------------------------------------------------------------------------------------------
static const Choice cMath_FFT_Source =
{
    Item_Choice, &pppMath_FFT, 0,
    {
        "Источник", "Source",
        "Выбор источника для расчёта спектра",
        "Selecting the source for the calculation of the spectrum"
    },
    {
        {"Канал 1",     "Channel 1"},
        {"Канал 2",     "Channel 2"},
        {"Канал 1 + 2", "Channel 1 + 2"}
    },
    (int8*)&SOURCE_FFT
};

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - Окно ---------------------------------------------------------------------------------------------------------------
static const Choice cMath_FFT_Window =
{
    Item_Choice, &pppMath_FFT, 0,
    {
        "Окно", "Window",
        "Задаёт окно для расчёта спектра",
        "Sets the window to calculate the spectrum"
    },
    {
        {"Прямоугольн", "Rectangle"},
        {"Хэмминга",    "Hamming"},
        {"Блэкмена",    "Blackman"},
        {"Ханна",       "Hann"}
    },
    (int8*)&WINDOW_FFT
};

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ ------------------------------------------------------------------------------------------------------------
static const arrayItems itemsMath_FFT_Cursors =
{
    (void*)&cMath_FFT_Cursors_Exit,     // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Выход
    (void*)&cMath_FFT_Cursors_Source,   // СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Источник
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0
};

static const Page ppppMath_FFT_Cursors
(
    &pppMath_FFT, IsActive_Math_FFT_Cursors,
    "КУРСОРЫ", "CURSORS",
    "Включает курсоры для измерения параметров спектра",
    "Includes cursors to measure the parameters of the spectrum",
    Page_SB_MathCursorsFFT, &itemsMath_FFT_Cursors, EmptyFuncVV, EmptyFuncVV, OnRegSet_Math_FFT_Cursors
);

void *PageService::Math::FFT::Cursors::GetPointer()
{
    return (void *)&ppppMath_FFT_Cursors;
}

static bool IsActive_Math_FFT_Cursors(void)
{
    return ENABLED_FFT;
}

static void OnRegSet_Math_FFT_Cursors(int angle)
{
    FFT_POS_CURSOR(FFT_CUR_CURSOR) += (uint8)angle;
    Sound::RegulatorShiftRotate();
}

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Выход ----------------------------------------------------------------------------------------------------
static const SmallButton cMath_FFT_Cursors_Exit
(
    &ppppMath_FFT_Cursors,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Math_FFT_Cursors_Exit,
    DrawSB_Exit
);

static void OnPress_Math_FFT_Cursors_Exit(void)
{
    Display::RemoveAddDrawFunction();
}

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - КУРСОРЫ - Источник -------------------------------------------------------------------------------------------------
static const SmallButton cMath_FFT_Cursors_Source
(
    &ppppMath_FFT_Cursors, 0,
    "Источник", "Source",
    "Выбор источника для расчёта спектра",
    "Source choice for calculation of a range",
    OnPress_Math_FFT_Cursors_Source,
    Draw_Math_FFT_Cursors_Source
);

static void OnPress_Math_FFT_Cursors_Source(void)
{
    FFT_CUR_CURSOR = (uint8)((FFT_CUR_CURSOR + 1) % 2);
}

static void Draw_Math_FFT_Cursors_Source(int x, int y)
{
    Painter::DrawText(x + 7, y + 5, FFT_CUR_CURSOR_IS_0 ? "1" : "2");
}

// СЕРВИС - МАТЕМАТИКА - СПЕКТР - Диапазон -----------------------------------------------------------------------------------------------------------
static const Choice cMath_FFT_Limit =
{
    Item_Choice, &pppMath_FFT,  IsActive_Math_FFT_Limit,
    {
        "Диапазон", "Range",
        "Здесь можно задать предел наблюдения за мощностью спектра",
        "Here you can set the limit of monitoring the power spectrum"
    },
    {
        {"-40дБ",   "-40dB"},
        {"-60дБ",   "-60dB"},
        {"-80дБ",   "-80dB"}
    },
    (int8*)&FFT_MAX_DB
};

static bool IsActive_Math_FFT_Limit(void)
{
    return SCALE_FFT_IS_LOG;
}

// СЕРВИС - ETHERNET /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsEthernet =
{
    (void*)&cEthernet_Enable,       // СЕРВИС - ETHERNET - Ethernet
    (void*)&ipEthernet_IP,          // СЕРВИС - ETHERNET - IP адрес
    (void*)&ipEthernet_Mask,        // СЕРВИС - ETHERNET - Маска подсети
    (void*)&ipEthernet_Gateway,     // СЕРВИС - ETHERNET - Шлюз
    (void*)&macEthernet_MAC         // СЕРВИС - ETHERNET - Физ адрес
};

static const Page ppEthernet
(
    &pService, 0,
    "ETHERNET", "ETHERNET",
    "Настройки ethernet",
    "Settings of ethernet",
    Page_ServiceEthernet, &itemsEthernet
);

// СЕРВИС - ETHERNET - Ethernet ----------------------------------------------------------------------------------------------------------------------
static const Choice cEthernet_Enable =
{
    Item_Choice, &ppEthernet, 0,
    {
        "Ethernet", "Ethernet"
        ,
        "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
        "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"Включено",    "Included"},
        {"Отключено",   "Disconnected"}
    },
    (int8*)&ETH_ENABLE, OnChanged_Ethernet_Enable
};

static void OnChanged_Ethernet_Enable(bool active)
{
    Display::ShowWarningGood(NeedRebootDevice2);
    Display::ShowWarningGood(NeedRebootDevice1);
}

// СЕРВИС - ETHERNET - IP адрес ----------------------------------------------------------------------------------------------------------------------
static const IPaddressStruct structIP =
{
    Item_IP, &ppEthernet, 0,
    {   "IP адрес", "IP-address",
        "Установка IP адреса",
        "Set of IP-address",  },
    &IP_ADDR0, &IP_ADDR1, &IP_ADDR2, &IP_ADDR3,
    OnChanged_Ethernet_Enable,
    &PORT
};

static const IPaddress ipEthernet_IP
(
    &structIP
);


// СЕРВИС - ETHERNET - Маска подсети -----------------------------------------------------------------------------------------------------------------
static const IPaddressStruct structMask =
{
    Item_IP, &ppEthernet, 0,
    {   "Маска подсети", "Network mask",
        "Установка маски подсети",
        "Set of network mask",    },
    &NETMASK_ADDR0, &NETMASK_ADDR1, &NETMASK_ADDR2, &NETMASK_ADDR3,
    OnChanged_Ethernet_Enable    
};

static const IPaddress ipEthernet_Mask
(
    &structMask
);

// СЕРВИС - ETHERNET - Шлюз --------------------------------------------------------------------------------------------------------------------------
static const IPaddressStruct structGateway =
{
    Item_IP, &ppEthernet, 0,
    {   "Шлюз", "Gateway",
        "Установка адреса основного шлюза",
        "Set of gateway address", },
    &GW_ADDR0, &GW_ADDR1, &GW_ADDR2, &GW_ADDR3,
    OnChanged_Ethernet_Enable
};

static const IPaddress ipEthernet_Gateway
(
    &structGateway
);

// СЕРВИС - ETHERNET - Физ адрес ---------------------------------------------------------------------------------------------------------------------
static const MACaddress macEthernet_MAC =
{
    Item_MAC, &ppEthernet, 0,
    {
        "Физ адрес", "MAC-address",
        "Установка физического адреса",
        "Set of MAC-address"
    },
    &MAC_ADDR0, &MAC_ADDR1, &MAC_ADDR2, &MAC_ADDR3, &MAC_ADDR4, &MAC_ADDR5,
    OnChanged_Ethernet_Enable
};

// СЕРВИС - Звук -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cSound =
{
    Item_Choice, &pService, 0,
    {
        "Звук", "Sound",
        "Включение/выключение звука",
        "Inclusion/switching off of a sound"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SOUND_ENABLED
};


// СЕРВИС - Язык -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cLang =
{
    Item_Choice, &pService, 0,
    {
        "Язык", "Language",
        "Позволяет выбрать язык меню",
        "Allows you to select the menu language"
    },
    {
        {"Русский",     "Russian"},
        {"Английский",  "English"}
    },
    (int8*)&LANG
};


// СЕРВИС - Время ------------------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time tTime =
{
    Item_Time, &pService, 0,
    {
        "Время", "Time"
        ,
        "Установка текущего времени.\nПорядок работы:\n"
        "Нажать на элемент меню \"Время\". Откроется меню установки текущего времени. Короткими нажатиями кнопки на цифровой клавиатуре, соответсвующей "
        "элементу управления \"Время\", выделить часы, минуты, секунды, год, месяц, или число. Выделенный элемент обозначается мигающей областью. "
        "Вращением ручки УСТАНОВКА установить необходимое значение. Затем выделить пункт \"Сохранить\", нажать и удреживать более 0.5 сек кнопку на панели "
        "управления. Меню установки текущего временя закроется с сохранением нового текущего времени. Нажатие длительное удержание кнопки на любом другом элементе "
        "приведёт к закрытию меню установки текущего времени без сохранения нового текущего времени"
        ,
        "Setting the current time. \nPoryadok work:\n"
        "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
        "Control \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
        "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
        "Control. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention of any other element "
        "will lead to the closure of the current time setting menu without saving the new current time"
    },
    &dServicetime, &hours, &minutes, &secondes, &month, &day, &year
};

// СЕРВИС - Реж длит СИНХР ---------------------------------------------------------------------------------------------------------------------------
static const Choice cModeLongPressButtonTrig =
{
    Item_Choice, &pService, 0,
    {
        "Реж длит СИНХР", "Mode long СИНХР"
        ,
        "Устанавливает действия для длительного нажатия кнопки СИНХР:\n\"Сброс уровня\" - установка уровня синхронизации в ноль,\n\"Автоуровень\" "
        "- автоматическое определение и установка уровня синхронизации"
        ,
        "Sets the action for long press CLOCK:\n\"Reset trig lvl\" - to set the trigger level to zero, \n\"Auto level\""
        "- Automatically detect and install the trigger level"
    },
    {
        {"Сброс уровня",    "Reset trig level"},
        {"Автоуровень",     "Autolevel"}
    },
    (int8*)&MODE_LONG_PRESS_TRIG
};

// СЕРВИС - ИНФОРМАЦИЯ -------------------------------------------------------------------------------------------------------------------------------
static const arrayItems itemsInformation =
{
    (void*)&sbInformation_Exit, // СЕРВИС - ИНФОРМАЦИЯ - Выход
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0,
    (void*)0
};

static const Page ppInformation
(
    &pService, 0,
    "ИНФОРМАЦИЯ", "INFORMATION",
    "Выводит на экран идентификационные данные осциллографа",
    "Displays identification data of the oscilloscope",
    Page_SB_Information, &itemsInformation, OnPress_Information, EmptyFuncVV, EmptyFuncVI
);

void *PageService::Information::GetPointer()
{
    return (void *)&ppInformation;
}

static void OnPress_Information(void)
{
    Menu::OpenPageAndSetItCurrent(PageService::Information::GetPointer());
    Display::SetDrawMode(DrawMode_Hand, Information_Draw);
}

static void Information_Draw(void)
{
    Language lang = LANG;

    Painter::BeginScene(COLOR_BLACK);
    int x = 100;
    int dY = 20;
    int y = 20;
    Painter::DrawRectangleC(0, 0, 319, 239, COLOR_FILL);
    y += dY;
    Painter::DrawText(x, y, lang == Russian ? "ИНФОРМАЦИЯ" : "INFORMATION");
    y += dY;
    Painter::DrawText(x, y, lang == Russian ? "Модель : С8-53/1" : "Model : S8-53/1");
    y += dY;

    char buffer[100];
    /*
    OTP_GetSerialNumber(buffer);
    if (buffer[0])
    {
        Painter::DrawFormatText(x, y, COLOR_FILL, lang == Russian ? "C/Н : %s" : "S/N : %s", buffer);
        y += dY;
    }
    */

    Painter::DrawText(x, y, lang == Russian ? "Программное обеспечение:" : "Software:");
    y += dY;
    sprintf(buffer, (const char*)((lang == Russian) ? "версия %s" : "version %s"), NUM_VER);
    Painter::DrawText(x, y, buffer);
    y += dY;

Painter::DrawFormatText(x, y, COLOR_FILL, "CRC32 : %X", Hardware::CalculateCRC32());

    dY = -10;
    Painter::DrawStringInCenterRect(0, 190 + dY, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter::DrawStringInCenterRect(0, 205 + dY, 320, 20, "Отдел маркетинга: тел./факс. 8-017-270-02-00");
    Painter::DrawStringInCenterRect(0, 220 + dY, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-270-02-23");

    Menu::Draw();
    Painter::EndScene();
}

// СЕРВИС - ИНФОРМАЦИЯ - Выход -----------------------------------------------------------------------------------------------------------------------
const SmallButton sbInformation_Exit
(
    &ppInformation,
    COMMON_BEGIN_SB_EXIT,
    OnPress_Information_Exit,
    DrawSB_Exit
);

static void OnPress_Information_Exit(void)
{
    Display::SetDrawMode(DrawMode_Auto, 0);
    Display::RemoveAddDrawFunction();
}


/** @}  @}
 */
