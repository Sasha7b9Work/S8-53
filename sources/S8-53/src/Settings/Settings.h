#pragma once
#include "SettingsTypes.h"
#include "Utils/Measures.h"
#include "Menu/MenuItems.h"
#include "Panel/Controls.h"
#include "SettingsChannel.h"
#include "SettingsDisplay.h"
#include "SettingsMemory.h"
#include "SettingsTime.h"
#include "SettingsTrig.h"
#include "SettingsService.h"
#include "SettingsCursors.h"
#include "SettingsDebug.h"
#include "SettingsMemory.h"
#include "SettingsMeasures.h"
#include "SettingsMath.h"


/** @defgroup Settings
 *  @{
 */


#define RSHIFT_ADD(ch, range, closed) set.chan[ch].rShiftAdd[range][closed]


 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /// Настройки каналов
typedef struct
{
    float       stretchADC;                 ///< Поправочный коэффициент.
    int16       rShiftRel;
    int16       rShiftAdd[RangeSize][2];    ///< Добавочное смещение для открытого (0) и закрытого (1) входов.
    ModeCouple  modeCouple;                 ///< Режим по входу.
    Divider     divider;                    ///< Множитель.
    Range       range;                      ///< Масштаб по напряжению.
    bool        enable;                     ///< Включён ли канал.
    bool        inverse;                    ///< Инвертирован ли канал.
    bool        filtr;                      ///< Фильтр
    int8        balanceShiftADC;            ///< Добавочное смещение для балансировки АЦП.
} SettingsChannel;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAC_ADDR0       (set.eth.mac0)
#define MAC_ADDR1       (set.eth.mac1)
#define MAC_ADDR2       (set.eth.mac2)
#define MAC_ADDR3       (set.eth.mac3)
#define MAC_ADDR4       (set.eth.mac4)
#define MAC_ADDR5       (set.eth.mac5)

#define IP_ADDR0        (set.eth.ip0)
#define IP_ADDR1        (set.eth.ip1)
#define IP_ADDR2        (set.eth.ip2)
#define IP_ADDR3        (set.eth.ip3)

#define PORT            (set.eth.port)

#define NETMASK_ADDR0   (set.eth.mask0)
#define NETMASK_ADDR1   (set.eth.mask1)
#define NETMASK_ADDR2   (set.eth.mask2)
#define NETMASK_ADDR3   (set.eth.mask3)

#define GW_ADDR0        (set.eth.gw0)
#define GW_ADDR1        (set.eth.gw1)
#define GW_ADDR2        (set.eth.gw2)
#define GW_ADDR3        (set.eth.gw3)

#define ETH_ENABLE      (set.eth.enable)


typedef struct
{
    uint8 mac0;
    uint8 mac1;
    uint8 mac2;
    uint8 mac3;
    uint8 mac4;
    uint8 mac5;

    uint8 ip0;
    uint8 ip1;
    uint8 ip2;
    uint8 ip3;

    uint16 port;

    uint8 mask0;
    uint8 mask1;
    uint8 mask2;
    uint8 mask3;

    uint8 gw0;
    uint8 gw1;
    uint8 gw2;
    uint8 gw3;

    bool enable;
} SettingsEthernet;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LANG            (set.common.lang)       ///< SettingsCommon.lang
#define LANG_RU         (LANG == Russian)
#define LANG_EN         (LANG == English)
                                    
typedef struct
{
    int     countEnables;               ///< Количество включений. Увеличивается при каждом включении.
    int     countErasedFlashData;       ///< Сколько раз стирался первый сектор с ресурсами.
    int     countErasedFlashSettings;   ///< Сколько раз стирался сектор с настройкаи.
    int     workingTimeInSecs;          ///< Время работы в секундах.
    Language lang;                      ///< Язык меню.
} SettingsCommon;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    bool    all;            ///< Показывать значения всех регистров.
    bool    flag;           ///< Выводить ли флаг готовности.
    bool    rShiftA;
    bool    rShiftB;
    bool    trigLev;
    bool    range[2];
    bool    chanParam[2];
    bool    trigParam;
    bool    tShift;
    bool    tBase;
} OutputRegisters;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_STRINGS                         (set.debug.numStrings)

#define SIZE_FONT_CONSOLE                   (set.debug.sizeFont)

#define CONSOLE_IN_PAUSE                    (set.debug.consoleInPause)

#define BALANCE_ADC_TYPE                    (set.debug.balanceADCtype)
#define BALANCE_ADC_TYPE_IS_HAND            (BALANCE_ADC_TYPE == BalanceADC_Hand)

#define BALANCE_ADC(ch)                     (set.debug.balanceADC[ch])
#define BALANCE_ADC_A                       (BALANCE_ADC(A))
#define BALANCE_ADC_B                       (BALANCE_ADC(B))

#define DEBUG_STRETCH_ADC_TYPE              (set.debug.stretchADCtype)
#define DEBUG_STRETCH_ADC_TYPE_IS_DISABLED  (DEBUG_STRETCH_ADC_TYPE == StretchADC_Disable)
#define DEBUG_STRETCH_ADC_TYPE_IS_HAND      (DEBUG_STRETCH_ADC_TYPE == StretchADC_Hand)
#define DEBUG_STRETCH_ADC_TYPE_IS_SETTINGS  (DEBUG_STRETCH_ADC_TYPE == StretchADC_Settings)

#define DEBUG_STRETCH_ADC(ch)               (set.debug.stretchADC[ch])
#define DEBUG_STRETCH_ADC_A                 (DEBUG_STRETCH_ADC(A))
#define DEBUG_STRETCH_ADC_B                 (DEBUG_STRETCH_ADC(B))

#define NUM_MEAS_FOR_GATES                  (set.debug.numMeasuresForGates)

#define ADD_SHIFT_T0                        (set.debug.shiftT0)

#define SHOW_STATS                          (set.debug.showStats)

#define NUM_AVE_FOR_RAND                    (set.debug.numAveForRand)


/// Отладочные настройки.
typedef  struct
{
    int16           numStrings;                 ///< Число строк в консоли.
    int8            sizeFont;                   ///< Размер шрифта консоли - 0 - 5, 1 - 8,
    bool            consoleInPause;             ///< \brief Признак того, что консоль находится в режиме паузы. Режим паузы означает, что новые 
                                                    /// сообщения она не записывает и не сохраняет.
    BalanceADCtype  balanceADCtype;             ///< Тип балансировки.
    int16           balanceADC[2];              ///< Значение дополнительного смещения АЦП для ручной балансировки.
    StretchADCtype  stretchADCtype;             ///< Тип растяжки канала.
    int16           stretchADC[2];              ///< Значение растяжки канала для ручного режима.
    //RShiftADCtype   rShiftADCtype;
    //int16           rShiftADC[RangeSize][2];    // Дополнительное смещение для каналов в режиме ручного управления. 0 - range == Range_2mV, 1 - все остальные
    int16           numMeasuresForGates;        ///< Число измерений для ворот.
    int16           shiftT0;                    ///< Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    bool            showStats;                  ///< Показывать статистику на экране (fps, например).
    int16           numAveForRand;              ///< По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool            viewAlteraWrittingData;     ///< Показывать ли данные, идущие в альтеру.
    bool            viewAllAlteraWrittingData;  ///< \brief Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде 
                                                    /// START, STOP не показываются).
    int16           altShift;                   ///< Добавочное смещение для устранения эффекта горизонтальной помехи синхронизации.
    OutputRegisters showRegisters;
} SettingsDebug;

typedef struct
{
    int8        posActItem[Page_NumPages];      ///< Позиция ативного пункта. bit7 == 1 - item is opened, 0x7f - нет активного пункта
    int8        currentSubPage[Page_NumPages];  ///< Номер текущей подстраницы.
    bool        pageDebugActive;                ///< Активна ли кнопка отладки в меню.
    unsigned    isShown : 1;                    ///< Меню показано.
} SettingsMenu;



int8 MenuPosActItem(NamePage namePage);                         ///< Возвращает позицию активного пункта на странице namePage.
void SetMenuPosActItem(NamePage namePage, int8 pos);            ///< Установить позицию активного пункта на странице namePage.
int8 MenuCurrentSubPage(NamePage namePage);                     ///< Возвращает номер текущей подстраницы страницы namePage.
void SetMenuCurrentSubPage(NamePage namePage, int8 posSubPage); ///< Устанавливает номер текущей подстраницы в странице namePage.
bool MenuIsShown();                                             ///< Отображено ли меню на экране.
void ShowMenu(bool show);                                       ///< Отобразить/скрыть меню.
bool MenuIsMinimize();                                          ///< Если true - меню находится в дополнительном режиме.
bool MenuPageDebugIsActive();                                   ///< Активна ли страница отладки.
void SetMenuPageDebugActive(bool active);                       ///< Сделать/разделать активной страницу отладки.
void CurrentPageSBregSet(int angle);                            ///< Повернуть ручку УСТАНОВКА на текущей странице малых кнопок.
const SmallButton*  GetSmallButton(PanelButton button);         ///< Вернуть указатель на малую кнопку, соответствующую данной кнопки панели.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Струкура хранит все настройки прибора.
typedef struct
{
    uint                size;
    SettingsDisplay     display;            ///< настройки изображения          (меню ДИСПЛЕЙ).
    SettingsChannel     chan[NumChannels];  ///< настройки каналов              (меню КАНАЛ 1 и КАНАЛ 2).
    SettingsTrig        trig;               ///< настройки синхронизации        (меню СИНХР).
    SettingsTime        time;               ///< временнЫе настройки            (меню РАЗВЁРТКА).
    SettingsCursors     cursors;            ///< настройки курсорных измерений  (меню КУРСОРЫ).
    SettingsMemory      memory;             ///< настройки режимов памяти       (меню ПАМЯТЬ).
    SettingsMeasures    measures;           ///< настройки измерений            (меню ИЗМЕРЕНИЯ).
    SettingsMath        math;               ///< настройки режима математических измерений.
    SettingsService     service;            ///< дополнительные настройки       (меню СЕРВИС).
    SettingsEthernet    eth;                ///< настройки для соединения по локальной сети.
    SettingsCommon      common;             ///< системные настройки.
    SettingsMenu        menu;               ///< состояние меню.
    SettingsDebug       debug;              ///< настройки режима отладки       (меню ОТЛАДКА).
    uint                crc32;              ///< контрольная сумма. Используется для проверки корректности сохранённых настроек
    //int temp[5];
    static void Load(bool _default);  ///< \brief Загрузить настройки. Если _default == true, загружаются настройки по умолчанию, иначе пытается 
                                      ///< загрузить настройки из ПЗУ, а в случае неудачи - тоже настройки по умолчанию.
    static void Save();               ///< Сохранить настройки во флеш-память.
    static bool DebugModeEnable();    ///< Возвращает true, если включён режим отладки.
} Settings;

extern Settings set;


/** @}
 */
