#pragma once 
#include "defines.h"


typedef enum
{
    kPressKey,                  // Нужно устанавливать, когда приходит нажатие клавиши.
    kShowLevelRShift0,          // Нужно устанавливать, когда изменяется положение ручки смещения канала 1.
    kShowLevelRShift1,          // Нужно устанавливать, когда изменяется положение ручки смещения канала 2.
    kShowLevelTrigLev,          // Нужно устанавливать, когда изменяется положение ручки уровня синхронизации
    kNumSignalsInSec,           // Для установки количества считываний сигнала в секунду.
    kFlashDisplay,              // Таймер для мерцающих участков экрана чёрно-белого дисплея.
    kP2P,                       // Таймер для чтения данных в режиме поточечного вывода.
    kShowMessages,              // Таймер для засекания время показа информационных и предупреждающих сообщений.
    kMenuAutoHide,              // Таймер для отсчёта времени скрывания меню.
    kRShiftMarkersAutoHide,     // Таймер для отсчёта времени скрывания дополнительных боковых меток.
    kTemp,                      // Вспомогательный, для общих нужд.
    kStopSound,                 // Выключить звук
    kTemporaryPauseFPGA,        // Временная пауза для фиксации сигнала на экране после поворота ручек
    kTimerDrawHandFunction,     // Этот таймер могут использовать функции, использующие свою процедуру рисования
    TypeTimerSize               // Общее количество таймеров.
} TypeTimer;


void    Timer_PauseOnTime(uint timeMS);
void    Timer_PauseOnTicks(uint numTicks);
void    Timer_Update1ms(void);                  // Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутренее время таймеров на 1мс.
void    Timer_Update10ms(void);                 // Функция вызывается по прерыванию системного таймера для работы таймеров. Увеличивает внутреннее время таймеров на 10мс.

void    Timer_Enable(TypeTimer type, int timeInMS, pFuncVV);
void    Timer_Disable(TypeTimer type);          // Остановить таймер
void    Timer_Pause(TypeTimer type);            // Приостановить таймер. Запустить снова можно функцией Timer_Continue()
void    Timer_Continue(TypeTimer type);         // Продолжить работу таймера, ранее приостановленного функцией Timer_Pause()
bool    Timer_IsRun(TypeTimer type);            // С помощью этой функции можно узнать, работает ли таймер

void    Timer_StartMultiMeasurement(void);      // Запускает счётчик для измерения малых отрезков времени.

void    Timer_StartLogging(void);               // Устанавливает стартовую точку логгирования. Далее вызовы Timer_LogPoint засекают временные интервалы от это точки.
uint    Timer_LogPointUS(char *name);
uint    Timer_LogPointMS(char *name);

extern volatile uint gTimerMS;                  // Время, прошедшее с момента инициализации таймера, в секундах
/**
  * gTimerTics - количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement().
  * В одной секунде 120.000.000 тиков. Максимальный отрезок времени, который можно отсчитать с её помощью - 35 сек.
  * Количество тиков, прошедших с момента последнего вызова функции Timer_StartMultiMeasurement(). Не более (1 << 32).
***/
#define gTimerTics (TIM2->CNT)
