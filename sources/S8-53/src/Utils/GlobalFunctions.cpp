#include "GlobalFunctions.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Display/Symbols.h"
#include "Math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


char* FloatFract2String(float value, bool alwaysSign, char bufferOut[20])
{
    return Float2String(value, alwaysSign, 4, bufferOut);
}

static int NumDigitsInIntPart(float value)
{
    float fabsValue = fabs(value);

    int numDigitsInInt = 0;
    if (fabsValue >= 10000)
    {
        numDigitsInInt = 5;
    }
    else if (fabsValue >= 1000)
    {
        numDigitsInInt = 4;
    }
    else if (fabsValue >= 100)
    {
        numDigitsInInt = 3;
    }
    else if (fabsValue >= 10)
    {
        numDigitsInInt = 2;
    }
    else
    {
        numDigitsInInt = 1;
    }

    return numDigitsInInt;
}

char* Float2String(float value, bool alwaysSign, int numDigits, char bufferOut[20])
{
    bufferOut[0] = 0;
    char *pBuffer = bufferOut;

    if(value == ERROR_VALUE_FLOAT)
    {
        strcat(bufferOut, ERROR_STRING_VALUE);
        return bufferOut;
    }

    if(!alwaysSign)
    {
        if(value < 0)
        {
            *pBuffer = '-';
            pBuffer++;
        }
    }
    else
    {
        *pBuffer = value < 0 ? '-' : '+';
        pBuffer++;
    }

    char format[] = "%4.2f\0\0";

    format[1] = (char)numDigits + 0x30;

    int numDigitsInInt = NumDigitsInIntPart(value);

    format[3] = (numDigits - numDigitsInInt) + 0x30;
    if(numDigits == numDigitsInInt)
    {
        format[5] = '.';
    }
    
    snprintf(pBuffer, 19, format, fabs(value));

    float val = atof(pBuffer);

    if (NumDigitsInIntPart(val) != numDigitsInInt)
    {
        numDigitsInInt = NumDigitsInIntPart(val);
        format[3] = (numDigits - numDigitsInInt) + 0x30;
        if (numDigits == numDigitsInInt)
        {
            format[5] = '.';
        }
        sprintf(pBuffer, format, value);
    }

    bool signExist = alwaysSign || value < 0;
    while(strlen(bufferOut) < numDigits + (signExist ? 2 : 1))
    {
        strcat(bufferOut, "0");
    }

    return bufferOut;
}

char* Int2String(int value, bool alwaysSign, int numMinFields, char buffer[20])
{
    char format[20] = "%";
    sprintf(&(format[1]), "0%d", numMinFields);
    strcat(format, "d");
    if(alwaysSign && value >= 0)
    {
        buffer[0] = '+';
        sprintf(buffer + 1, format, value);
    }
    else
    {
        sprintf(buffer, format, value);
    }
    return buffer;
}

bool String2Int(char *str, int *value)  
{
    int sign = str[0] == '-' ? -1 : 1;
    if (str[0] < '0' || str[0] > '9')
    {
        str++;
    }
    int length = strlen(str);
    if (length == 0)
    {
        return false;
    }
    *value = 0;
    int pow = 1;
    for(int i = length - 1; i >= 0; i--)
    {
        int val = str[i] & (~(0x30));
        if(val < 0 || val > 9)
        {
            return false;
        }
        *value += val * pow;
        pow *= 10;
    }
    if(sign == -1)
    {
        *value *= -1;
    }
    return true;
}

char*    Bin2String(uint8 value, char buffer[9])
{
    for(int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(value, bit) ? '1' : '0';
    }
    return buffer;
}

char*   Bin2String16(uint16 value, char valBuffer[19])
{
    char buffer[9];
    strcpy(valBuffer, Bin2String((uint8)(value >> 8), buffer));
    strcpy((valBuffer[8] = ' ', valBuffer + 9), Bin2String((uint8)value, buffer));
    return valBuffer;
}

char* Hex8toString(uint8 value, char buffer[3])
{
    sprintf(value < 16 ? (buffer[0] = '0', buffer + 1) :  (buffer), "%x", value);
    return buffer;
}

char* Voltage2String(float voltage, bool alwaysSign, char buffer[20])
{
    buffer[0] = 0;
    char *suffix;
    if(voltage == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, ERROR_STRING_VALUE);
        return buffer;
    }
    else if(fabs(voltage) + 0.5e-4 < 1e-3)
    {
        suffix = set.common.lang == Russian ? "\x10ìêÂ" : "\x10uV";
        voltage *= 1e6;
    }
    else if(fabs(voltage) + 0.5e-4 < 1)
    {
        suffix = set.common.lang == Russian ? "\x10ìÂ" : "\x10mV" ;
        voltage *= 1e3;
    }
    else if(fabs(voltage) + 0.5e-4 < 1000)
    {
        suffix = set.common.lang == Russian ? "\x10Â" : "\x10V";
    }
    else
    {
        suffix = set.common.lang == Russian ? "\x10êÂ" : "\x10kV";
        voltage *= 1e-3f;
    }

    char bufferOut[20];

    Float2String(voltage, alwaysSign, 4, bufferOut);
    strcat(buffer, bufferOut);
    strcat(buffer, suffix);
    return buffer;
}

char* Time2String(float time, bool alwaysSign, char buffer[20])
{
    buffer[0] = 0;
    char *suffix = 0;
    if(time == ERROR_VALUE_FLOAT)
    {
        strcat(buffer, ERROR_STRING_VALUE);
        return buffer;
    }
    else if(fabs(time) + 0.5e-10 < 1e-6)
    {
        suffix = set.common.lang == Russian ? "íñ" : "ns";
        time *= 1e9;
    }
    else if(fabs(time) + 0.5e-7 < 1e-3)
    {
        suffix = set.common.lang == Russian ? "ìêñ" : "us";
        time *= 1e6;
    }
    else if(fabs(time) + 0.5e-3 < 1)
    {
        suffix = set.common.lang == Russian ? "ìñ" : "ms";
        time *= 1e3;
    }
    else
    {
        suffix = set.common.lang == Russian ? "ñ" : "s";
    }

    char bufferOut[20];
    strcat(buffer, Float2String(time, alwaysSign, 4, bufferOut));
    strcat(buffer, suffix);
    return buffer;
}

char* Phase2String(float phase, bool empty, char bufferOut[20])
{
    char buffer[20];
    sprintf(bufferOut, "%s\xa8", Float2String(phase, false, 4, buffer));
    return bufferOut;
}

char *  Freq2String(float freq, bool alwaysSign, char bufferOut[20])
{
    bufferOut[0] = 0;
    char *suffix = 0;
    if(freq == ERROR_VALUE_FLOAT)
    {
        strcat(bufferOut, ERROR_STRING_VALUE);
        return bufferOut;
    }
    if(freq >= 1e6)
    {
        suffix = set.common.lang == Russian ? "ÌÃö" : "MHz";
        freq /= 1e6;
    }
    else if (freq >= 1e3)
    {
        suffix = set.common.lang == Russian ? "êÃö" : "kHz";
        freq /= 1e3;
    }
    else
    {
        suffix = set.common.lang == Russian ? "Ãö" : "Hz";
    }
    char buffer[20];
    strcat(bufferOut, Float2String(freq, false, 4, buffer));
    strcat(bufferOut, suffix);
    return bufferOut;
}

char* Float2Db(float value, int numDigits, char bufferOut[20])
{
    bufferOut[0] = 0;
    char buffer[20];
    strcat(bufferOut, Float2String(value, false, numDigits, buffer));
    strcat(bufferOut, "äÁ");
    return bufferOut;
}

bool IntInRange(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}

float MaxFloat(float val1, float val2, float val3)
{
    float retValue = val1;
    if(val2 > retValue)
    {
        retValue = val2;
    }
    if(val3 > retValue)
    {
        retValue = val3;
    }
    return retValue;
}

int8 CircleIncreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int16 CircleIncreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int CircleIncreaseInt(int *val, int min, int max)
{
    (*val)++;
    if((*val) > max)
    {
        (*val) = min;
    }
    return (*val);
}

int8 CircleDecreaseInt8(int8 *val, int8 min, int8 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return *val;
}

int16 CircleDecreaseInt16(int16 *val, int16 min, int16 max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}

int CircleDecreaseInt(int *val, int min, int max)
{
    (*val)--;
    if((*val) < min)
    {
        (*val) = max;
    }
    return (*val);
}

float CircleAddFloat(float *val, float delta, float min, float max)
{
    *val += delta;
    if(*val > max)
    {
        *val = min;
    }
    return *val;
}

float CircleSubFloat(float *val, float delta, float min, float max)
{
    *val -= delta;
    if(*val < min)
    {
        *val = max;
    }
    return *val;
}

void AddLimitationFloat(float *val, float delta, float min, float max)
{
    float sum = *val + delta;
    if(sum < min)
    {
        *val = min;
    }
    else if(sum > max)
    {
        *val = max;
    }
    else
    {
        *val = sum;
    }
}

void SwapInt(int *value0, int *value1)
{
    int temp = *value0;
    *value0 = *value1;
    *value1 = temp;
}

void SortInt(int *value0, int *value1)
{
    if(*value1 < *value0)
    {
        SwapInt(value0,  value1);
    }
}

char GetSymbolForGovernor(int value)
{
    static const char chars[] =
    {
        SYMBOL_GOVERNOR_SHIFT_0,
        SYMBOL_GOVERNOR_SHIFT_1,
        SYMBOL_GOVERNOR_SHIFT_2,
        SYMBOL_GOVERNOR_SHIFT_3
    };
    while(value < 0)
    {
        value += 4;
    }
    return chars[value % 4];
}

void EmptyFuncVV() { }

void EmptyFuncVI(int i) { }

void EmptyFuncVpV(void *empty) { }

void EmptyFuncpVII(void *v, int i, int ii) { }

void EmptyFuncVI16(int16 i) {}

void EmptyFuncVB(bool b) {}

bool EmptyFuncBV()
{
    return true;
}

void IntToStrCat(char *_buffer, int _value)
{
    static const int LENGHT = 10;
    char buffer[LENGHT];
    for(int i = 0; i < LENGHT; i++)
    {
        buffer[i] = 0;
    }
    int pointer = LENGHT -1;

    while(_value > 0)
    {
        buffer[pointer] = (int8)(_value % 10);
        _value /= 10;
        pointer--;
    }

    while(*_buffer)
    {
        _buffer++;
    }
    int i = 0;

    for(; i < LENGHT; i++)
    {
        if(buffer[i] > 0)
        {
            break;
        }
    }

    for(; i < LENGHT; i++)
    {
        *_buffer = 0x30 | buffer[i];
        _buffer++;
    }

    *_buffer = 0;
}
