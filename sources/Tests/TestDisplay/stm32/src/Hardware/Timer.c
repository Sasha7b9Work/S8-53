#include "defines.h"
#include "Timer.h"
#include "Log.h"


volatile uint gTimerMS = 0;
static void (*f[TypeTimerSize])() = {0};
static int reactionTimeMS[TypeTimerSize] = {0};
static int currentTimeMS[TypeTimerSize] = {0};
static bool isRun[TypeTimerSize] = {false};


uint Timer_GetMS(void); 
void Timer_StartMultiMeasurement(void);
uint Timer_MultiMeasTicks(void);
void Timer_PauseOnTime(uint timeMS);
void Timer_Enable(TypeTimer type, int timeInMS, pFuncVV);
void Timer_Disable(TypeTimer type);
void Timer_Pause(TypeTimer type);
void Timer_Continue(TypeTimer type);
bool Timer_IsRun(TypeTimer type);
void Timer_Update1ms(void);
void Timer_Update10ms(void);

void Timer_PauseOnTicks(uint numTicks)
{
    uint startTicks = gTimerTics;
    while (gTimerTics - startTicks < numTicks) {};
}

void Timer_PauseOnTime(uint timeMS)
{
    uint time = gTimerMS;
    while(gTimerMS - time < timeMS) {};
}

void Timer_StartMultiMeasurement()
{
    TIM2->CR1 &= (uint16)~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN; 
}

static uint timeStartLogging = 0;
static uint timePrevPoint = 0;

void Timer_StartLogging()
{
    timeStartLogging = gTimerTics;
    timePrevPoint = timeStartLogging;
}

uint Timer_LogPointUS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f us", name, interval / 120.0f);
    return interval;
}

uint Timer_LogPointMS(char *name)
{
    uint interval = gTimerTics - timePrevPoint;
    timePrevPoint = gTimerTics;
    LOG_WRITE("%s %.2f ms", name, interval / 120e3);
    return interval;
}

void Timer_Enable(TypeTimer type, int timeInMS, void(*eF)())
{
    f[type] = eF;
    reactionTimeMS[type] = timeInMS;
    currentTimeMS[type] = timeInMS - 1;
    isRun[type] = true;
}

void Timer_Disable(TypeTimer type)
{
    isRun[type] = false;
}

void Timer_Pause(TypeTimer type)
{
    isRun[type] = false;
}

void Timer_Continue(TypeTimer type)
{
    isRun[type] = true;
}

bool Timer_IsRun(TypeTimer type)
{
    return isRun[type];
};

void Timer_Update1ms()
{
    gTimerMS++;
    for(int num = 0; num < TypeTimerSize; num++)
    {
        if(isRun[num])
        {
            currentTimeMS[num]--;
            if(currentTimeMS[num] < 0)
            {
                f[num]();
                currentTimeMS[num] = reactionTimeMS[num] - 1;
            }
            
        }
    }
}

void Timer_Update10ms()
{
    gTimerMS += 10;
    for(int num = 0; num < TypeTimerSize; num++)
    {
        if(isRun[num])
        {
            currentTimeMS[num] -= 10;
            if(currentTimeMS[num] < 0)
            {
                f[num]();
                currentTimeMS[num] = reactionTimeMS[num] - 1;
            }

        }
    }
}
