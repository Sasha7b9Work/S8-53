

#pragma once
#include "SettingsTypes.h"
#include "defines.h"


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsTime
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_TBASE                           (set.time.tBase)        ///< SettingTime.tBase

#define TSHIFT                              (set.time.tShiftRel)    ///< SettingsTime.tShiftRel

#define TIME_DIV_XPOS                       (set.time.timeDivXPos)  ///< SettingsTime.timeDivXPos
#define TIME_DIV_XPOS_IS_SHIFT_IN_MEMORY    (TIME_DIV_XPOS == FunctionTime_ShiftInMemory)

#define SET_TPOS                            (set.time.tPos)         ///< SettingsTime.tPos
#define SET_TPOS_IS_LEFT                    (SET_TPOS == TPos_Left)
#define SET_TPOS_IS_CENTER                  (SET_TPOS == TPos_Center)
#define SET_TPOS_IS_RIGHT                   (SET_TPOS == TPos_Right)

#define SAMPLE_TYPE                         (set.time.sampleType)   ///< SettingsTime.sampleType
#define SAMPLE_TYPE_IS_EQUAL                (SAMPLE_TYPE == SampleType_Equal)
#define SAMPLE_TYPE_IS_REAL                 (SAMPLE_TYPE == SampleType_Real)

#define PEAKDET                             (set.time.peakDet)      ///< SettingsTime.peakDet
#define PEAKDET_IS_DISABLE                  (PEAKDET == PeackDet_Disable)
#define PEAKDET_IS_ENABLE                   (PEAKDET == PeackDet_Enable)

#define SET_SELFRECORDER                    (set.time.selfRecorder) ///< SettingsTime.selfRecorder


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Функция ручки ВРЕМЯ/ДЕЛ.
typedef enum
{
    FunctionTime_Time,              ///< Ручка управляет смещением по времени.
    FunctionTime_ShiftInMemory      ///< Ручка управляет отображаемым на экране участком памяти.
} FunctionTime;

/// Привязка синхронизации к памяти.
typedef enum
{
    TPos_Left,      ///< Синхронизация привязана к началу памяти.
    TPos_Center,    ///< Синхронизация привязана к центру памяти.
    TPos_Right      ///< Синхронизация привязана к концу памяти.
} TPos;

/// Тип выборки для режима рандомизатора.
typedef enum
{
    SampleType_Real,   ///< реальное время - в построении участвуют только реально считанные точки, ничего не рассчитывается.
    SampleType_Equal   ///< эквивалентная - сигнал строится по последним точкам, полученным от рандомизатора.
} SampleType;

/// Режим работы пикового детектора.
typedef enum
{
    PeackDet_Disable,
    PeackDet_Enable,
    PeackDet_Average
} PeackDetMode;

/// Число точек сигнала, с которым идёт работа.
typedef enum
{
    FNP_281,
    FNP_512,
    FNP_1024
} ENUM_POINTS_FPGA;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Настройки оси X.
typedef struct
{ //-V802
    TBase               tBase;          ///< Масштаб по времени.
    int16               tShiftRel;      ///< Смещение по времени.
    FunctionTime        timeDivXPos;
    TPos                tPos;           ///< Привязка синхронизации к памяти.
    SampleType          sampleType;     ///< Тип выборки для режима рандомизатора.
    PeackDetMode        peakDet;        ///< Режим работы пикового детектора
    bool                selfRecorder;   ///< Включен ли режим самописца.
    ENUM_POINTS_FPGA    oldNumPoints;   ///< \brief Когда переключаемся в режим пикового детектора, устанавливаем количество точек в 1024, а сюда 
                                        /// записываем то, что было, чтобы потом восстановить.
} SettingsTime;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Сохранить масштаб по времени.
void sTime_SetTBase(TBase tBase);
/// Сохранить смещение по времени в относительных единицах.
void sTime_SetTShift(int16 shift);                   
/// Узнать привязку отсительно уровня синхронизации в точках.
int sTime_TPosInPoints(PeackDetMode peakDet, int numPoints, TPos tPos);
/// Смещение по времени в точках экрана. Т.к. на канал у нас работают два АЦП поочерёдно, это значение отличается от засылаемого в аппаратную часть в два раза.
int sTime_TShiftInPoints(PeackDetMode peakDet);
/// Минимальное смещение по времени, которое может быть записано в аппаратную часть.
int16 sTime_TShiftMin();
/// Смещение по времени, соответствующее позиции TPos.
int16 sTime_TShiftZero();
/// Если true, значит, работает рандомизатор.
bool sTime_RandomizeModeEnabled();


/** @}  @}
 */
