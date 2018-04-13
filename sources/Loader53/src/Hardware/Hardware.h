#pragma once  
#include "defines.h"


#define PRIORITY_PANEL_EXTI9_5  3, 0
#define PRIORITY_PANEL_SPI1     4, 0


typedef enum
{
    TypeWriteDAC_RShift0,
    TypeWriteDAC_RShift1,
    TypeWriteDAC_TrigLev
} TypeWriteDAC;

typedef enum
{
    TypeWriteAnalog_All,
    TypeWriteAnalog_Range0,
    TypeWriteAnalog_Range1,
    TypeWriteAnalog_TrigParam,
    TypeWriteAnalog_ChanParam0,
    TypeWriteAnalog_ChanParam1
} TypeWriteAnalog;


void Hardware_Init(void);

void Hardware_WriteToAnalog(TypeWriteAnalog type, uint data);

void Hardware_WriteToDAC(TypeWriteDAC type, uint16 data);

#define HARDWARE_ERROR HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__);
void HardwareErrorHandler(const char *file, const char *function, int line);
