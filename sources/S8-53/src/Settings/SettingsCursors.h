

#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsCursors Settings Cursors
 *  @{
 */

#define CURSORS_SHOW_FREQ           (set.cursors.showFreq)                      ///< SettingsCursors.showFreq
#define CURS_MOVEMENT               (set.cursors.movement)                      ///< SettingsCursors.movement
#define CURS_MOVEMENT_IS_PERCENTS   (CURS_MOVEMENT == CursMovement_Percents)

#define CURS_SOURCE                 (set.cursors.source)                        ///< SettingsCursors.source
#define CURS_SOURCE_A               (CURS_SOURCE == A)

#define CURsU_CNTRL_CH(ch)          (set.cursors.cntrlU[ch])                    ///< SettingsCursors.cntrlU
#define CURsU_CNTRL                 (CURsU_CNTRL_CH(CURS_SOURCE))
#define CURS_CNTRL_U_IS_DISABLE(ch) (CURsU_CNTRL_CH(ch) == CursCntrl_Disable)





#define CURS_CNTRL_T(ch)            (set.cursors.cntrlT[ch])                    ///< SettingsCursors.cntrlT
#define CURS_CNTRL_T_IS_DISABLE(ch) (CURS_CNTRL_T(ch) == CursCntrl_Disable)

#define CURS_POS_U(ch, num)         (set.cursors.posCurU[ch][num])              ///< SettingsCursors.posCurU
#define CURS_POS_U0(ch)             (CURS_POS_U(ch, 0))
#define CURS_POS_U1(ch)             (CURS_POS_U(ch, 1))

#define CURS_POS_T(ch, num)         (set.cursors.posCurT[ch][num])              ///< SettingsCursors.posCurT
#define CURS_POS_T0(ch)             (CURS_POS_T(ch, 0))
#define CURS_POS_T1(ch)             (CURS_POS_T(ch, 1))

#define DELTA_U100(ch)              (set.cursors.deltaU100percents[ch])

#define DELTA_T100(ch)              (set.cursors.deltaT100percents[ch])

#define CURS_ACTIVE                 (set.cursors.active)                        ///< SettingsCursors.active
#define CURS_ACTIVE_IS_T            (CURS_ACTIVE == CursActive_T)
#define CURS_ACTIVE_IS_U            (CURS_ACTIVE == CursActive_U)

#define CURS_LOOKMODE(num)          (set.cursors.lookMode[num])                 ///< SettingsCursors.lookMode
#define CURS_LOOKMODE_0             (CURS_LOOKMODE(0))
#define CURS_LOOKMODE_1             (CURS_LOOKMODE(1))

#define CURS_SHOW                   (set.cursors.showCursors)                   ///< SettingsCursors.showCursors


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Настройки курсорных измерений.
typedef struct
{
    CursCntrl       cntrlU[NumChannels];        ///< Активные курсоры напряжения.
    CursCntrl       cntrlT[NumChannels];        ///< Активные курсоры напряжения.
    Channel         source;                     ///< Источник - к какому каналу относятся курсоры.
    float           posCurU[NumChannels][2];    ///< Текущие позиции курсоров напряжения обоих каналов.
    float           posCurT[NumChannels][2];    ///< Текущие позиции курсоров времени обоих каналов.
    float           deltaU100percents[2];       ///< Расстояние между курсорами напряжения для 100%, для обоих каналов.
    float           deltaT100percents[2];       ///< Расстояние между курсорами времени для 100%, для обоих каналов.
    CursMovement    movement;                   ///< Как перемещаться курсорам - по точкам или по процентам.
    CursActive      active;                     ///< Какие курсоры сейчас активны.
    CursLookMode    lookMode[2];                ///< Режимы слежения за курсорами для двух пар курсоров.
    bool            showFreq;                   ///< Установленное в true значение, что нужно показывать на экране значение 1/dT между курсорами.
    bool            showCursors;                ///< Показывать ли курсоры.
} SettingsCursors;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Получить позицию курсора напряжения.
float sCursors_GetCursPosU(Channel chan, int numCur);
/// Возвращает true,если нужно рисовать курсоры.
bool sCursors_NecessaryDrawCursors();
/// Получить строку курсора напряжения.
const char* sCursors_GetCursVoltage(Channel source, int numCur, char buffer[20]);
/// Получить строку курсора времени.
const char* sCursors_GetCursorTime(Channel source, int numCur, char buffer[20]);
/// Получить строку процентов курсоров напряжения.
const char* sCursors_GetCursorPercentsU(Channel source, char buffer[20]);
/// Получить строку процентов курсоров времени.
const char* sCursors_GetCursorPercentsT(Channel source, char buffer[20]);


/** @}  @}
 */
