#include "Math.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "FPGA/FPGA_Types.h"
#include "Display/Grid.h"
#include "Hardware/Timer.h"


#include <math.h>


const float tableScalesRange[RangeSize] = {2e-3f, 5e-3f, 10e-3f, 20e-3f, 50e-3f, 100e-3f, 200e-3f, 500e-3f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f};
/*
static const float tableScalesTBase[TBaseSize] = 
    {2e-9, 5e-9, 10e-9, 20e-9, 50e-9, 100e-9, 200e-9, 500e-9,
    1e-6, 2e-6, 5e-6, 10e-6, 20e-6, 50e-6, 100e-6, 200e-6, 500e-6,
    1e-3, 2e-3, 5e-3, 10e-3, 20e-3, 50e-3, 100e-3, 200e-3, 500e-3,
    1.0f, 2.0f, 5.0f, 10.0f};
*/

const float absStepRShift[] =
{
    2e-3f / 20 / STEP_RSHIFT,
    5e-3f / 20 / STEP_RSHIFT,
    10e-3f / 20 / STEP_RSHIFT,
    20e-3f / 20 / STEP_RSHIFT,
    50e-3f / 20 / STEP_RSHIFT,
    100e-3f / 20 / STEP_RSHIFT,
    200e-3f / 20 / STEP_RSHIFT,
    500e-3f / 20 / STEP_RSHIFT,
    1.0f / 20 / STEP_RSHIFT,
    2.0f / 20 / STEP_RSHIFT,
    5.0f / 20 / STEP_RSHIFT,
    10.0f / 20 / STEP_RSHIFT,
    20.0f / 20 / STEP_RSHIFT
};

const float voltsInPixel[] =
{
    2e-3f / 20,      // 2mV
    5e-3f / 20,      // 5mV
    10e-3f / 20,     // 10mV
    20e-3f / 20,     // 20mV
    50e-3f / 20,     // 50mV
    100e-3f / 20,    // 100mV
    200e-3f / 20,    // 200mV
    500e-3f / 20,    // 500mV
    1.0f / 20,       // 1V
    2.0f / 20,       // 2V
    5.0f / 20,       // 5V
    10.0f / 20,      // 10V
    20.0f / 20       // 20V
};

const int voltsInPixelInt[] =   // Коэффициент 20000
{
    2,      // 2
    5,      // 5
    10,     // 10
    20,     // 20
    50,     // 50
    100,    // 100
    200,    // 200
    500,    // 500
    1000,   // 1
    2000,   // 2
    5000,   // 5
    1000,   // 10
    20000   // 20
};

const float absStepTShift[] =
{
    2e-9f / 20, 5e-9f / 20, 10e-9f / 20, 20e-9f / 20, 50e-9f / 20, 100e-9f / 20, 200e-9f / 20, 500e-9f / 20,
    1e-6f / 20, 2e-6f / 20, 5e-6f / 20, 10e-6f / 20, 20e-6f / 20, 50e-6f / 20, 100e-6f / 20, 200e-6f / 20, 500e-6f / 20,
    1e-3f / 20, 2e-3f / 20, 5e-3f / 20, 10e-3f / 20, 20e-3f / 20, 50e-3f / 20, 100e-3f / 20, 200e-3f / 20, 500e-3f / 20,
    1.0f / 20, 2.0f / 20, 5.0f / 20, 10.0f / 20
};

int Math_MinFrom2Int(int val0, int val1)
{
    if (val0 < val1)
    {
        return val0;
    }
    return val1;
}

int Math_RShift2Rel(float rShiftAbs, Range range)
{
    int retValue = RShiftZero + rShiftAbs / absStepRShift[range];
    if(retValue < RShiftMin)
    {
        retValue = RShiftMin;
    }
    else if(retValue > RShiftMax)
    {
        retValue = RShiftMax;
    }
    return retValue;
};

float Math_VoltageCursor(float shiftCurU, Range range, int16 rShift)
{
    return MAX_VOLTAGE_ON_SCREEN(range) - shiftCurU * voltsInPixel[range] - RSHIFT_2_ABS(rShift, range);
}

float Math_TimeCursor(float shiftCurT, TBase tBase)
{
    return shiftCurT * absStepTShift[tBase];
}

void Math_DataExtrapolation(uint8 *data, uint8 *there, int size)
{
    int pointer = 0;
    while(there[pointer] == 0)
    {
        pointer++;
    }
    for(int i = 0; i < pointer; i++)
    {
        data[i] = data[pointer];
    }
    
    while(pointer < size)
    {
        while(there[pointer] != 0)
        {
            pointer++;
            if(pointer >= size)
            {
                return;
            }
        }
        pointer--;
        int pointer2 = pointer + 1;
        while(there[pointer2] == 0)
        {
            pointer2++;
            if(pointer2 >= size)
            {
                return;
            }
        }
        int deltaY = pointer2 - pointer;
        if(deltaY >= 2)
        {
            float deltaX = (float)(data[pointer2] - data[pointer]) / deltaY;
            for(int i = 1; i < deltaY; i++)
            {
                data[pointer + i] = data[pointer] + i * deltaX;
            }
        }
        pointer = pointer2 + 1;
    }
}

void Math_PointsRelToVoltage(const uint8 *points, int numPoints, Range range, int16 rShift, float *voltage)
{
    int voltInPixel = voltsInPixelInt[range];
    float maxVoltsOnScreen = MAX_VOLTAGE_ON_SCREEN(range);
    float rShiftAbs = RSHIFT_2_ABS(rShift, range);
    int diff = (MIN_VALUE * voltInPixel) + (maxVoltsOnScreen + rShiftAbs) * 20e3;;
    float koeff = 1.0f / 20e3f;
    for (int i = 0; i < numPoints; i++)
    {
        voltage[i] = (points[i] * voltInPixel - diff) * koeff;
    }
}

void Math_PointsVoltageToRel(const float *voltage, int numPoints, Range range, int16 rShift, uint8 *points)
{
    float maxVoltOnScreen = MAX_VOLTAGE_ON_SCREEN(range);
    float rShiftAbs = RSHIFT_2_ABS(rShift, range);
    float voltInPixel = 1.0f / voltsInPixel[range];

    float add = maxVoltOnScreen + rShiftAbs;

    float delta = add * voltInPixel + MIN_VALUE;

    for (int i = 0; i < numPoints; i++)
    {
        int value = voltage[i] * voltInPixel + delta;
        if (value < 0)
        {
            points[i] = 0;
            continue;
        }
        else if (value > 255)
        {
            points[i] = 255;
            continue;
        }
        points[i] = (uint8)value;
    }
}

uint8 Math_VoltageToPoint(float voltage, Range range, int16 rShift)
{
    int relValue = (voltage + MAX_VOLTAGE_ON_SCREEN(range) + RSHIFT_2_ABS(rShift, range)) / voltsInPixel[range] + MIN_VALUE;
    LIMITATION(relValue, relValue, 0, 255);
    return (uint8)relValue;
}

float Math_GetIntersectionWithHorizontalLine(int x0, int y0, int x1, int y1, int yHorLine)
{
    if(y0 == y1)
    {
        return x1;
    }

    return (yHorLine - y0) / ((float)(y1 - y0) / (float)(x1 - x0)) + x0;
}

bool Math_FloatsIsEquals(float value0, float value1, float epsilonPart)
{
    float max = fabs(value0) > fabs(value1) ? fabs(value0) : fabs(value1);

    float epsilonAbs = max * epsilonPart;

    return fabs(value0 - value1) < epsilonAbs;
}

float Math_MinFrom3float(float value1, float value2, float value3)
{
    float retValue = value1;
    if(value2 < retValue)
    {
        retValue = value2;
    }
    if(value3 < retValue)
    {
        retValue = value3;
    }
    return retValue;
}

int Math_MinInt(int val1, int val2)
{
    return val1 < val2 ? val1 : val2;
}

/*
    Быстрое преобразование Фурье. Вычисляет модуль спектра для дейсвтительного сигнала.
    Количество отсчётов должно быть 2**N
*/

#ifndef DEBUG
#include "TablesWindow.h"
#include "TablesLog.h"

static float const *Koeff(int numPoints)
{
    float const *tables[3][4] = {
        {koeffsNorm256, koeffsHamming256, koeffsBlack256, koeffsHann256},
        {koeffsNorm512, koeffsHamming512, koeffsBlack512, koeffsHann512},
        {koeffsNorm1024, koeffsHamming1024, koeffsBlack1024, koeffsHann1024},
    };

    int row = 0;
    if (numPoints == 512)
    {
        row = 1;
    }
    else if (numPoints == 1024)
    {
        row = 2;
    }

    return tables[row][PageServiceMath_GetWindowFFT()];
}

#endif

static void Normalize(float *data, int numPoints)
{
    float max = 0.0;
    for (int i = 0; i < 256; i++)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
    }

    for (int i = 0; i < 256; i++)
    {
        data[i] /= max;
    }
}

static void MultiplyToWindow(float *data, int numPoints)
{
#ifndef DEBUG
    float const *koeff = Koeff(numPoints);

    for (int i = 0; i < numPoints; i++)
    {
        data[i] *= koeff[i];
    }
#else
    if (WINDOW_FFT_IS_HAMMING)
    {
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= 0.53836 - 0.46164 * cos(2 * 3.1415926 * i / (numPoints - 1));
        }
    }
    else if (WINDOW_FFT_IS_BLACKMAN)
    {
        float alpha = 0.16f;
        float a0 = (1.0f - alpha) / 2.0f;
        float a1 = 0.5f;
        float a2 = alpha / 2.0f;
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= a0 - a1 * cos(2 * 3.1415926 * i / (numPoints - 1)) + a2 * cos(4 * 3.1415926 * i / (numPoints - 1));
        }
    }
    else if (WINDOW_FFT_IS_HANN)
    {
        for (int i = 0; i < numPoints; i++)
        {
            data[i] *= 0.5f * (1.0f - cos(2.0f * 3.1415926 * i / (numPoints - 1.0f)));
        }
    }
#endif
}

#define SET_MIN_MAX(x)		\
	if (x < min) min = x;	\
	if (x > max) max = x;

void Math_CalculateFFT(float *dataR, int numPoints, float *result, float *freq0, float *density0, float *freq1, float *density1, int *y0, int *y1)
{
    float scale = 1.0 / absStepTShift[SET_TBASE] / 1024.0;

    float K = 1024.0 / numPoints;

    *freq0 = scale * FFT_POS_CURSOR_0 * K;
    *freq1 = scale * FFT_POS_CURSOR_1 * K;
    if (PEAKDET)
    {
        *freq0 *= 2;
        *freq1 *= 2;
    }

    for (int i = 0; i < numPoints; i++)
    {
        result[i] = 0.0f;
    }

    MultiplyToWindow(dataR, numPoints);

    int logN = 8;
    if (numPoints == 512)
    {
        logN = 9;
    }
    else if (numPoints == 1024)
    {
        logN = 10;
    }
    static const float Rcoef[14] =
    {
        -1.0000000000000000F, 0.0000000000000000F, 0.7071067811865475F,
        0.9238795325112867F, 0.9807852804032304F, 0.9951847266721969F,
        0.9987954562051724F, 0.9996988186962042F, 0.9999247018391445F,
        0.9999811752826011F, 0.9999952938095761F, 0.9999988234517018F,
        0.9999997058628822F, 0.9999999264657178F
    };

    static const float Icoef[14] =
    {
        0.0000000000000000F, -1.0000000000000000F, -0.7071067811865474F,
        -0.3826834323650897F, -0.1950903220161282F, -0.0980171403295606F,
        -0.0490676743274180F, -0.0245412285229122F, -0.0122715382857199F,
        -0.0061358846491544F, -0.0030679567629659F, -0.0015339801862847F,
        -0.0007669903187427F, -0.0003834951875714F
    };

    int nn = numPoints >> 1;
    int ie = numPoints;

    for (int n = 1; n <= logN; n++) 
    {
        float rw = Rcoef[logN - n];
        float iw = Icoef[logN - n];
        int in = ie >> 1;
        float ru = 1.0f;
        float iu = 0.0f;
        for (int j = 0; j < in; j++) 
        {
            for (int i = j; i < numPoints; i += ie) 
            {
                int io = i + in;
                float dRi = dataR[i];
                float dRio = dataR[io];
                float ri = result[i];
                float rio = result[io];
                dataR[i] = dRi + dRio;
                result[i] = ri + rio;
                float rtq = dRi - dRio;
                float itq = ri - rio;
                dataR[io] = rtq * ru - itq * iu;
                result[io] = itq * ru + rtq * iu;
            }
            float sr = ru;
            ru = ru * rw - iu * iw;
            iu = iu * rw + sr * iw;
        }
        ie >>= 1;
    }

    for (int j = 1, i = 1; i < numPoints; i++) 
    {
        if (i < j)
        {
            int io = i - 1;
            int in = j - 1;
            float rtp = dataR[in];
            float itp = result[in];
            dataR[in] = dataR[io];
            result[in] = result[io];
            dataR[io] = rtp;
            result[io] = itp;
        }

        int k = nn;

        while (k < j)
        {
            j = j - k;
            k >>= 1;
        }

        j = j + k;
    }

    for (int i = 0; i < 256; i++)
    {
        result[i] = sqrt(dataR[i] * dataR[i] + result[i] * result[i]);
    }

    result[0] = 0.0f;       // WARN нулевая составляющая мешает постоянно. надо её убрать

    Normalize(result, 256);

    if (SCALE_FFT_IS_LOG)
    {
        float minDB = sService_MathGetFFTmaxDBabs();

        for (int i = 0; i < 256; i++)
        {
#ifdef DEBUG
            result[i] = 20 * log10(result[i]);
#else
            result[i] = Log10[(int)(result[i] * 10000)];
#endif
            if (i == FFT_POS_CURSOR_0)
            {
                *density0 = result[i];
            }
            else if (i == FFT_POS_CURSOR_1)
            {
                *density1 = result[i];
            }
            if (result[i] < minDB)
            {
                result[i] = minDB;
            }
            result[i] = 1.0f - result[i] / minDB;
        }
    }
    else
    {
        *density0 = result[FFT_POS_CURSOR_0];
        *density1 = result[FFT_POS_CURSOR_1];
    }
    *y0 = Grid::MathBottom() - result[FFT_POS_CURSOR_0] * Grid::MathHeight();
    *y1 = Grid::MathBottom() - result[FFT_POS_CURSOR_1] * Grid::MathHeight();
}

void Math_CalculateMathFunction(float *data0andResult, float *data1, int numPoints)
{
    if (MATH_FUNC_IS_SUM)
    {
        int delta = data1 - data0andResult;
        float *end = &data0andResult[numPoints];
        while (data0andResult < end)
        {
            *data0andResult += *(data0andResult + delta);
            data0andResult++;
        }
    }
    else if (MATH_FUNC_IS_MUL)
    {
        int delta = data1 - data0andResult;
        float *end = &data0andResult[numPoints];
        while (data0andResult < end)
        {
            *data0andResult *= *(data0andResult + delta);
            data0andResult++;
        }
    }
}

float Math_RandFloat(float min, float max)
{
    float delta = max - min;
    return min + ((rand() / (float)RAND_MAX) * delta);
}

int8 Math_AddInt8WithLimitation(int8 value, int8 delta, int8 min, int8 max)
{
    int8 retValue = value + delta;
    if (retValue < 0)
    {
        return 0;
    }
    if (retValue > max)
    {
        return max;
    }
    return retValue;
}

int Math_Sign(int value)
{
    if (value > 0)
    {
        return 1;
    }
    if (value < 0)
    {
        return -1;
    }
    return 0;
}

int Math_Pow10(int pow)
{
    int retValue = 1;

    while (pow--)
    {
        retValue *= 10;
    }

    return retValue;
}

int Math_NumDigitsInNumber(int value)
{
    value = Math_FabsInt(value);
    int num = 1;
    while ((value /= 10) > 0)
    {
        num++;
    }
    return num;
}

int Math_FabsInt(int value)
{
    return value >= 0 ? value : -value;
}

uint8 Math_GetMaxFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint)
{
    uint8 max = Math_GetMaxFromArray(data, firstPoint, lastPoint);
    if (max >= MAX_VALUE)
    {
        max = ERROR_VALUE_UINT8;
    }
    return max;
}

uint8 Math_GetMinFromArrayWithErrorCode(const uint8 *data, int firstPoint, int lastPoint)
{
    uint8 min = Math_GetMinFromArray(data, firstPoint, lastPoint);
    if (min < MIN_VALUE || min >= MAX_VALUE)
    {
        min = ERROR_VALUE_UINT8;
    }
    return min;
}

uint8 Math_GetMinFromArray(const uint8 *data, int firstPoint, int lastPoint)
{

#define MIN_IF_LESS if(d < min) { min = d; }

    uint8 min = 255;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i += 2)
    {
        uint8 d = *pointer++;
        MIN_IF_LESS
        d = *pointer++;
        MIN_IF_LESS
    }
    if ((lastPoint - firstPoint + 1) & 1)
    {
        uint8 d = *pointer;
        MIN_IF_LESS
    }

    return min;
}

uint8 Math_GetMaxFromArray(const uint8 *data, int firstPoint, int lastPoint)
{

#define MAX_IF_ABOVE if(d > max) { max = d; }

    uint8 max = 0;
    const uint8 *pointer = &data[firstPoint];

    for (int i = firstPoint; i < lastPoint; i += 2)
    {
        uint8 d = *pointer++;
        MAX_IF_ABOVE;
        d = *pointer++;
        MAX_IF_ABOVE;
    }
    if ((lastPoint - firstPoint + 1) & 1)
    {
        uint8 d = *pointer;
        MAX_IF_ABOVE
    }

    return max;
}

#define LIMIT                           \
    if (value <= min) { return min; }   \
    if (value >= max) { return max; }   \
    return value;

int LimitationInt(int value, int min, int max)
{
    LIMIT
}

uint8 LimitationUInt8(uint8 value, uint8 min, uint8 max)
{
    LIMIT
}

float LimitationFloat(float value, float min, float max)
{
    LIMIT
}

uint8 Math_CalculateFiltr(const uint8 *data, int x, int numPoints, int numSmoothing)
{
    if (numSmoothing < 2)
    {
        return data[x];
    }

    int count = 1;
    int sum = data[x];
    int startDelta = 1;

    int endDelta = numSmoothing / 2;

    for (int delta = startDelta; delta <= endDelta; delta++)
    {
        if (((x - delta) >= 0) && ((x + delta) < (numPoints)))
        {
            sum += data[x - delta];
            sum += data[x + delta];
            count += 2;
        }
    }

    if ((numSmoothing % 2) == 1)
    {
        int delta = numSmoothing / 2;
        if ((x + delta) < numPoints)
        {
            sum += data[x + delta];
            count++;
        }
    }

    return (uint8)(sum / count);
}

void Math_CalculateFiltrArray(const uint8 *dataIn, uint8 *dataOut, int numPoints, int numSmoothing)
{
    if (numSmoothing < 2)
    {
        memcpy(dataOut, dataIn, numPoints);
    }
    else
    {
        bool addCalculation = (numSmoothing % 2) == 1;
        int endDelta = numSmoothing / 2;
        int startDelta = 1;
        int d = numSmoothing / 2;

        for (int i = 0; i < numPoints; i++)
        {
            int count = 1;
            int sum = dataIn[i];

            for (int delta = startDelta; delta <= endDelta; delta++)
            {
                if (((i - delta) >= 0) && ((i + delta) < (numPoints)))
                {
                    sum += dataIn[i - delta];
                    sum += dataIn[i + delta];
                    count += 2;
                }
            }

            if (addCalculation)
            {
                if ((i + d) < numPoints)
                {
                    sum += dataIn[i + d];
                    count++;
                }
            }

            dataOut[i] = (uint8)(sum / count);
        }
    }
}
