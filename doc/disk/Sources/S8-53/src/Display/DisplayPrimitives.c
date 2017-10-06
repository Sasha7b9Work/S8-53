// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "DisplayPrimitives.h"
#include "Colors.h"
#include "Painter.h"
#include <stdio.h> 


void ProgressBar_Draw(ProgressBar *bar)
{
    int x = bar->x;
    int y = bar->y;
    char buffer[100] = {0};
    float passedPercents = bar->fullTime == 0 ? 0 : bar->passedTime / bar->fullTime * 100;
    sprintf(buffer, "Завершено %.1f %%", passedPercents);
    Painter_DrawStringInCenterRectC(x, y - 15, bar->width, bar->height, buffer, COLOR_FILL);
    Painter_DrawRectangle(bar->x, bar->y, bar->width, bar->height);
    Painter_FillRegion(bar->x, bar->y, bar->width * passedPercents / 100.0f, bar->height);
    buffer[0] = 0;
    sprintf(buffer, "Осталось %.1f с", (int)(bar->fullTime - bar->passedTime) / 1000.0f);
    Painter_DrawStringInCenterRect(x, y + bar->height, bar->width, bar->height, buffer);
}
