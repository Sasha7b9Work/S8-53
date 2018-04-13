#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Settings
 *  @{
 *  @defgroup SettingsService
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SOUND_ENABLED       (set.service.soundEnabled)          ///< SettingsService.soundEnabled

#define CALIBRATOR          (set.service.calibrator)            ///< SettingsService.calibrator
#define CALIBRATOR_IS_FREQ  (CALIBRATOR == Calibrator_Freq)
#define CALIBRATOR_IS_DC    (CALIBRATOR == Calibrator_DC)

#define IP_ADDRESS          (set.service.IPaddress)             ///< SettingsService.IPaddress

#define COLOR_SCHEME                    (set.service.colorScheme)
#define IS_COLOR_SCHEME_WHITE_LETTERS   (COLOR_SCHEME == ColorScheme_WhiteLetters)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Режим работы калибратора.
typedef enum
{
    Calibrator_Freq,            ///< На выходе калибратора 4В, 1кГц.
    Calibrator_DC,              ///< На выходе калибратора 4В постоянного напряжения.
    Calibrator_GND              ///< На выходе калибратора 0В.
} CalibratorMode;

/// Цветовая схема
typedef enum
{
    ColorScheme_WhiteLetters,   ///< В этом случае заголовки элементов меню пишутся белым - не очень хорошо видно снизу.
    ColorScheme_BlackLetters    ///< В этом случае заголовки элементов меню пишутся чёрным - не очень красиво выглядит.
} ColorScheme;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Настройки меню СЕРВИС.
typedef struct
{ //-V802
    bool            screenWelcomeEnable;    ///< Будет ли показываться экран приглашения при включении прибора. \todo убрать этот артефакт.
    bool            soundEnabled;           ///< Включены ли звуки.
    CalibratorMode  calibrator;             ///< Режим работы калибратора.
    int8            IPaddress;              ///< IP-адрес (временно).
    ColorScheme     colorScheme;            ///< Цветовая схеама.
} SettingsService;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sService_MathEnabled();

float sService_MathGetFFTmaxDBabs();

/** @}  @}
 */
