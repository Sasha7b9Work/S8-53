// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Debug.h"
#include "Hardware/Timer.h"


static uint timeStart;
static uint timePrev;
static uint timeCounter;
static uint timeStartCounter;

// Время старта профилировщика
volatile static uint timeStartMS = 0;
// Время предыдущей точки профилировщика
volatile static uint timePrevMS = 0;

pchar Debug::file[10] = { 0 };
int   Debug::line[10] = { 0 };
int   Debug::posItem = -1;
int   Debug::counter = 0;
int   Debug::pushed = 0;


void Debug::StartProfilingMS()
{
    timeStartMS = TIME_MS;
    timePrevMS = TIME_MS; //-V656
}


uint Debug::PointProfilingMS(pchar _file, int _line)
{
    uint d = TIME_MS - timePrevMS;
    LOG_WRITE("%s %d %d", _file, _line, d);
    timePrevMS = TIME_MS;

    return d;
}


void Debug::StartProfilingUS()
{
    timeStart = TIME_TICKS;
    timePrev = timeStart;
}


uint Debug::PointProfilingUS(pchar _file, int _line)
{
    uint d = TIME_TICKS - timePrev;
    LOG_WRITE("%s:%d - %d us", _file, _line, d);
    timePrev = TIME_TICKS;
    return d;
}


void Debug::ClearTimeCounter()
{
    timeCounter = 0;
}


void Debug::StartIncreaseCounter()
{
    timeStartCounter = TIME_TICKS;
}


void Debug::StopIncreaseCounter()
{
    timeCounter += (TIME_TICKS - timeStartCounter);
}


uint Debug::GetTimeCounterUS()
{
    return timeCounter;
}
