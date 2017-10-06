#include "Panel.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGA_Types.h"
#include "Menu/Menu.h"
#include "Menu/MenuFunctions.h"
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
    gBF.showHelpHints++;
    gStringForHint = 0;
    gItemHint = 0;
}

void Channel0Long(void)
{
    Menu_LongPressureButton(B_Channel0);
}

void Channel1Long(void)
{
    Menu_LongPressureButton(B_Channel1);
}

void TimeLong(void)
{
    Menu_LongPressureButton(B_Time);
}

void TrigLong(void)
{
    if (MODE_LONG_PRESS_TRIG_IS_LEVEL0)
    {
        Menu_LongPressureButton(B_Trig);
    }
    else
    {
        FPGA_FindAndSetTrigLevel();
    }
}

void StartDown()                        // B_Start
{
    if (MODE_WORK_IS_DIRECT)
    {
        Menu_PressButton(B_Start);
    }
}

void PowerDown()                        // B_Power
{
    ShortPressOnPageItem(OpenedItem(), 0);
    Settings_Save();
    Log_DisconnectLoggerUSB();
    Panel_TransmitData(0x04);           // Посылаем команду выключения
}

void MenuLong() 
{
    Menu_LongPressureButton(B_Menu);
}

void F1Long()
{
    Menu_LongPressureButton(B_F1);
}

void F2Long()
{
    Menu_LongPressureButton(B_F2);
}

void F3Long()
{
    Menu_LongPressureButton(B_F3);
}

void F4Long()
{
    Menu_LongPressureButton(B_F4);
}

void F5Long()
{
    Menu_LongPressureButton(B_F5);
}

//****************************************************************************************************************
int CalculateCount(int *prevTime)
{
    uint time = gTimerMS;
    uint delta = time - *prevTime;
    *prevTime = time;

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
        FPGA_TemporaryPause();
    }
    int count = CalculateCount(prevTime);
    int rShiftOld = RSHIFT(chan);
    int rShift = RSHIFT(chan) + relStep * count;
    if ((rShiftOld > RShiftZero && rShift < RShiftZero) || (rShiftOld < RShiftZero && rShift > RShiftZero))
    {
        rShift = RShiftZero;
    }
    if (CanChangeRShiftOrTrigLev((TrigSource)chan, rShift))
    {
        Sound_RegulatorShiftRotate();
        f(chan, rShift);
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
    if (CanChangeRShiftOrTrigLev(trigSource, trigLev))
    {
        Sound_RegulatorShiftRotate();
        f(trigSource, trigLev);
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
        Sound_RegulatorShiftRotate();
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

void RShift0Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, A, -STEP_RSHIFT);
}

void RShift0Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, A, +STEP_RSHIFT);
}

void RShift1Left()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, B, -STEP_RSHIFT);
}

void RShift1Right()
{
    static int prevTime = 0;
    ChangeRShift(&prevTime, FPGA_SetRShift, B, +STEP_RSHIFT);
}

void TrigLevLeft()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, FPGA_SetTrigLev, TRIG_SOURCE, -STEP_RSHIFT);
}

void TrigLevRight()
{
    static int prevTime = 0;
    ChangeTrigLev(&prevTime, FPGA_SetTrigLev, TRIG_SOURCE, +STEP_RSHIFT);
}

void XShift(int delta)
{
    static int prevTime = 0;
    if (!FPGA_IsRunning() || TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY)
    {
        if (!ENUM_POINTS_IS_281)
        {
            ChangeShiftScreen(&prevTime, Display_ShiftScreen, 2 * delta);
        }
    }
    else
    {
        ChangeTShift(&prevTime, FPGA_SetTShift, delta);
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
    Sound_RegulatorSwitchRotate();
    FPGA_RangeIncrease(A);
}

void Range0Right()
{
    Sound_RegulatorSwitchRotate();
    FPGA_RangeDecrease(A);
}

void Range1Left()
{
    Sound_RegulatorSwitchRotate();
    FPGA_RangeIncrease(B);
}

void Range1Right()
{
    Sound_RegulatorSwitchRotate();
    FPGA_RangeDecrease(B);
}

void TBaseLeft()
{
    Sound_RegulatorSwitchRotate();
    FPGA_TBaseIncrease();
}

void TBaseRight()
{
    Sound_RegulatorSwitchRotate();
    FPGA_TBaseDecrease();
}

void SetLeft()
{
    Menu_RotateRegSetLeft();
}

void SetRight()
{
    Menu_RotateRegSetRight();
}
