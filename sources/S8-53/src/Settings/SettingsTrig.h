

#pragma once
#include "defines.h"
#include "SettingsTypes.h"


/** @addtogroup Settings
 *  @{
 *  @addtogroup SettingsTrig
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define START_MODE              (set.trig.startMode)                    ///< SettingsTrig.startMode
#define START_MODE_IS_SINGLE    (START_MODE == StartMode_Single)        ///< \c true, если однократный режим запуска.
#define START_MODE_IS_AUTO      (START_MODE == StartMode_Auto)          ///< \c true, если автоматический режим запуска.

#define TRIG_SOURCE             (set.trig.source)                       ///< SettingsTrig.source
#define TRIG_SOURCE_IS_EXT      (TRIG_SOURCE == TrigSource_Ext)         ///< \c true, если внешний источник синхронизации.

#define TRIG_POLARITY           (set.trig.polarity)                     ///< SettingsTrig.polarity
#define TRIG_POLARITY_IS_FRONT  (TRIG_POLARITY == TrigPolarity_Front)   ///< \c true, если синхронизация по фронту.

#define TRIG_INPUT              (set.trig.input)                        ///< SettingsTrig.input
#define TRIG_INPUT_IS_AC        (TRIG_INPUT == TrigInput_AC)            ///< \c true, если закрытый вход синхронизации.

#define TRIG_LEVEL(source)      (set.trig.levelRel[source])             ///< SettingsTrig.levelRel
#define TRIG_LEVEL_SOURCE       (TRIG_LEVEL(TRIG_SOURCE))               /// set.trig.levelRel[set.trig.source]
#define TRIG_LEVEL_A            (TRIG_LEVEL(A))                         ///< set.trig.levelRel[A]
#define TRIG_LEVEL_B            (TRIG_LEVEL(B))                         ///< set.trig.levelRel[B]

#define MODE_LONG_PRESS_TRIG    (set.trig.modeLongPressTrig)            ///< SettingsTrig.modeLongPressTrig
/// если \c true, то длительное нажатие кнопки \b СИНХР сбрасывает уровень синхронизации в 0.
#define MODE_LONG_PRESS_TRIG_IS_LEVEL0  (MODE_LONG_PRESS_TRIG == ModeLongPRessTrig_Level0)

#define TRIG_MODE_FIND          (set.trig.modeFind)                     ///< SettingsTrig.modeFind
#define TRIG_MODE_FIND_IS_AUTO  (TRIG_MODE_FIND == TrigModeFind_Auto)   ///< \c true, если автоматическая установка синхронизации.
#define TRIG_MODE_FIND_IS_HAND  (TRIG_MODE_FIND == TrigModeFind_Hand)   ///< \c true, если синхронизация устанавливается вручную.


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Режим запуска.
typedef enum
{
    StartMode_Auto,             ///< Автоматический.
    StartMode_Wait,             ///< Ждущий.
    StartMode_Single            ///< Однократный.
} StartMode;

/// Источник синхронизации.
typedef enum
{
    TrigSource_ChannelA,        ///< Канал 1.
    TrigSource_ChannelB,        ///< Канал 2.
    TrigSource_Ext              ///< Внешняя.
} TrigSource;

/// Тип синхронизацц
typedef enum
{
    TrigPolarity_Front,         ///< Синхронизация по фронту.
    TrigPolarity_Back           ///< Синхронизация по срезу.
} TrigPolarity;

/// Вход синхронизации.
typedef enum
{
    TrigInput_Full,             ///< Полный сиганл.
    TrigInput_AC,               ///< Переменный.
    TrigInput_LPF,              ///< ФНЧ.
    TrigInput_HPF               ///< ФВЧ.
} TrigInput;

/// Режим длительного нажатия кнопки СИНХР.
typedef enum
{
    ModeLongPRessTrig_Level0,   ///< Сброс уровня синхронизации в 0.
    ModeLongPressTrig_Auto      ///< Автоматический поиск синхронизации - уровень устанавливается посередине между максимумом и минимумом.
} ModeLongPressTrig;


/// Режим установки синхронизации.
typedef enum
{
    TrigModeFind_Hand,          ///< Уровень синхронизации устанавливается вручную.
    TrigModeFind_Auto           ///< Подстройки уровня синхронизации производится автоматически после каждого нового считанного сигнала.
} TrigModeFind;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Настройки синхронизации
typedef struct
{
    StartMode           startMode;          ///< Режим запуска.
    TrigSource          source;             ///< Источник.
    TrigPolarity        polarity;           ///< Тип синхронизации.
    TrigInput           input;              ///< Вход синхронизации.
    int16               levelRel[3];        ///< Уровень синхронизации для трёх источников.
    ModeLongPressTrig   modeLongPressTrig;  ///< Режим работы длительного нажатия кнопки СИНХР.
    TrigModeFind        modeFind;           ///< Поиск синхронизации - вручную или автоматически.
} SettingsTrig;


/** @}  @}
 */
