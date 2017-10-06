// СЕРВИС - МАТЕМАТИКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Utils/GlobalFunctions.h"
#include "Display/Symbols.h"
#include "Definition.h"
#include "Hardware/Sound.h"
#include "Utils/Math.h"

const Page mspCursFFT;
const Page mspServiceMathFunction;
const Page mspFFT;
const Page pMath;

void Reg_FFT_Cursors(int angle)
{
    FFT_POS_CURSOR(FFT_CUR_CURSOR) += angle;
    Sound_RegulatorShiftRotate();
}

void DrawSB_FFTcurs_Source(int x, int y)
{
    Painter_DrawText(x + 7, y + 5, FFT_CUR_CURSOR_IS_0 ? "1" : "2");
}

void PressSB_FFTcurs_Source()
{
    FFT_CUR_CURSOR = (FFT_CUR_CURSOR + 1) % 2;
}

static const SmallButton sbCursFFTSource =
{
    Item_SmallButton, &mspCursFFT, 0,
    {
        "Источник", "Source",
        "Выбор источника для расчёта спектра",
        "Source choice for calculation of a range"
    },
    PressSB_FFTcurs_Source,
    DrawSB_FFTcurs_Source
};

static void PressSB_MathFunction_Type()
{
    CircleIncreaseInt8((int8*)&MATH_FUNC, 0, 1);
}

static void DrawSB_MathFunction_Sum(int x, int y)
{
    Painter_DrawHLine(y + 9, x + 4, x + 14);
    Painter_DrawVLine(x + 9, y + 4, y + 14);
}

static void DrawSB_MathFunction_Mul(int x, int y)
{
    Painter_SetFont(TypeFont_UGO2);
    Painter_Draw4SymbolsInRect(x + 4, y + 3, SYMBOL_MATH_FUNC_MUL);
    Painter_SetFont(TypeFont_8);
}

static void DrawSB_MathFunction_Type(int x, int y)
{
    const pFuncVII funcs[2] = {DrawSB_MathFunction_Sum, DrawSB_MathFunction_Mul};
    funcs[MATH_FUNC](x, y);
}


static const SmallButton sbMathFunction =
{
    Item_SmallButton, &mspServiceMathFunction, 0,
    {
        "Вид", "Type",
        "Выбор математической функции",
        "Choice of mathematical function"
    },
    PressSB_MathFunction_Type,
    DrawSB_MathFunction_Type,
    {
        { DrawSB_MathFunction_Sum,      "Сложение",     "Addition"       },
        { DrawSB_MathFunction_Mul,      "Умножение",    "Multiplication" }
    }
};

static void PressSB_MathFunction_ModeDraw()
{
    if (ENABLED_FFT)
    {
        Display_ShowWarningBad(ImpossibleEnableMathFunction);
    }
    else
    {
        CircleIncreaseInt8((int8*)&MODE_DRAW_MATH, 0, 2);
    }
}

static void DrawSB_MathFunction_ModeDraw_Separate(int x, int y)
{
    Painter_DrawRectangle(x + 3, y + 5, 13, 9);
    Painter_DrawHLine(y + 9, x + 3, x + 16);
    Painter_DrawHLine(y + 10, x + 3, x + 16);
}

static void DrawSB_MathFunction_ModeDraw_Disable(int x, int y)
{
    Painter_DrawText(x + 2 + (set.common.lang == English ? 2 : 0), y + 5, set.common.lang == Russian ? "Вык" : "Dis");
}

static void DrawSB_MathFunction_ModeDraw_Together(int x, int y)
{
    Painter_DrawRectangle(x + 3, y + 5, 13, 9);
}

static void DrawSB_MathFunction_ModeDraw(int x, int y)
{
    static const pFuncVII funcs[3] =
    {
        DrawSB_MathFunction_ModeDraw_Disable,
        DrawSB_MathFunction_ModeDraw_Separate,
        DrawSB_MathFunction_ModeDraw_Together
    };
    funcs[MODE_DRAW_MATH](x, y);
}

static const SmallButton sbMathFunctionModeDraw =
{
    Item_SmallButton, &mspServiceMathFunction, 0,
    {
        "Экран", "Display",
        "Выбирает режим отображения математического сигнала",
        "Chooses the mode of display of a mathematical signal"
    },
    PressSB_MathFunction_ModeDraw,
    DrawSB_MathFunction_ModeDraw,
    {
        {DrawSB_MathFunction_ModeDraw_Disable,  "Вывод математической функции отключён",                        "The conclusion of mathematical function is disconnected"},
        {DrawSB_MathFunction_ModeDraw_Separate, "Сигналы и математическая функция выводятся в разных окнах",    "Signals and mathematical function are removed in different windows"},
        {DrawSB_MathFunction_ModeDraw_Together, "Сигналы и математическая функция выводятся в одном окне",      "Signals and mathematical function are removed in one window"}
    }
};

static void PressSB_MathFunction_RangeA()
{
    MATH_RANGE = RANGE_A;
    MATH_MULTIPLIER = MULTIPLIER_A;
}

static void DrawSB_MathFunction_RangeA(int x, int y)
{
    Painter_DrawChar(x + 8, y + 5, '1');
}

static const SmallButton sbMathFunctionRangeA =
{
    Item_SmallButton, &mspServiceMathFunction, 0,
    {
        "Масштаб 1-го канала", "Scale of the 1st channel",
        "Берёт масштаб для математического сигнала из первого канала",
        "Takes scale for a mathematical signal from the first channel"
    },
    PressSB_MathFunction_RangeA,
    DrawSB_MathFunction_RangeA
};

static void PressSB_MathFunction_RangeB()
{
    MATH_RANGE = RANGE_B;
    MATH_MULTIPLIER = MULTIPLIER_B;
}

static void DrawSB_MathFunction_RangeB(int x, int y)
{
    Painter_DrawChar(x + 8, y + 5, '2');
}

static const SmallButton sbMathFunctionRangeB =
{
    Item_SmallButton, &mspServiceMathFunction, 0,
    {
        "Масштаб 2-го канала", "Scale of the 2nd channel",
        "Берёт масштаб для математического сигнала из второго канала",
        "Takes scale for a mathematical signal from the second channel"
    },
    PressSB_MathFunction_RangeB,
    DrawSB_MathFunction_RangeB
};

static void PressSB_MathFunction_ModeRegSet()
{
    CircleIncreaseInt8((int8*)&MATH_MODE_REG_SET, 0, 1);
}

static void DrawSB_MathFunction_ModeRegSet_Range(int x, int y)
{
    Painter_DrawChar(x + 7, y + 5, set.common.lang == Russian ? 'M' : 'S');
}

static void DrawSB_MathFunction_ModeRegSet_RShift(int x, int y)
{
    Painter_DrawText(x + 5 - (set.common.lang == English ? 3 : 0), y + 5, set.common.lang == Russian ? "См" : "Shif");
}

static void DrawSB_MathFunction_ModeRegSet(int x, int y)
{
    const pFuncVII funcs[2] = {DrawSB_MathFunction_ModeRegSet_Range, DrawSB_MathFunction_ModeRegSet_RShift};
    funcs[MATH_MODE_REG_SET](x, y);
}

static const SmallButton sbMathFunctionModeRegSet =
{
    Item_SmallButton, &mspServiceMathFunction, 0,
    {
        "Режим ручки УСТАНОВКА", "Mode regulator SET",
        "Выбор режима ручки УСТАНОВКА - управление масштабом или смещением",
        "Choice mode regulcator УСТАНОВКА - management of scale or shift"
    },
    PressSB_MathFunction_ModeRegSet,
    DrawSB_MathFunction_ModeRegSet,
    {
        {DrawSB_MathFunction_ModeRegSet_Range,  "Управление масштабом", "Management of scale"},
        {DrawSB_MathFunction_ModeRegSet_RShift, "Управление смещением", "Management of shift"}
    }
};
    
// СЕРВИС -> Математика -> ФУНКЦИЯ -> Формула
static bool ActiveF_MathFormula()
{
    return MATH_FUNC_IS_MUL || MATH_FUNC_IS_SUM;
}

static void ChangeF_MathFormula()
{

}
                                    
static int8 curDigit = 0;
const Formula mfMathFormula =
{
    Item_Formula, &mspServiceMathFunction, ActiveF_MathFormula,
    {
        "Формула", "Formulf",
        "Здесь задаются коэффициенты и знаки в математической формуле",
        "Here you can set the coefficients and signs in a mathematical formula"
    },
    (int8*)&MATH_FUNC, (int8*)&set.math.koeff1add, (int8*)&set.math.koeff2add, (int8*)&set.math.koeff1mul, (int8*)&set.math.koeff2mul, &curDigit, ChangeF_MathFormula
};

/// СЕРВИС -> Математика -> ФУНКЦИЯ -> Масштаб
static bool ActiveF_Math_Scale()
{
    return MATH_FUNC_IS_MUL || MATH_FUNC_IS_SUM;
}

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Отображение
const Choice mcFFTenable =
{
    Item_Choice, &mspFFT, 0,
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

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Шкала
const Choice mcFFTscale =
{
    Item_Choice, &mspFFT, 0,
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

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Источник
const Choice mcFFTsource =
{
    Item_Choice, &mspFFT, 0,
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

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Окно
const Choice mcFFTwindow =
{
    Item_Choice, &mspFFT, 0,
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

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Предел
bool ActiveC_FFT_Limit()
{
    return SCALE_FFT_IS_LOG;
}

const Choice mcFFTlimit =
{
    Item_Choice, &mspFFT,  ActiveC_FFT_Limit,
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

// СЕРВИС -> МАТЕМАТИКА -> СПЕКТР -> Курсоры
bool ActiveP_FFT_Cursors()
{
    return ENABLED_FFT;
}

static const SmallButton sbExitMathFunction =
{
    Item_SmallButton, &mspServiceMathFunction,
    COMMON_BEGIN_SB_EXIT,
    EmptyFuncVV,
    DrawSB_Exit
};

static void OnRegSetMathFunction(int delta)
{
    if (DISABLED_DRAW_MATH)
    {
        return;
    }

    if (MATH_MODE_REG_SET_IS_RSHIFT)
    {
        int16 prevRShift = MATH_RSHIFT;
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
                Sound_RegulatorShiftRotate();
                MATH_RSHIFT = rShift;
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
                Sound_RegulatorShiftRotate();
                MATH_RSHIFT = rShift;
            }
        }
    }
    if (MATH_MODE_REG_SET_IS_RANGE)
    {
        static int sum = 0;
        sum -= delta;

        float rShiftAbs = RSHIFT_2_ABS(MATH_RSHIFT, MATH_RANGE);

        if (sum > 2)
        {
            if (MATH_RANGE < RangeSize - 1)
            {
                MATH_RANGE++;
                MATH_RSHIFT = Math_RShift2Rel(rShiftAbs, MATH_RANGE);
                Sound_RegulatorSwitchRotate();
            }
            sum = 0;
        }
        else if (sum < -2)
        {
            if (MATH_RANGE > 0)
            {
                MATH_RANGE--;
                MATH_RSHIFT = Math_RShift2Rel(rShiftAbs, MATH_RANGE);
                Sound_RegulatorSwitchRotate();
            }
            sum = 0;
        }
    }
}

static bool FuncOfActiveServiceMathFunction(void)
{
    return !ENABLED_FFT;
}

static void FuncOfPressServiceMathFunction(void)
{
    if (ENABLED_FFT)
    {
        Display_ShowWarningBad(ImpossibleEnableMathFunction);
    }
}

// СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ /////////////////////////////////////////////////////////////////////////////
static const Page mspServiceMathFunction =
{
    Item_Page, &pMath, FuncOfActiveServiceMathFunction,
    {
        "ФУНКЦИЯ", "FUNCTION",
        "Установка и выбор математической функции - сложения или умножения",
        "Installation and selection of mathematical functions - addition or multiplication"
    },
    Page_SB_MathFunction,
    {
        (void*)&sbExitMathFunction,
        (void*)&sbMathFunctionModeDraw,
        (void*)&sbMathFunction,
        (void*)&sbMathFunctionModeRegSet,
        (void*)&sbMathFunctionRangeA,
        (void*)&sbMathFunctionRangeB
    },
    FuncOfPressServiceMathFunction,
    EmptyFuncVV,
    OnRegSetMathFunction
};

static void PressSB_CursFFT_Exit()
{
    Display_RemoveAddDrawFunction();
}

const SmallButton sbExitCursFFT =
{
    Item_SmallButton, &mspCursFFT,
    COMMON_BEGIN_SB_EXIT,
    PressSB_CursFFT_Exit,
    DrawSB_Exit
};

///////////////////////////////////////////////////////////////////////////////////
static const Page mspCursFFT =
{
    Item_Page, &mspFFT, ActiveP_FFT_Cursors,
    {
        "КУРСОРЫ", "CURSORS",
        "Включает курсоры для измерения параметров спектра",
        "Includes cursors to measure the parameters of the spectrum"
    },
    Page_SB_MathCursorsFFT,
    {
        (void*)&sbExitCursFFT,
        (void*)&sbCursFFTSource,
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)0
    },
    EmptyFuncVV, EmptyFuncVV, Reg_FFT_Cursors
};

static bool FuncOfActiveFFT(void)
{
    return DISABLED_DRAW_MATH;
}

static void FuncOfPressFFT(void)
{
    if (!FuncOfActiveFFT())
    {
        Display_ShowWarningBad(ImpossibleEnableFFT);
    }
}

// СЕРВИС - МАТЕМАТИКА - КУРСОРЫ /////////////////////////////////////////////////////////////////////////////
static const Page mspFFT =
{
    Item_Page, &pMath, FuncOfActiveFFT,
    {
        "СПЕКТР", "SPECTRUM",
        "Отображение спектра входного сигнала",
        "Mapping the input signal spectrum"
    },
    Page_MathFFT,
    {
        (void*)&mcFFTenable,
        (void*)&mcFFTscale,
        (void*)&mcFFTsource,
        (void*)&mcFFTwindow,
        (void*)&mspCursFFT,
        (void*)&mcFFTlimit
    },
    FuncOfPressFFT
};
