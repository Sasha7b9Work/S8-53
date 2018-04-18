#pragma once


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsMath
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SCALE_FFT                   (set.math.scaleFFT)                 ///< SettingMath.scaleFFT
#define SCALE_FFT_IS_LOG            (SCALE_FFT == ScaleFFT_Log)
#define SCALE_FFT_IS_LINEAR         (SCALE_FFT == ScaleFFT_Linear)

#define SOURCE_FFT                  (set.math.sourceFFT)                ///< SettingsMath.sourceFFT
#define SOURCE_FFT_IS_A             (SOURCE_FFT == SourceFFT_A)
#define SOURCE_FFT_IS_B             (SOURCE_FFT == SourceFFT_B)

#define WINDOW_FFT                  (set.math.windowFFT)                ///< SettingsMath.windowFFT
#define WINDOW_FFT_IS_HAMMING       (WINDOW_FFT == WindowFFT_Hamming)
#define WINDOW_FFT_IS_BLACKMAN      (WINDOW_FFT == WindowFFT_Blackman)
#define WINDOW_FFT_IS_HANN          (WINDOW_FFT == WindowFFT_Hann)

#define FFT_MAX_DB                  (set.math.fftMaxDB)                 ///< SettingsMath.fftMaxDB

#define MATH_FUNC                   (set.math.func)                     ///< SettingsMath.func
#define MATH_FUNC_IS_SUM            (MATH_FUNC == Function_Sum)
#define MATH_FUNC_IS_MUL            (MATH_FUNC == Function_Mul)

#define FFT_CUR_CURSOR              (set.math.currentCursor)            ///< SettingsMath.currentCursor
#define FFT_CUR_CURSOR_IS_0         (FFT_CUR_CURSOR == 0)

#define FFT_POS_CURSOR(num)         (set.math.posCur[num])              ///< SettingsMath.currentCursor
#define FFT_POS_CURSOR_0            (FFT_POS_CURSOR(0))
#define FFT_POS_CURSOR_1            (FFT_POS_CURSOR(1))

#define ENABLED_FFT                 (set.math.enableFFT)                ///< SettingsMath.enableFFT

#define MODE_DRAW_MATH              (set.math.modeDraw)                 ///< SettingsMath.modeDraw
#define DISABLED_DRAW_MATH          (MODE_DRAW_MATH == ModeDrawMath_Disable)
#define MODE_DRAW_MATH_IS_TOGETHER  (MODE_DRAW_MATH == ModeDrawMath_Together)
#define MODE_DRAW_MATH_IS_SEPARATE  (MODE_DRAW_MATH == ModeDrawMath_Separate)

#define MATH_MODE_REG_SET           (set.math.modeRegSet)               ///< SettingsMath.modeRegSet
#define MATH_MODE_REG_SET_IS_RSHIFT (MATH_MODE_REG_SET == ModeRegSet_RShift)
#define MATH_MODE_REG_SET_IS_RANGE  (MATH_MODE_REG_SET == ModeRegSet_Range)

#define MATH_MULTIPLIER             (set.math.multiplier)               ///< SettingsMath.multiplier


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Масштаб шкалы для отображения спектра.
typedef enum
{
    ScaleFFT_Log,           ///< Это значение означает логарифмическую шкалу вывода спектра.
    ScaleFFT_Linear         ///< Это значение означает линейную шкалу вывода спектра.
} ScaleFFT;

/// Источинк сигнала для отображения спектра.
typedef enum
{
    SourceFFT_A,            ///< Расчёт и отображение спектра первого канала.
    SourceFFT_B,            ///< Расчёт и отображение спектра второго канала.
    SourceFFT_Both          ///< Расчёт и отображение спектров обоих каналов.
} SourceFFT;

/// Вид окна для умножения сигнала перед расчётом спектра.
typedef enum
{
    WindowFFT_Rectangle,    ///< Перед расчётом спектра сигнал не умножается на окно.
    WindowFFT_Hamming,      ///< Перед расчётом спектра сигнал умножается на окно Хэмминга.
    WindowFFT_Blackman,     ///< Перед расчётом спектра сигнал умножается на окно Блэкмана.
    WindowFFT_Hann          ///< Перед расчётом спектра сигнал умножается на окно Ханна.
} WindowFFT;

/// Минимальное значение на вертикальной координате графика спектра.
typedef enum
{
    FFTmaxDB_40,            ///< Вертикальная координата до -40Дб
    FFTmaxDB_60,            ///< Вертикальная координата до -60Дб
    FFTmaxDB_80             ///< Вертикальная координата до -80Дб
} FFTmaxDB;

/// Какую функцию рассчитывать.
typedef enum
{
    Function_Sum,           ///< Сумма двух каналов.
    Function_Mul            ///< Произведение двух каналов.
} Function;

/// Где отрисовывать математический сигнал.
typedef enum
{
    ModeDrawMath_Disable,   ///< Отображение математического сигнала выключено.
    ModeDrawMath_Separate,  ///< Математический сигнал отображается на отдельном экране.
    ModeDrawMath_Together   ///< Математический сигнал отображается поверх текущего сигнала.
} ModeDrawMath;

/// Чем управляет ручка УСТАНОВКА, когда включена математическая функция.
typedef enum
{
    ModeRegSet_Range,       ///< Ручка УСТАНОВКА управляет масштабом по напряжению.
    ModeRegSet_RShift       ///< Ручка УСТАНОВКА управляет смещением по напряжению.
} ModeRegSet;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    ScaleFFT        scaleFFT;       ///< Масштаб шкалы для отображения спектра.
    SourceFFT       sourceFFT;      ///< Источник сигнала для расчёта и отображения спектра.
    WindowFFT       windowFFT;      ///< Задаёт вид окна, на которое умножается сигнал перед расчётом спектра.
    FFTmaxDB        fftMaxDB;       ///< Минимальное значение на вертикальной координате графика спектра.
    Function        func;           ///< Включённая математическая функция.
    uint8           currentCursor;  ///< Определяет, каким курсором спектра управляет ручка УСТАНОВКА.
    uint8           posCur[2];      ///< Позиция курсора спектра. Изменятеся 0...256.
    int8            koeff1add;      ///< Коэффициент при первом слагаемом для сложения.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;      ///< \c true, если включён спектр.
    ModeDrawMath    modeDraw;       ///< Раздельный или общий дисплей в режиме математической функции.
    ModeRegSet      modeRegSet;     ///< Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали.
    Range           range;          ///< Масштаб по напряжению математического сигнала.
    Divider      multiplier;     ///< Множитель масштаба математического сигнала.
    int16           rShift;         ///< Смещение по вертикальной оси математического сигнала.
} SettingsMath;


/** @}  @}
 */
