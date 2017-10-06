#include "Menu.h"
#include "MenuItemsLogic.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/RTC.h"
#include "Utils/Math.h"



int8 gCurDigit = 0;


void Governor_StartChange(Governor *governor, int delta)
{
    sound.GovernorChangedValue();
    if (delta > 0 && gBF.addressGovernor == (uint)governor && gBF.inMoveIncrease == 1)
    {
        *governor->cell = Governor_NextValue(governor);
    }
    else if (delta < 0 && gBF.addressGovernor == (uint)governor && gBF.inMoveDecrease == 1)
    {
        *governor->cell = Governor_PrevValue(governor);
    }
    else
    {
        gBF.timeStartMS = gTimerMS;   
    }
    gBF.inMoveIncrease = (delta > 0) ? 1 : 0;
    gBF.inMoveDecrease = (delta < 0) ? 1 : 0;
}

void Governor_ChangeValue(Governor *governor, int delta)
{
    int16 oldValue = *governor->cell;
    LIMITATION(*governor->cell, oldValue + Math_Sign(delta) * Math_Pow10(gCurDigit), governor->minValue, governor->maxValue);
    if (*governor->cell != oldValue)
    {
        if (governor->funcOfChanged)
        {
            governor->funcOfChanged();
        }
        sound.GovernorChangedValue();
    }
}

void IPaddress_ChangeValue(IPaddress *ip, int delta)
{
    int numByte = 0;
    int numPos = 0;

    IPaddress_GetNumPosIPvalue(&numByte, &numPos);

    int oldValue = 0;

    if (numByte < 4)
    {
        uint8 *bytes = ip->ip0;
        oldValue = bytes[numByte];
    }
    else
    {
        oldValue = *ip->port;
    }

    int newValue = oldValue + Math_Sign(delta) * Math_Pow10(numPos);
    LIMITATION(newValue, newValue, 0, numByte == 4 ? 65535 : 255);

    if (oldValue != newValue)
    {
        if (numByte == 4)
        {
            *ip->port = newValue;
        }
        else
        {
            ip->ip0[numByte] = newValue;
        }
        sound.GovernorChangedValue();
        display.ShowWarningGood(NeedRebootDevice2);
        display.ShowWarningGood(NeedRebootDevice1);
    }
}

void MACaddress_ChangeValue(MACaddress *mac, int delta)
{
    uint8 *value = mac->mac0 + gCurDigit;
    *value += delta > 0 ? 1 : -1;
    sound.GovernorChangedValue();
    display.ShowWarningGood(NeedRebootDevice2);
    display.ShowWarningGood(NeedRebootDevice1);
}

void IPaddress_GetNumPosIPvalue(int *numIP, int *selPos)
{
    if (gCurDigit < 12)
    {
        *numIP = gCurDigit / 3;
        *selPos = 2 - (gCurDigit - (*numIP * 3));
    }
    else
    {
        *numIP = 4;
        *selPos = 4 - (gCurDigit - 12);
    }


}

float Governor_Step(Governor *governor)
{
    static const float speed = 0.05f;
    static const int numLines = 10;
    if (gBF.addressGovernor == (uint)governor && gBF.inMoveDecrease == 1)
    {
        float delta = -speed * (gTimerMS - gBF.timeStartMS);
        if (delta == 0.0f)
        {
            return -0.001f;
        }
        if (delta < -numLines)
        {
            gBF.inMoveDecrease = gBF.inMoveIncrease = 0;
            *governor->cell = Governor_PrevValue(governor);
            if (governor->funcOfChanged)
            {
                governor->funcOfChanged();
            }
            return 0.0f;
        }
        return delta;
    }
    if (gBF.addressGovernor == (uint)governor && gBF.inMoveIncrease == 1)
    {
        float delta = speed * (gTimerMS - gBF.timeStartMS);
        if (delta == 0.0f)
        {
            return 0.001f;
        }
        if (delta > numLines)
        {
            gBF.inMoveDecrease = gBF.inMoveIncrease = 0;
            *governor->cell = Governor_NextValue(governor);
            if(governor->funcOfChanged)
            {
                governor->funcOfChanged();
            }
            return 0.0f;
        }
        return delta;
    }
    return 0.0f;
}

int16 Governor_NextValue(Governor *governor)
{
    return ((*governor->cell) < governor->maxValue) ? (*governor->cell) + 1 : governor->minValue;
}

int16 Governor_PrevValue(Governor *governor)
{
    return ((*governor->cell) > governor->minValue) ? (*governor->cell) - 1 : governor->maxValue;
}

void Governor_NextPosition(Governor *governor)
{
    if (menu.OpenedItem() == governor)
    {
        CircleIncreaseInt8(&gCurDigit, 0, Governor_NumDigits(   governor) - 1);
    }
}

int Governor_NumDigits(Governor *governor)
{
    int min = Math_NumDigitsInNumber(Math_FabsInt(governor->minValue));
    int max = Math_NumDigitsInNumber(Math_FabsInt(governor->maxValue));
    if (min > max)
    {
        max = min;
    }
    return max;
}

void IPaddress_NextPosition(IPaddress *ipEthernet_IP)
{
    CircleIncreaseInt8(&gCurDigit, 0, ipEthernet_IP->port == 0 ? 11 : 16);
}

void ItemTime_SetOpened(Time *item)
{
    PackedTime time = RTC_GetPackedTime();
    *(item->seconds) = time.seconds;
    *(item->minutes) = time.minutes;
    *(item->hours) = time.hours;
    *(item->day) = time.day;
    *(item->month) = time.month;
    *(item->year) = time.year ;
}

void ItemTime_SetNewTime(Time *time)
{
    RTC_SetTimeAndData(*time->day, *time->month, *time->year, *time->hours, *time->minutes, *time->seconds);
}

void ItemTime_SelectNextPosition(Time *time)
{
    CircleIncreaseInt8(time->curField, 0, 7);
    painter.ResetFlash();
}

void ItemTime_IncCurrentPosition(Time *time)
{
    sound.GovernorChangedValue();
    int8 *value[] = {0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds};
    int8 position = *time->curField;
    if (position != iSET && position != iEXIT)
    {
        static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
        static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
        *(value[position]) = (*(value[position]))++;
        if (*value[position] > max[position])
        {
            *value[position] = min[position];
        }
    }
}

void ItemTime_DecCurrentPosition(Time *time)
{
    sound.GovernorChangedValue();
    static const int8 max[] = {0, 31, 12, 99, 23, 59, 59};
    static const int8 min[] = {0, 1, 1, 15, 0, 0, 0};
    int8 *value[] = {0, time->day, time->month, time->year, time->hours, time->minutes, time->seconds};
    int8 position = *time->curField;
    if (position != iSET && position != iEXIT)
    {
        (*(value[position]))--;
        if (*value[position] < min[position])
        {
            *value[position] = max[position];
        }
    }
}

void GovernorColor_ChangeValue(GovernorColor *governor, int delta)
{
    ColorType *ct = governor->colorType;
    if (ct->currentField == 0)
    {
        Color_BrightnessChange(ct, delta);
    }
    else
    {
        Color_ComponentChange(ct, delta);
    }
}

