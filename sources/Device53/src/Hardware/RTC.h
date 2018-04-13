#pragma once
#include "defines.h"


void RTC_Init();

bool RTC_SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds);

PackedTime RTC_GetPackedTime();
