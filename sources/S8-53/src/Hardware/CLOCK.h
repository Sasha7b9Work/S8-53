#pragma once
#include "defines.h"


class Clock
{
public:
    static void Init();
    static bool SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);
    static PackedTime GetPackedTime();

private:
    static const RTC_HandleTypeDef handle;
};
