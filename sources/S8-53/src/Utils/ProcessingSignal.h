#pragma once
#include "Measures.h"
#include "defines.h"
#include "FPGA/FPGA_Types.h"


class Processing
{
public:
    /// ”становить сигнал дл€ обработки.
    static void SetSignal(uint8 *data0, uint8 *data1, DataSettings *ds, int firstPoint, int lastPoint);
    /// ѕолучить данные ранее установленного сигнала.
    static void GetData(uint8 **data0, uint8 **data1, DataSettings **ds);
    /// ѕолучить позицию курсора напр€жени€, соответствующю заданной позиции курсора posCurT.
    static float GetCursU(Channel chan, float posCurT);
    /// ѕолучить позицию курсора времени, соответствующую заданной позиции курсора напр€жени€ posCurU.
    static float GetCursT(Channel chan, float posCurU, int numCur);
    /// јппроксимировать единичное измерение режима рандомизатора функцией sinX/X.
    static void InterpolationSinX_X(uint8 data[FPGA_MAX_POINTS], TBase tBase);
    /// ¬озвращает строку автоматического измерени€.
    static char* GetStringMeasure(Measure measure, Channel chan, char buffer[20]);
    /// –асчитать все измерени€.
    static void CalculateMeasures();
    /// ¬озвращает значение горизонтального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerHorizontal(Channel chan, int numMarker);
    /// ¬озвращает значение вертикального маркера. ≈сли ERROR_VALUE_INT - маркер рисовать не нужно.
    static int GetMarkerVertical(Channel chan, int numMarker);
private:
    static float CalculateVoltageMax(Channel chan);
    static float CalculateVoltageMin(Channel chan);
    static float CalculateVoltagePic(Channel chan);
    static float CalculateVoltageMaxSteady(Channel chan);
    static float CalculateVoltageMinSteady(Channel chan);
    static float CalculateVoltageAmpl(Channel chan);
    static float CalculateVoltageAverage(Channel chan);
    static float CalculateVoltageRMS(Channel chan);
    static float CalculateVoltageVybrosPlus(Channel chan);
    static float CalculateVoltageVybrosMinus(Channel chan);
    static float CalculatePeriod(Channel chan);
    /// “очно вычисл€ет период или целое число периодов в точках сигнала.
    static int   CalculatePeriodAccurately(Channel chan);
    static float CalculateFreq(Channel chan);
    static float CalculateTimeNarastaniya(Channel chan);
    static float CalculateTimeSpada(Channel chan);
    static float CalculateDurationPlus(Channel chan);
    static float CalculateDurationMinus(Channel chan);
    static float CalculateSkvaznostPlus(Channel chan);
    static float CalculateSkvaznostMinus(Channel chan);
    /// ¬озвращает минимальное значение относительного сигнала    
    static float CalculateMinRel(Channel chan);
    /// ¬озвращает минимальное установившеес€ значение относительного сигнала
    static float CalculateMinSteadyRel(Channel chan);
    /// ¬озвращает максимальное значение относительного сигнала
    static float CalculateMaxRel(Channel chan);
    /// ¬озвращает максимальное установившеес€ значение относительного сигнала
    static float CalculateMaxSteadyRel(Channel chan);
    /// ¬озвращает среденее значение относительного сигнала
    static float CalculateAverageRel(Channel chan);
    static float CalculatePicRel(Channel chan);
    static float CalculateDelayPlus(Channel chan);
    static float CalculateDelayMinus(Channel chan);
    static float CalculatePhazaPlus(Channel chan);
    static float CalculatePhazaMinus(Channel chan);
    /// Ќайти точку пересечени€ сигнала с горизонтальной линией, проведЄнной на уровне yLine. numItersection - пор€дковый номер пересечени€, 
    /// начинаетс€ с 1. downToTop - если true, ищем пересечение сигнала со средней линией при прохождении из "-" в "+".
    static float FindIntersectionWithHorLine(Channel chan, int numIntersection, bool downToUp, uint8 yLine);

    static void CountedToCurrentSettings();

    typedef float (*pFuncFCh)(Channel);

    struct MeasureCalculate
    {
        char        *name;
        pFuncFCh    FuncCalculate;
        pFuncCFBC   FucnConvertate;
        /// ≈сли true, нужно показывать знак.
        bool        showSign;
    };

    static const MeasureCalculate measures[Measure_NumMeasures];
};
