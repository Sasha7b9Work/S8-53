#include "Panel.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Definition.h"
#include "Menu/Pages/PageMemory.h"
#include "Utils/Math.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"


static const uint MIN_TIME = 500;


static void ChangeRShift(int *prevTime, void(*f)(Channel, int16), Channel chan, int16 relStep);


void HelpLong(void)
{
    SHOW_HELP_HINTS++;
    gStringForHint = 0;
    gItemHint = 0;
}

void Channel0Long(void)
{
    Menu::LongPressureButton(B_Channel0);
}

void Channel1Long(void)
{
    Menu::LongPressureButton(B_Channel1);
}

void TimeLong(void)
{
    Menu::LongPressureButton(B_Time);
}

void TrigLong(void)
{
    if (MODE_LONG_PRESS_TRIG_IS_LEVEL0)
    {
        Menu::LongPressureButton(B_Trig);
    }
    else
    {
        FPGA::FindAndSetTrigLevel();
    }
}

void StartDown()                        // B_Start
{
    if (MODE_WORK_IS_DIRECT)
    {
        Menu::PressButton(B_Start);
    }
}

void PowerDown()                        // B_Power
{
    Menu::ShortPressOnPageItem((Page *)Menu::OpenedItem(), 0);
    Settings::Save();
    Log_DisconnectLoggerUSB();
    Panel::TransmitData(0x04);           // Посылаем команду выключения
}

void MenuLong() 
{
    Menu::LongPressureButton(B_Menu);
}

void F1Long()
{
    Menu::LongPressureButton(B_F1);
}

void F2Long()
{
    Menu::LongPressureButton(B_F2);
}

void F3Long()
{
    Menu::LongPressureButton(B_F3);
}

void F4Long()
{
    Menu::LongPressureButton(B_F4);
}

void F5Long()
{
    Menu::LongPressureButton(B_F5);
}

//****************************************************************************************************************
int CalculateCount(int *prevTime)
{
    uint time = gTimerMS;
    uint delta = time - *prevTime;
    *prevTime = (int)time;

    if (delta > 75)
    {
        return 1;
    }
    else if (delta > 50)
    {
        return 2;
    }
    else if (delta > 25)
    {
        return 3;
    }
    return 4;
}

bool CanChangeTShift(int16 tShift)
{
    static uint time = 0;
    if (tShift == 0)
    {
        time = gTimerMS;
        return true;
    }
    else if (time == 0)
    {
        return true;
    }
    else if (gTimerMS - time > MIN_TIME)
    {
        time = 0;
        return true;
    }
    return false;
}

bool CanChangeRShiftOrTrigLev(TrigSource channel, int16 rShift)
{
    static uint time[3] = {0, 0, 0};
    if (rShift == RShiftZero)
    {
        time[channel] = gTimerMS;
        return true;
    }
    else if (time[channel] == 0)
    {
        return true;
    }
    else if (gTimerMS - time[channel] > MIN_TIME)
    {
        time[channel] = 0;
        return true;
    }
    return false;
}

void ChangeRShift(int *prevTime, void(*f)(Channel, int16), Channel chan, int16 relStep)
{
    if (ENUM_ACCUM_IS_NONE)
    {
        FPGA::TemporaryPause();
    }
    int count = CalculateCount(prevTime);
    int rShiftOld = SET_RSHIFT(chan);
    int rShift = SET_RSHIFT(chan) + relStep * count;
    if ((rShiftOld > RShiftZero && rShift < RShiftZero) || (rShiftOld < RShiftZero && rShift > RShiftZero))
    {
        rShift = RShiftZero;
    }
    if (CanChangeRShiftOrTrigLev((TrigSource)chan, (int16)rShift))
    {
        Sound::RegulatorShiftRotate();
        f(chan, (int16)rShift);
    }
}

void ChangeTrigLev(int *prevTime, void(*f)(TrigSource, int16), TrigSource trigSource, int16 relStep)
{
    int count = CalculateCount(prevTime);
    int trigLevOld = TRIG_LEVEL(trigSource);
    int trigLev = TRIG_LEVEL(trigSource) + relStep * count;
    if ((trigLevOld > TrigLevZero && trigLev < TrigLevZero) || (trigLevOld < TrigLevZero && trigLev > TrigLevZero))
    {
        trigLev = TrigLevZero;
    }
    if (CanChangeRShiftOrTrigLev(trigSource, (int16)trigLev))
    {
        Sound::RegulatorShiftRotate();
        f(trigSource, (int16)trigLev);
    }
}

void ChangeTShift(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    int tShiftOld = TSHIFT;
    float step = relStep * count;
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else
    {
        if (step < 1)
        {
            step = 1;
        }
    }

    int16 tShift = TSHIFT + step;
    if (((tShiftOld > 0) && (tShift < 0)) || (tShiftOld < 0 && tShift > 0))
    {
        tShift = 0;
    }
    if (CanChangeTShift(tShift))
    {
        Sound::RegulatorShiftRotate();
        f(tShift);
    }
}

void ChangeShiftScreen(int *prevTime, void(*f)(int), int16 relStep)
{
    int count = CalculateCount(prevTime);
    float step = relStep * count;
    if (step < 0)
    {
        if (step > -1)
        {
            step = -1;
        }
    }
    else if (step < 1)
    {
        step = 1;
    }
    f(step);
}

static void SetRShift(Channel ch, int16 rShift)
{
    FPGA::SetRShift(ch, rShift);
}

void RShift0Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, -STEP_RSHIFT);
}

void RShift0Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, A, +STEP_RSHIFT);
}

void RShift1Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, -STEP_RSHIFT);
}

void RShift1Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, SetRShift, B, +STEP_RSHIFT);
}

static void SetTrigLev(TrigSource ch, int16 trigLev)
{
    FPGA::SetTrigLev(ch, trigLev);
}

void TrigLevLeft()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, -STEP_RSHIFT);
}

void TrigLevRight()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, SetTrigLev, TRIG_SOURCE, +STEP_RSHIFT);
}

static void ShiftScreen(int shift)
{
    Display::ShiftScreen(shift);
}

static void SetTShift(int tShift)
{
    FPGA::SetTShift(tShift);
}

void XShift(int delta)
{
    static int prevTime = 0;
    if (!FPGA::IsRunning() || TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
    {
        if (!ENUM_POINTS_IS_281)
        {
            ChangeShiftScreen(&prevTime, ShiftScreen, 2 * delta);
        }
    }
    else
    {
        ChangeTShift(&prevTime, SetTShift, (int16)delta);
    }
}

void TShiftLeft()
{
    XShift(-1);
}

void TShiftRight()
{
    XShift(1);
}

void Range0Left()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(A);
}

void Range0Right()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(A);
}

void Range1Left()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeIncrease(B);
}

void Range1Right()
{
    Sound::RegulatorSwitchRotate();
    FPGA::RangeDecrease(B);
}

void TBaseLeft()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseIncrease();
}

void TBaseRight()
{
    Sound::RegulatorSwitchRotate();
    FPGA::TBaseDecrease();
}

void SetLeft()
{
    Menu::RotateRegSetLeft();
}

void SetRight()
{
    Menu::RotateRegSetRight();
}
