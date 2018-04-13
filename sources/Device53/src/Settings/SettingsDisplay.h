#pragma once
#include "defines.h"
#include "SettingsTypes.h"
#include "Display/Colors.h"


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsDisplay
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIME_SHOW_LEVELS            (set.display.timeShowLevels)                ///< SettingsDisplay.timeShowLevels

#define SHIFT_IN_MEMORY             (set.display.shiftInMemory)                 ///< SettingsDisplay.shiftInMemory

#define TIME_MESSAGES               (set.display.timeMessages)                  ///< SettingsDisplay.timeMessages

#define BRIGHTNESS                  (set.display.brightness)                    ///< SettingsDisplay.brightness

#define BRIGHTNESS_GRID             (set.display.brightnessGrid)                ///< SettingsDisplay.brightnessGrid

#define DISPLAY_COLOR(num)          (set.display.colors[num])                   ///< SettingsDisplay.colors

#define MODE_DRAW_SIGNAL            (set.display.modeDrawSignal)                ///< SettingsDisplay.modeDrawSignal
#define MODE_DRAW_IS_SIGNAL_LINES   (MODE_DRAW_SIGNAL == ModeDrawSignal_Lines)  ///< \c true, если сигнал выводится линиями.

#define TYPE_GRID                   (set.display.typeGrid)                      ///< SettingsDisplay.typeGrid
#define TYPE_GRID_IS_1              (TYPE_GRID == TypeGrid_1)                   ///< Установлен тип сетки TypeGrid_1.
#define TYPE_GRID_IS_2              (TYPE_GRID == TypeGrid_2)                   ///< Установлен тип сетки TypeGrid_2.
#define TYPE_GRID_IS_3              (TYPE_GRID == TypeGrid_3)                   ///< Установлен тип сетки TypeGrid_3.

#define ENUM_ACCUM                  (set.display.enumAccumulation)              ///< SettingsDisplay.numAccumulation
#define ENUM_ACCUM_IS_NONE          (ENUM_ACCUM == NumAccumulation_1)           ///< \c true, если накопление выключено.
#define ENUM_ACCUM_IS_INFINITY      (ENUM_ACCUM == NumAccumulation_Infinity)    ///< \c true, если установлено бесконечное число накоплений.
#define NUM_ACCUM                   (1 << (int)set.display.enumAccumulation)    ///< Количество накоплений.

#define ENUM_AVE                    (set.display.enumAve)                       ///< SettingsDisplay.enumAve
#define NUM_AVE                     (1 << ENUM_AVE)                             ///< Количество усреднений.

#define MODE_AVE                    (set.display.modeAve)                       ///< SettingsDisplay.modeAve

#define ENUM_MIN_MAX                (set.display.enumMinMax)                    ///< SettingsDisplay.enumMinMax
#define NUM_MIN_MAX                 (1 << ENUM_MIN_MAX)                         ///< Количество измерений сигналов для расчёта минимумов и максимумов.

#define SMOOTHING                   (set.display.smoothing)                     ///< SettingsDisplay.smoothing
#define SMOOTHING_IS_DISABLE        (SMOOTHING == Smoothing_Disable)            ///< \c true, если скользящий фильтр выключен.

#define ENUM_SIGNALS_IN_SEC         (set.display.enumSignalsInSec)              ///< SettingsDisplay.enumSignalsInSec

#define LAST_AFFECTED_CHANNEL       (set.display.lastAffectedChannel)           ///< SettingsDisplay.lastAffectedChannel
#define LAST_AFFECTED_CHANNEL_IS_A  (LAST_AFFECTED_CHANNEL == A)                ///< \c true, если последний канал, которым управляли - 1.
#define LAST_AFFECTED_CHANNEL_IS_B  (LAST_AFFECTED_CHANNEL == B)                ///< \c true, если последний канал, которым управляли - 2.

#define MODE_ACCUM                  (set.display.modeAccumulation)              ///< SettingsDisplay.modeAccumulation
#define MODE_ACCUM_IS_RESET         (MODE_ACCUM == ModeAccumulation_Reset)      ///< \c true, если накопление со сбросом.
#define MODE_ACCUM_IS_NORESET       (MODE_ACCUM == ModeAccumulation_NoReset)    ///< \c true, если накопление без сброса.

#define ALT_MARKERS                 (set.display.altMarkers)                    ///< SettingsDisplay.altMarkers
#define ALT_MARKERS_HIDE            (ALT_MARKERS == AM_Hide)                    ///< \c true, если дополнительные маркеры никогда не надо показывать.

#define MENU_AUTO_HIDE              (set.display.menuAutoHide)                  ///< SettingsDisplay.menuAutoHide

#define SHOW_STRING_NAVIGATION      (set.display.showStringNavigation)          ///< SettingsDisplay.showStringNavigation

#define LINKING_RSHIFT              (set.display.linkingRShift)                 ///< SettingsDisplay.linkingRShift
#define LINKING_RSHIFT_IS_VOLTAGE   (LINKING_RSHIFT == LinkingRShift_Voltage)   ///< \c true, если привязка к абсолютному напряжению.


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /// Режим отрисовки сигнала.
typedef enum
{
    ModeDrawSignal_Lines = 0,   ///< Сигнал рисуется линиями.
    ModeDrawSignal_Points = 1   ///< Сигнал рисуется точками.
} ModeDrawSignal;

/// Тип сетки на экране.
typedef enum
{
    TypeGrid_1,
    TypeGrid_2,
    TypeGrid_3,
    TypeGrid_4,
    TG_Size
} TypeGrid;

/// Перечисление накоплений.
typedef enum
{
    NumAccumulation_1,
    NumAccumulation_2,
    NumAccumulation_4,
    NumAccumulation_8,
    NumAccumulation_16,
    NumAccumulation_32,
    NumAccumulation_64,
    NumAccumulation_128,
    NumAccumulation_Infinity
} ENumAccumulation;

/// Количество усреднений по измерениям.
typedef enum
{
    NumAveraging_1,
    NumAveraging_2,
    NumAveraging_4,
    NumAveraging_8,
    NumAveraging_16,
    NumAveraging_32,
    NumAveraging_64,
    NumAveraging_128,
    NumAveraging_256,
    NumAveraging_512
} ENumAveraging;

/// Тип усреднений по измерениям.
typedef enum
{
    Averaging_Accurately,   ///< Усреднять точно.
    Averaging_Around        ///< Усреднять приблизительно.
} ModeAveraging;

/// Количество измерений для расчёта минимального и максимального значений.
typedef enum
{
    NumMinMax_1,
    NumMinMax_2,
    NumMinMax_4,
    NumMinMax_8,
    NumMinMax_16,
    NumMinMax_32,
    NumMinMax_64,
    NumMinMax_128
} ENumMinMax;

/// Количество точек для расчёта скользящего фильтра.
typedef enum
{
    Smoothing_Disable,
    Smoothing_2points = 1,
    Smoothing_3points = 2,
    Smoothing_4points = 3,
    Smoothing_5points = 4,
    Smoothing_6points = 5,
    Smoothing_7points = 6,
    Smoothing_8points = 7,
    Smoothing_9points = 8,
    Smoothing_10points = 9
} Smoothing;

/// Ограничение FPS.
typedef enum
{
    NumSignalsInSec_25,
    NumSignalsInSec_10,
    NumSignalsInSec_5,
    NumSignalsInSec_2,
    NumSignalsInSec_1
} ENumSignalsInSec;

/// Режим накопления.
typedef enum
{
    ModeAccumulation_NoReset,   ///< В этом режиме показываются строго N последних измерений.
    ModeAccumulation_Reset      ///< В этом режиме набираются N последних измерений и потом сбрасываются.
} ModeAccumulation;

/// Режим отображения дополнительных боковых маркеров смещения по напряжению.
typedef enum
{
    AM_Hide,        ///< Никода не выводить.
    AM_Show,        ///< Всегда выводить.
    AM_AutoHide     ///< Выводить и прятать через timeShowLevels.
} AltMarkers;

/// Через какое время после последнего нажатия кнопки скрывать меню.
typedef enum
{
    MenuAutoHide_None = 0,  ///< Никогда.
    MenuAutoHide_5 = 5,     ///< Через 5 секунд.
    MenuAutoHide_10 = 10,   ///< Через 10 секунд.
    MenuAutoHide_15 = 15,   ///< Через 15 секунд.
    MenuAutoHide_30 = 30,   ///< Через 30 секунд.
    MenuAutoHide_60 = 60    ///< Через 60 секунд.
} MenuAutoHide;

/// Тип привязки к смещению по вертикали
typedef enum
{
    LinkingRShift_Voltage,      ///< Смещение привязано к напряжению
    LinkingRShift_Position      ///< Смещение привязаоно к позиции на экране
} LinkingRShift;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Настройки изображения
typedef struct
{
    int16               timeShowLevels;             ///< Время, в течение которого нужно показывать уровни смещения.
    int16               shiftInMemory;              ///< Показывает смещение левого края сетки относительно нулевого байта памяти. Нужно для правильного отображения сигнала в окне.
    int16               timeMessages;               ///< Время в секундах, на которое сообщение остаётся на экране.
    int16               brightness;                 ///< Яркость дисплея.
    int16               brightnessGrid;             ///< Яркость сетки от 0 до 100.
    uint16              colors[16];                 ///< Цвета.
    ModeDrawSignal      modeDrawSignal;             ///< Режим отрисовки сигнала.
    TypeGrid            typeGrid;                   ///< Тип сетки.
    ENumAccumulation    enumAccumulation;           ///< Перечисление накоплений сигнала на экране.
    ENumAveraging       enumAve;                    ///< Перечисление усреднений сигнала.
    ModeAveraging       modeAve;                    ///< Тип усреднений по измерениям.
    ENumMinMax          enumMinMax;                 ///< Число измерений для определения минимумов и максимумов.
    Smoothing           smoothing;                  ///< Число точек для скользящего фильтра.
    ENumSignalsInSec    enumSignalsInSec;           ///< Перечисление считываний сигнала в секунду.
    Channel             lastAffectedChannel;        ///< Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation    modeAccumulation;           ///< Задаёт режим накопления сигналов.
    AltMarkers          altMarkers;                 ///< Режим отображения дополнительных боковых маркеров смещений.
    MenuAutoHide        menuAutoHide;               ///< Через сколько времени после последнего нажатия клавиши прятать меню.
    bool                showFullMemoryWindow;       ///< Показывать ли окно памяти вверху экрана. \todo Не используется.
    bool                showStringNavigation;       ///< Показывать ли строку текущего состояния меню..
    LinkingRShift       linkingRShift;              ///< Тип привязки смещения по вертикали.
} SettingsDisplay;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Возвращает ограничение частоты кадров.
int sDisplay_NumSignalsInS();
/// Устанавливает ограничение частоты кадров.
void sDisplay_SetNumSignalsInS(int maxFPS);
/// Возвращает число точек сглаживающего фильтра (режим ДИСПЛЕЙ - Сглаживание).
int sDisplay_NumPointSmoothing();
/// Возвращает адрес первой и последней точки на экране.
void sDisplay_PointsOnDisplay(int *firstPoint, int *lastPoint);
/// Возвращает время, через которое меню автоматически скрывается, если не было больше нажатий.
int sDisplay_TimeMenuAutoHide();
/// Если экран разделён на две части и основной сигнал выводится сверху - например, в режиме вывода спектра
bool sDisplay_IsSeparate();
/// brightness = 1..100
void sDisplay_SetBrightness(int16 brightness);
/// Возвращает режим усреднения
ModeAveraging sDisplay_ModeAveraging();

int sDisplay_NumAverage();


/** @}  @}
 */
