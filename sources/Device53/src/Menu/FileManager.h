#pragma once 
#include "defines.h"


/// Вызывается один раз при обнаружении новой флешки.
void FM_Init();

void FM_Draw();

void PressSB_FM_LevelUp();

void PressSB_FM_LevelDown();

void FM_RotateRegSet(int angle);

bool FM_GetNameForNewFile(char name[255]);

bool FM_FileIsExist(char name[255]);
