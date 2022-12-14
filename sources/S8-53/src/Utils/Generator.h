#pragma once


#include "defines.h"
#include "Settings/SettingsTypes.h"


typedef enum
{
    Wave_Sinus,
    Wave_Meander
} TypeWave;

typedef struct
{
    void(*SetParametersWave)(Channel channel, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);    // numWave = 1, 2. ????? ???? ?? ???? ????????.
                                                                                                    // amplWave = 0.0f...1.0f, amplNoise = 0.0f...1.0f - ???????????? ????????? ???????.
    void(*StartNewWave)(Channel channel);
    uint8(*GetSampleWave)(Channel channel);
} SGenerator;

extern const SGenerator Generator;
