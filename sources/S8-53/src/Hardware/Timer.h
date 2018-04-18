#pragma once 
#include "defines.h"
#include "Utils/GlobalFunctions.h"


typedef enum
{
    kPressKey,                  ///< Нужно устанавливать, когда приходит нажатие клавиши.
    kShowLevelRShift0,          ///< Нужно устанавливать, когда изменяется положение ручки смещения канала 1.
    kShowLevelRShift1,          ///< Нужно устанавливать, когда изменяется положение ручки смещения канала 2.
    kShowLevelTrigLev,          ///< Нужно устанавливать, когда изменяется положение ручки уровня синхронизации
    kNumSignalsInSec,           ///< Для установки количества считываний сигнала в секунду.
    kFlashDisplay,              ///< Таймер для мерцающих участков экрана чёрно-белого дисплея.
    kP2P,                       ///< Таймер для чтения данных в режиме поточечного вывода.
    kShowMessages,              ///< Таймер для засекания время показа информационных и предупреждающих сообщений.
    kMenuAutoHide,              ///< Таймер для отсчёта времени скрывания меню.
    kRShiftMarkersAutoHide,     ///< Таймер для отсчёта времени скрывания дополнительных боковых меток.
    kTemp,                      ///< Вспомогательный, для общих нужд.
    kStopSound,                 ///< Выключить звук
    kTemporaryPauseFPGA,        ///< Временная пауза для фиксации сигнала на экране после поворота ручек
    kTimerDrawHandFunction,     ///< Этот таймер могут использовать функции, использующие свою процедуру рисования
    TypeTimerSize               ///< Общее количество таймеров.
} TypeTimer;


class Timer
{
public:
    /// Задержка на timeMS миллисекунд
    static void PauseOnTime(uint timeMS) { HAL_Delay(timeMS); };

    static void PauseOnTicks(uint numTicks);
    /// Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутренее время таймеров на 1мс.
    static void Update1ms();
    /// Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутреннее время таймеров на 10мс.
    static void Update10ms();
         
    static void Enable(TypeTimer type, int timeInMS, pFuncVV);
    /// Остановить таймер.
    static void Disable(TypeTimer type);
    /// Приостановить таймер. Запустить снова можно функцией Timer_Continue().
    static void Pause(TypeTimer type);
    /// Продолжить работу таймера, ранее приостановленного функцией Timer_Pause().
    static void Continue(TypeTimer type);
    /// С помощью этой функции можно узнать, работает ли таймер.
    static bool IsRun(TypeTimer type);
    /// Запускает счётчик для измерения малых отрезков времени.
    static void StartMultiMeasurement();
    /// Устанавливает стартовую точку логгирования. Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
    static void StartLogging();
    
    static uint LogPointUS(char *name);
    
    static uint LogPointMS(char *name);
};

#define gTimerMS HAL_GetTick()

/**
  * gTimerTics - количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement().
  * В одной секунде 120.000.000 тиков. Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
  * Количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement(). Не более (1 << 32).
***/
#define gTimerTics (TIM2->CNT)
