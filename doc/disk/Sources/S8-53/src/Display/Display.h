#pragma once


#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


void    Display_Init(void);
void    Display_Update(void);
void    Display_RotateRShift(Channel channel);
void    Display_RotateTrigLev(void);
void    Display_Redraw(void);
void    Display_EnableTrigLabel(bool enable);
// Функция должна вызываться для очистки поточечной памяти. Вызывается при изменении масштаба по напряжению.
void    Display_ResetP2Ppoints(bool empty);
// Добавляет точки в режиме поточечного вывода и самописца
// data00, data01 - для первого канала; data10, data11 - для второго канала
void    Display_AddPoints(uint8 data00, uint8 data01, uint8 data10, uint8 data11);
void    Display_ShowWarningBad(Warning warning);
void    Display_ShowWarningGood(Warning warning);
void    Display_ClearFromWarnings(void);
void    Display_SetDrawMode(DrawMode mode, pFuncVV func);
void    Display_SetAddDrawFunction(pFuncVV func);
void    Display_RemoveAddDrawFunction(void);
void    Display_Clear(void);
void    Display_ShiftScreen(int delta);
void    Display_ChangedRShiftMarkers(bool active);
void    Display_AddStringToIndicating(const char *string);
void    Display_OneStringUp(void);
void    Display_OneStringDown(void);
void    Display_SetPauseForConsole(bool pause);
void    Display_RunAfterDraw(pFuncVV func);         // После отрисовки очередного экрана эта функция будет вызвана один раз.
