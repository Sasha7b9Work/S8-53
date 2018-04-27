#pragma once
#include "Panel/Controls.h"
#include "Settings/SettingsTypes.h"
#include "Settings/SettingsChannel.h"
#include "Settings/SettingsTime.h"
#include <stm32f2xx_hal_def.h>
#include "stm32f2xx_hal_conf.h"
#include <stm32f2xx_hal_hcd.h>
#include <stm32f2xx_hal_pcd.h>
#include <stm32f2xx_hal_dma.h>
#include <stm32f2xx_hal_spi.h>
#include <stm32f2xx_hal_adc.h>
#include <stm32f2xx_hal_dac.h>
#include <usbd_def.h>
#include <usbh_def.h>


extern USBH_HandleTypeDef handleUSBH;
extern HCD_HandleTypeDef handleHCD;
extern SPI_HandleTypeDef handleSPI;
extern ADC_HandleTypeDef handleADC;
extern DAC_HandleTypeDef handleDAC;


#define CLIENT_LAN_IS_CONNECTED  (gBF.clientLANisConnected)
#define CABLE_LAN_IS_CONNECTED   (gBF.cableLANisConnected)
#define CLIENT_VCP_IS_CONNECTED  (gBF.connectToHost)
#define CABLE_VCP_IS_CONNECTED   (gBF.cableVCPisConnected)


#define SETTINGS_IS_LOADED       (gBF.settingsLoaded)
#define NEED_OPEN_FILE_MANAGER   (gBF.needOpenFileMananger)
#define FLASH_DRIVE_IS_CONNECTED (gBF.flashDriveIsConnected)
#define NEED_CLOSE_PAGE_SB       (gBF.needClosePageSB)
#define AUTO_FIND_IN_PROGRESS    (gBF.FPGAautoFindInProgress)
#define NEED_RESET_SETTINGS      (gBF.needResetSettings)
#define NEED_FINISH_REDRAW       (gBF.needFinishDraw)
#define SHOW_LEVEL_RSHIFT_0      (gBF.showLevelRShift0)
#define SHOW_LEVEL_RSHIFT_1      (gBF.showLevelRShift1)
#define SHOW_LEVEL_TRIGLEV       (gBF.showLevelTrigLev)
#define NUM_DRAWING_SIGNALS      (gBF.numDrawingSignals)
#define TRIG_ENABLE              (gBF.trigEnable)
#define DRAW_RSHIFT_MARKERS      (gBF.drawRShiftMarkers)
#define TOP_MEASURES             (gBF.topMeasures)
#define FRAMES_ELAPSED           (gBF.framesElapsed)
#define FPGA_CAN_READ_DATA       (gBF.FPGAcanReadData)
#define FPGA_CRITICAL_SITUATION  (gBF.FPGAcritiacalSituation)
#define TRIG_AUTO_FIND           (gBF.FPGAtrigAutoFind)
#define FPGA_TEMPORARY_PAUSE     (gBF.FPGAtemporaryPause)
#define FPGA_IN_PROCESS_READ     (gBF.FPGAinProcessingOfRead)
#define FPGA_FIRST_AFTER_WRITE   (gBF.FPGAfirstAfterWrite)
#define SOUND_IS_BEEP            (gBF.soundIsBeep)
#define SOUND_WARN_IS_BEEP       (gBF.soundWarnIsBeep)
#define BUTTON_IS_PRESSED        (gBF.buttonIsPressed)
#define NEED_REDRAW_FILEMANAGER  (gBF.needRedrawFileManager)
#define CURSORS_IN_DIRS          (gBF.cursorInDirs)
#define SHOW_HELP_HINTS          (gBF.showHelpHints)
#define SHOW_DEBUG_MENU          (gBF.showDebugMenu)
#define ADDRESS_GOVERNOR         (gBF.addressGovernor)
#define IN_MOVE_INCREASE         (gBF.inMoveIncrease)
#define IN_MOVE_DECREASE         (gBF.inMoveDecrease)
#define TIME_START_MS            (gBF.timeStartMS)
#define PANEL_IS_RUNNING         (gBF.panelIsRunning)


typedef struct
{
    // Ethernet
    uint ethTimeLastEthifInput      : 32;   // Время последнего входа в процедуру ethernetif.c:ethernetif_input() Используется для определения того, 
                                            // что разъём ethernet подключен. Анализироваться будет в функции отрисовки значка подключения
    uint cableLANisConnected        : 1;    // Если 1, значит, просто подключён кабель
    uint clientLANisConnected       : 1;    // Если 1, то подсоединён клиент

    // Для рисования : Display::c
    uint showLevelRShift0           : 1;    // Нужно ли рисовать горизонтальную линию уровня смещения первого канала
    uint showLevelRShift1           : 1;
    uint showLevelTrigLev           : 1;    // Нужно ли рисовать горизонтальную линию уровня смещения уровня синхронизации
    uint trigEnable                 : 1;
    uint drawRShiftMarkers          : 1;
    uint needFinishDraw             : 1;    // Если 1, то дисплей нуждается в перерисовке
    uint framesElapsed              : 1;
    uint numDrawingSignals          : 8;    // Число нарисованных сигналов для режима накопления

    // FPGA
    uint FPGAtrigAutoFind           : 1;    // Установленное в 1 значение означает, что нужно производить автоматический поиск синхронизации, если 
                                            // выбрана соответствующая настройка.
    uint FPGAautoFindInProgress     : 1;
    uint FPGAtemporaryPause         : 1;
    uint FPGAinProcessingOfRead     : 1;
    uint FPGAcanReadData            : 1;
    uint FPGAcritiacalSituation     : 1;
    uint FPGAfirstAfterWrite        : 1;    // Используется в режиме рандомизатора. После записи любого параметра в альтеру
                                            // нужно не использовать первое считанное данное с АЦП, потому что оно завышено и портит ворота

    // Звук
    uint soundIsBeep                : 1;
    uint soundWarnIsBeep            : 1;
    uint buttonIsPressed            : 1;    // Когда запускается звук нажатой кнопки, устанавливается этот флаг, чтобы знать, проигрывать ли знак 
                                            // отпускания

    // Панель
    uint panelIsRunning             : 1;
    
    // Governor
    uint inMoveIncrease             : 1;
    uint inMoveDecrease             : 1;
    uint addressGovernor            : 32;
    uint timeStartMS                : 32;

    // VCP
    uint cableVCPisConnected        : 1;
    uint connectToHost              : 1;

    // Flash drive
    uint flashDriveIsConnected      : 1;
    uint cursorInDirs               : 1;    // Если 1, то ручка УСТАНОВКА перемещает по каталогам
    uint needRedrawFileManager      : 2;    // Если 1, то файл-менеджер нуждается в полной перерисовке
                                            // Если 2, то перерисовать только каталоги
                                            // Если 3, то перерисовать только файлы

    uint settingsLoaded             : 1;    // Эта переменная нужна для того, чтобы исключить ложную запись пустых настроек из-за неправильного 
                                            // включения прибора (при исключённом из схемы программном включении иногда сигнал от кнопки отключения 
                                            // питания приходит быстрее, чем программа успевает настроить настройки).
    uint showHelpHints              : 1;    // Если 1, то при нажатии кнопки вместо выполнения её фунции выводится подсказка о её назначении

    uint showDebugMenu              : 1;
    
    int topMeasures                 : 9;    // Верх таблицы вывода измерений. Это значение нужно для нормального вывода сообщений на экран - чтобы 
                                            // они ничего не перекрывали
    uint needOpenFileMananger       : 1;    // Если 1, то нужно открыть файловый менеджер (сработало автоподключение)
    uint needClosePageSB            : 1;    // Если 1, нужно закрывать страницу малых кнопок
    uint needResetSettings          : 1;    // Если 1, то необходим сброс настроек
} BitField;


extern BitField gBF;

#define CURRENT_NUM_LATEST_SIGNAL       (gMemory.currentNumLatestSignal)
#define CURRENT_NUM_INT_SIGNAL          (gMemory.currentNumIntSignal)
#define EXIT_FROM_INT_TO_LAST           (gMemory.exitFromIntToLast)
#define RUN_FPGA_AFTER_SMALL_BUTTONS    (gMemory.runningFPGAbeforeSmallButtons)
#define INT_SHOW_ALWAYS                 (gMemory.showAlways)
#define NEED_SAVE_TO_DRIVE              (gMemory.needForSaveToFlashDrive)

#define EXIT_FROM_SET_NAME_TO           (gMemory.exitFromModeSetNameTo)
#define RETURN_TO_MAIN_MENU             0U
#define RETURN_TO_LAST_MEM              1U
#define RETURN_TO_INT_MEM               2U
#define RETURN_TO_DISABLE_MENU          3U
#define EXIT_FROM_SET_NAME_TO_MAIN_MENU (EXIT_FROM_SET_NAME_TO == RETURN_TO_MAIN_MENU)
#define EXIT_FROM_SET_NAME_TO_LAST      (EXIT_FROM_SET_NAME_TO == RETURN_TO_LAST_MEM)
#define EXIT_FROM_SET_NAME_TO_INT       (EXIT_FROM_SET_NAME_TO == RETURN_TO_INT_MEM)
#define EXIT_FROM_SET_NAME_TO_DIS_MENU  (EXIT_FROM_SET_NAME_TO == RETURN_TO_DISABLE_MENU)

typedef struct 
{
    int16   currentNumLatestSignal;                 // Текущий номер последнего сигнала в режиме ПАМЯТЬ - Последние
    int8    currentNumIntSignal;                    // Текущий номер сигнала, сохранённого в ППЗУ
    uint    showAlways                      : 1;    // Если 1, то показывать всегда выбранный в режиме "Внутр. ЗУ" сигнал
    uint    runningFPGAbeforeSmallButtons   : 1;    // Здесь сохраняется информация о том, работала ли ПЛИС перед переходом в режим работы с памятью
    uint    exitFromIntToLast               : 1;    // Если 1, то выходить из страницы внутренней памяти нужно не стандартно, а в меню последних
    uint    exitFromModeSetNameTo           : 2;    // Куда возвращаться из окна установки имени при сохранении : 0 - в основное меню, 1 - в окно 
                                                    // последних, 2 - в окно Внутр ЗУ, 3 - в основно окно в выключенным меню
    uint    needForSaveToFlashDrive         : 1;    // Если 1, то нужно сохранить после отрисовки на флешку.
} GMemory;


extern GMemory gMemory;


typedef enum
{
    StateCalibration_None,
    StateCalibration_ADCinProgress,
    StateCalibration_RShift0start,
    StateCalibration_RShift0inProgress,
    StateCalibration_RShift1start,
    StateCalibration_RShift1inProgress,
    StateCalibration_ErrorCalibration0,
    StateCalibration_ErrorCalibration1
} StateCalibration;

typedef enum
{
    StateWorkFPGA_Stop,                             // СТОП - не занимается считыванием информации.
    StateWorkFPGA_Wait,                             // Ждёт поступления синхроимпульса.
    StateWorkFPGA_Work,                             // Идёт работа.
    StateWorkFPGA_Pause                             // Это состояние, когда временно приостановлен прибор, например, для чтения данных или для 
                                                    // записи значений регистров.
} StateWorkFPGA;


typedef struct
{
    bool needCalibration;				        // Установленное в true значение означает, что необходимо произвести калибровку.
    StateWorkFPGA stateWorkBeforeCalibration;
    StateCalibration stateCalibration;          // Текущее состояние калибровки. Используется в процессе калибровки.
} StateFPGA;

typedef struct
{
    uint hours : 5;
    uint minutes : 6;
    uint seconds : 6;
    uint year : 7;
    uint month : 4;
    uint day : 5;
} PackedTime;

typedef struct
{
    void*       addrNext;               // Адрес следующей записи.
    void*       addrPrev;               // Адрес предыдущей записи.
    uint        rShiftCh0       : 10;   // Смещение по напряжению
    uint        rShiftCh1       : 10;
    uint        trigLevCh0      : 10;   // Уровень синхронизации
    int16       tShift;                 // Смещение по времени
    ModeCouple  modeCouple1     : 2;
    Range       range[2];               // Масштаб по напряжению обоих каналов.

    uint        trigLevCh1      : 10;
    uint        length1channel  : 11;   // Сколько занимает в байтах длина измерения одного канала
    TBase       tBase           : 5;    // Масштаб по времени
    ModeCouple  modeCouple0     : 2;    // Режим канала по входу
    uint        peakDet         : 2;    // Включен ли пиковый детектор
    uint        enableCh0       : 1;    // Включён ли канал 0
    uint        enableCh1       : 1;    // Включен ли канал 1

    uint        inverseCh0      : 1;
    uint        inverseCh1      : 1;
    Divider  multiplier0     : 1;
    Divider  multiplier1     : 1;
    PackedTime  time;
} DataSettings;


extern const char *gStringForHint;  // Строка подсказки, которую надо выводить в случае включённого режима подсказок.
extern void* gItemHint;              // Item, подсказку для которого нужно выводить в случае включённого режима подсказок.

void SetItemForHint(void *item);

extern StateFPGA gStateFPGA;

extern uint8        *gData0;    // Указатель на данные первого канала, который надо рисовать на экране
extern uint8        *gData1;    // Указатель на данные второго канала, который надо рисовать на экране
extern DataSettings *gDSet;     // Указатель на параметры рисуемых сигналов

extern DataSettings *gDSmemInt;       //--
extern uint8        *gData0memInt;    // | Здесь данные из ППЗУ, которые должны выводиться на экран
extern uint8        *gData1memInt;    //-/

extern DataSettings *gDSmemLast;
extern uint8        *gData0memLast;
extern uint8        *gData1memLast;

extern void *extraMEM;      // Это специальный указатель. Используется для выделения памяти переменным, которые не нужны всё время выполения программы,
                            // но нужны болеее чем в одной функции. Перед использованием с помощью вызова malloc() выделяется необходимое количество
                            // памяти, которая затем освобождается вызвом free()

#define MALLOC_EXTRAMEM(NameStruct, name)   extraMEM = malloc(sizeof(NameStruct));    \
                                            NameStruct *name = (NameStruct*)extraMEM
#define ACCESS_EXTRAMEM(NameStruct, name)   NameStruct *name = (NameStruct*)extraMEM
#define FREE_EXTRAMEM()                     free(extraMEM)

extern int transmitBytes;
