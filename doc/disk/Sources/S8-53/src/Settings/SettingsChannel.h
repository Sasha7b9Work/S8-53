// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once
#include "SettingsTypes.h"


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsChannel
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define STRETCH_ADC(ch)         (set.chan[ch].stretchADC)       ///< SettingsChannel.stretchADC
#define STRETCH_ADC_A           (STRETCH_ADC(A))                ///< set.chan[A].stretchADC
#define STRETCH_ADC_B           (STRETCH_ADC(B))                ///< set.chan[B].stretchADC

#define RSHIFT(ch)              (set.chan[ch].rShiftRel)        ///< SettingsChannel.rShiftRel
#define RSHIFT_A                (RSHIFT(A))
#define RSHIFT_B                (RSHIFT(B))

#define MODE_COUPLE(ch)         (set.chan[ch].modeCouple)       ///< SettingsChannel.modeCouple
#define MODE_COUPLE_A           (MODE_COUPLE(A))
#define MODE_COUPLE_B           (MODE_COUPLE(B))

#define MULTIPLIER(ch)          (set.chan[ch].multiplier)       ///< SettingsChannel.multiplier
#define MULTIPLIER_A            MULTIPLIER(A)
#define MULTIPLIER_B            MULTIPLIER(B)
#define MULTIPLIER_1_(ch)       (MULTIPLIER(ch) == Multiplier_1)
#define MULTIPLIER_10_(ch)      (MULTIPLIER(ch) == Multiplier_10)
#define VALUE_MULTIPLIER(ch)    (sChannel_MultiplierRel2Abs(MULTIPLIER(ch)))

#define RANGE(ch)               (set.chan[ch].range)            ///< SettingsChannel.range
#define RANGE_A                 (RANGE(A))
#define RANGE_B                 (RANGE(B))

#define ENABLED(ch)             (set.chan[ch].enable)           ///< SettingsChannel.enable
#define ENABLED_A               ENABLED(A)                      ///< set.chan[A].enable
#define ENABLED_B               ENABLED(B)                      ///< set.chan[B].enable

#define INVERSE(ch)             (set.chan[ch].inverse)          ///< SettingsChannel.inverse
#define INVERSE_A               (INVERSE(A))                    ///< set.chan[A].inverse
#define INVERSE_B               (INVERSE(B))                    ///< set.chan[B].inverse

#define FILTR(ch)               (set.chan[ch].filtr)            ///< SettingsChannel.filtr
#define FILTR_A                 (FILTR(A))                      ///< set.chan[A].filtr
#define FILTR_B                 (FILTR(B))                      ///< set.chan[B].filtr

#define BALANCE_SHIFT_ADC(ch)   (set.chan[ch].balanceShiftADC)  ///< SettingsChannel.balanceShiftADC
#define BALANCE_SHIFT_ADC_A     (BALANCE_SHIFT_ADC(A))          ///< set.chan[A].balanceShiftADC
#define BALANCE_SHIFT_ADC_B     (BALANCE_SHIFT_ADC(B))          ///< set.chan[B].balanceShiftADC


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Режим канала по входу.
typedef enum
{
    ModeCouple_DC,      ///< Открытый вход.
    ModeCouple_AC,      ///< Закрытый вход.
    ModeCouple_GND      ///< Вход заземлён.
} ModeCouple;

/// Делитель.
typedef enum
{
    Multiplier_1,
    Multiplier_10
} Multiplier;

/// Масштаб по напряжению.
typedef enum
{
    Range_2mV,
    Range_5mV,
    Range_10mV,
    Range_20mV,
    Range_50mV,
    Range_100mV,
    Range_200mV,
    Range_500mV,
    Range_1V,
    Range_2V,
    Range_5V,
    Range_10V,
    Range_20V,
    RangeSize
} Range;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Настройки каналов
typedef struct
{
    float       stretchADC;                 ///< Поправочный коэффициент.
    int16       rShiftRel;
    int16       rShiftAdd[RangeSize][2];    ///< Добавочное смещение для открытого (0) и закрытого (1) входов.
    ModeCouple  modeCouple;                 ///< Режим по входу.
    Multiplier  multiplier;                 ///< Множитель.
    Range       range;                      ///< Масштаб по напряжению.
    bool        enable;                     ///< Включён ли канал.
    bool        inverse;                    ///< Инвертирован ли канал.
    bool        filtr;                      ///< Фильтр
    int8        balanceShiftADC;            ///< Добавочное смещение для балансировки АЦП.
} SettingsChannel;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Установить масштаб по напряжению канала chan.
void sChannel_SetRange(Channel chan, Range range);

int sChannel_MultiplierRel2Abs(Multiplier multiplier);
/// Возвращает true, если канал chan включён.
bool sChannel_Enabled(Channel chan);

const char *sChannel_Range2String(Range range, Multiplier multiplier);

const char *sChannel_RShift2String(int16 rShiftRel, Range range, Multiplier multiplier, char buffer[20]);

/** @}  @}
 */
