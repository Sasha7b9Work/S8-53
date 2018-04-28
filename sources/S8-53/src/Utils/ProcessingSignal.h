#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    /// Установить сигнал для обработки.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    /// Получить данные ранее установленного сигнала.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    /// Получить позицию курсора напряжения, соответствующю заданной позиции курсора posCurT.
    static float GetCursU(Channel chan, float posCurT);
    /// Получить позицию курсора времени, соответствующую заданной позиции курсора напряжения posCurU.
    static float GetCursT(Channel chan, float posCurU, int numCur);
    /// Аппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA_MAX_POINTS], TBase tBase);
    /// Возвращает строку автоматического измерения.
    static char* GetStringMeasure(Measure measure, Channel chan, char buffer[20]);
    /// Расчитать все измерения.
    static void CalculateMeasures();
    /// Возвращает значение горизонтального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerHorizontal(Channel chan, int numMarker);
    /// Возвращает значение вертикального маркера. Если ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerVertical(Channel chan, int numMarker);
};
