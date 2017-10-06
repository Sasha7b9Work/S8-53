#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA_Types.h"
#include "FPGA/DataStorage.h"


void Processing_SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);         // Установить сигнал для обработки.
void Processing_GetData(uint8 **data0, uint8 **data1, DataSettings **ds);        // Получить данные ранее установленного сигнала.
float Processing_GetCursU(Channel chan, float posCurT);                           // Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT.
float Processing_GetCursT(Channel chan, float posCurU, int numCur);               // Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU.
void Processing_InterpolationSinX_X(uint8 data[FPGA_MAX_POINTS], TBase tBase);   // Аппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
char* Processing_GetStringMeasure(Measure measure, Channel chan, char buffer[20]);// Возвращает строку автоматического измерения.
void Processing_CalculateMeasures();                                         // Расчитать все измерения.
int Processing_GetMarkerHorizontal(Channel chan, int numMarker);                // Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
int Processing_GetMarkerVertical(Channel chan, int numMarker);                  // Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
