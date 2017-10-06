#pragma once
#include "Settings/SettingsDisplay.h"


typedef enum
{
    LimitChan1_Volts,
    LimitChan2_Volts,
    LimitSweep_Time,
    EnabledPeakDet,
    LimitChan1_RShift,
    LimitChan2_RShift,
    LimitSweep_Level,
    LimitSweep_TShift,
    TooSmallSweepForPeakDet,
    TooFastScanForSelfRecorder,
    FileIsSaved,
    SignalIsSaved,
    SignalIsDeleted,
    MenuDebugEnabled,
    MovingData,
    TimeNotSet,
    SignalNotFound,
    SetTPosToLeft1,
    SetTPosToLeft2,
    SetTPosToLeft3,
    VerySmallMeasures,
    NeedRebootDevice1,
    NeedRebootDevice2,
    ImpossibleEnableMathFunction,
    ImpossibleEnableFFT,
    FirmwareSaved,
    FullyCompletedOTP,
    Warning_NumWarnings
} Warning;

typedef enum
{
    ExcessValues        // Превышение значения количества сигналов в "НАКОПЛЕНИЕ", "УСРЕДНЕНИЕ", "Мин Макс"
} WarningWithNumber;


ENumSignalsInSec Tables_NumSignalsInSecToENUM(int enumSignalsInSec);

int Tables_ENUMtoNumSignalsInSec(ENumSignalsInSec enumSignalsInSec);

const char* Tables_GetWarning(Warning warning);

const char* Tables_GetTBaseString(TBase tBase);

const char* Tables_GetTBaseStringEN(TBase tBase);

const char* Tables_RangeNameFromValue(Range range);
/// Возвращает номер канала от 1 до 2
int Tables_GetNumChannel(Channel chan);

extern const char* symbolsAlphaBet[0x48];
/// Выводит строку из таблицы symbolsAlphaBet
void DrawStr(int index, int x, int y);
