#pragma once


#include "defines.h"
#include "SettingsTypes.h"
#include "Panel/Controls.h"


// Настройки изображения
typedef struct
{
    int16               timeShowLevels;             // Время, в течение которого нужно показывать уровни смещения
    int16               shiftInMemory;              // Показывает смещение левого края сетки относительно нулевого байта памяти. Нужно для правильного отображения сигнала в окне.
    int16               timeMessages;               // Время в секундах, на которое сообщение остаётся на экране
    int16               brightness;                 // Яркость дисплея (только для цветного)
    int16               brightnessGrid;             // Яркость сетки от 0 до 100
    uint16              colors[16];
    ModeDrawSignal      modeDrawSignal;             // Режим отрисовки сигнала
    TypeGrid            typeGrid;                   // Тип сетки
    NumAccumulation     numAccumulation;            // Число накоплений сигнала на экране
    NumAveraging        numAve;                     // Число усреднений сигнала
    ModeAveraging       modeAve;                    // Тип усреднений по измерениям
    NumMinMax           numMinMax;                  // Число измерений для определения минимумов и максимумов
    DisplaySmoothing    smoothing;                  // Число точек для скользящего фильтра
    NumSignalsInSec     numSignalsInSec;            // Число считываний сигнала в секунду
    Channel             lastAffectedChannel;        // Здесь хранится номер последнего канала, которым управляли ручками. Нужно для того, чтобы знать, какой сигнал рисовать наверху.
    ModeAccumulation    modeAccumulation;           // Задаёт режим накопления сигналов
    AltMarkers          altMarkers;                 // Режим отображения дополнительных боковых маркеров смещений
    MenuAutoHide        menuAutoHide;               // Через сколько времени после последнего нажатия клавиши прятать меню 
    bool                showFullMemoryWindow;       // Показывать ли окно памяти вверху экрана.
    bool                showStringNavigation;       // Показывать ли строку текущего состояния меню.
} SettingsDisplay;


// Настройки каналов
typedef struct
{
    float           stretchADC;         	    // Поправочный коэффициент.
    int16           rShiftRel;
    int16           rShiftAdd[RangeSize][2];    // Добавочное смещение для открытого (0) и закрытого (1) входов.
    ModeCouple      modeCouple;         	    // Режим по входу.
    Multiplier      multiplier;         	    // Множитель.
    Range           range;              	    // Масштаб по напряжению.
    bool            enable;             	    // Включён ли канал.
    bool            inverse;            	    // Инвертирован ли канал.
    bool            filtr;              	    // Фильтр
    int8            balanceShiftADC;    	    // Добавочное смещение для балансировки АЦП.
} SettingsChannel;


// Настройки синхронизации
typedef struct
{
    StartMode           startMode;          // Режим запуска.
    TrigSource          source;             // Источник.
    TrigPolarity        polarity;           // Тип синхронизации.
    TrigInput           input;              // Вход синхронизации.
    int16               levelRel[3];        // Уровень синхронизации для трёх источников.
    ModeLongPressTrig   modeLongPressTrig;  // Режим работы длительного нажатия кнопки СИНХР.
    TrigModeFind        modeFind;           // Поиск синхронизации - вручную или автоматически.
} SettingsTrig;


// ВременнЫе настройки
typedef struct
{
    TBase           tBase;                  // Масштаб по времени.
    int16           tShiftRel;              // Смещение по времени
    FunctionTime    timeDivXPos;
    TPos            tPos;
    SampleType      sampleType;
    PeackDetMode    peakDet;
    bool            selfRecorder;           // Включен ли режим самописца.
    FPGA_NUM_POINTS oldNumPoints;           // Когда переключаемся в режим пикового детектора, устанавливаем количество точек в 1024, а сюда записываем то, что было, чтобы потом восстановить
} SettingsTime;


// Настройки курсоров
typedef struct
{
    CursCntrl           cntrlU[NumChannels];                    // Активные курсоры напряжения.
    CursCntrl           cntrlT[NumChannels];                    // Активные курсоры напряжения.
    Channel             source;                                 // Источник - к какому каналу относятся курсоры.
    float               posCurU[NumChannels][2];                // Текущие позиции курсоров напряжения обоих каналов.
    float               posCurT[NumChannels][2];                // Текущие позиции курсоров времени обоих каналов.
    float               deltaU100percents[2];                   // Расстояние между курсорами напряжения для 100%, для обоих каналов.
    float               deltaT100percents[2];                   // Расстояние между курсорами времени для 100%, для обоих каналов.
    CursMovement        movement;                               // Как перемещаться курсорам - по точкам или по процентам.
    CursActive          active;                                 // Какие курсоры сейчас активны.
    CursLookMode        lookMode[2];                            // Режимы слежения за курсорами для двух пар курсоров.
    bool                showFreq;                               // Установленное в true значение, что нужно показывать на экране значение 1/dT между курсорами.
    bool                showCursors;                            // Показывать ли курсоры
} SettingsCursors;

typedef  struct
{
    bool isActiveModeSelect;
} StructMemoryLast;

// Настройки МЕНЮ->ПАМЯТЬ
typedef struct
{
#define MAX_SYMBOLS_IN_FILE_NAME 35
    FPGA_NUM_POINTS fpgaNumPoints;              // Число точек.
    ModeWork        modeWork;                   // Режим работы.
    FileNamingMode  fileNamingMode;             // Режим именования файлов.
    char            fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; // Здесь маска для автоматического именования файлов\n
         // Правила именования.\n
         // %y('\x42') - год, %m('\x43') - месяц, %d('\x44') - день, %H('\x45') - часы, %M('\x46') - минуты, %S('\x47') - секунды\n
         // %Nn('\x48''n') - порядковый номер, котрый занимает не менее n знакомест, например, 7 в %3N будет преобразовано в 007\n
         // Примеры\n
         // name_%4N_%y_%m_%d_%H_%M_%S будет генерировать файлы вида name_0043_2014_04_25_14_45_32\n
         // При этом обратите внимание, что если спецификатор %4N стоИт после временнЫх параметров, то, скорее всего, этот параметр будет всегда равен 0001, т.к. для определения номера просматриваются только символы ДО него.
    char            fileName[MAX_SYMBOLS_IN_FILE_NAME];     // Имя файла для режима ручного задания
    int8                indexCurSymbolNameMask; // Индекс текущего символа в режиме задания маски или выбора имени.
    StructMemoryLast    strMemoryLast;
    ModeShowIntMem      modeShowIntMem;         // Какие сигналы показывать в режиме внутреннего ЗУ
    bool                flashAutoConnect;       // Если true, при подлючении флеш автоматически выводится NC (Нортон Коммандер)
    ModeBtnMemory       modeBtnMemory;     
    ModeSaveSignal      modeSaveSignal;         // В каком виде сохранять сигнал.
} SettingsMemory;
                                    
typedef struct
{
    ScaleFFT        scaleFFT;
    SourceFFT       sourceFFT;
    WindowFFT       windowFFT;
    FFTmaxDB        fftMaxDB;
    Function        func;
    uint8           currentCursor;          // Определяет, каким курсором спектра управляет ручка УСТАНОВКА
    uint8           posCur[2];              // Позиция курсора спектра. Изменятеся 0...256.
    int8            koeff1add;              // Коэффициент при первом слагаемом для сложения.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;
    ModeDrawMath    modeDraw;               // Раздельный или общий дисплей в режиме математической функции
    ModeRegSet      modeRegSet;             // Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали
    Range           range;
    Multiplier      multiplier;
    int16           rShift;
} SettingsMath;

// Эти настройки меняются через МЕНЮ -> СЕРВИС
typedef struct
{
    bool            screenWelcomeEnable;    // Будет ли показываться экран приглашения при включении прибора.
    bool            soundEnable;            // Включены ли звуки.
    CalibratorMode  calibrator;             // Режим работы калибратора.
    int8            IPaddress;              // IP-адрес (временно)
    ColorScheme     colorScheme;            //
} SettingsService;

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
                                    
typedef struct
{
    int     countEnables;               // Количество включений. Увеличивается при каждом включении
    int     countErasedFlashData;       // Сколько раз стирался первый сектор с ресурсами
    int     countErasedFlashSettings;   // Сколько раз стирался сектор с настройкаи
    int     workingTimeInSecs;          // Время работы в секундах
    Language lang;                      // Язык меню
} SettingsCommon;

typedef struct
{
    bool    all;                                // Показывать значения всех регистров.
    bool    flag;                               // Выводить ли флаг готовности.
    bool    rShift0;
    bool    rShift1;
    bool    trigLev;
    bool    range[2];
    bool    chanParam[2];
    bool    trigParam;
    bool    tShift;
    bool    tBase;
} OutputRegisters;


// Настройки отладчика
typedef  struct
{
    int16           numStrings;                 // Число строк в консоли.
    int8            sizeFont;                   // Размер шрифта консоли - 0 - 5, 1 - 8,
    bool            consoleInPause;             // Признак того, что консоль находится в режиме паузы. Режим паузы означает, что новые сообщения она не записывает и не сохраняет.
    BalanceADCtype  balanceADCtype;             // Тип балансировки.
    int16           balanceADC[2];              // Значение дополнительного смещения АЦП для ручной балансировки.
    StretchADCtype  stretchADCtype;             // Тип растяжки канала.
    int16           stretchADC[2];              // Значение растяжки канала для ручного режима.
    //RShiftADCtype   rShiftADCtype;
    //int16           rShiftADC[RangeSize][2];    // Дополнительное смещение для каналов в режиме ручного управления. 0 - range == Range_2mV, 1 - все остальные
    int16           numMeasuresForGates;        // Число измерений для ворот.
    int16           shiftT0;                    // Дополнительное смещение по времени для данной развёртки режима рандомизатора.
    bool            showStats;                  // Показывать статистику на экране (fps, например).
    int16           numAveForRand;              // По скольким измерениям усреднять сигнал в режиме рандомизатора.
    bool            viewAlteraWrittingData;     // Показывать ли данные, идущие в альтеру.
    bool            viewAllAlteraWrittingData;  // Показывать ли все данные, идущие в альтеру (если false, то постоянно идущие команды вроде START, STOP не показываются).
    int16           altShift;                   // Добавочное смещение для устранения эффекта горизонтальной помехи синхронизации.
    OutputRegisters showRegisters;
} SettingsDebug;

// Струкура хранит все настройки прибора.
typedef struct
{
    SettingsDisplay     display;                    // настройки изображения          (меню ДИСПЛЕЙ)
    SettingsTime        time;                       // временнЫе настройки            (меню РАЗВЁРТКА)
    SettingsCursors     cursors;                    // настройки курсорных измерений  (меню КУРСОРЫ)
    SettingsMemory      memory;                     // настройки режимов памяти       (меню ПАМЯТЬ)
    SettingsMath        math;                       // настройки режима математических измерений
    SettingsService     service;                    // дополнительные настройки       (меню СЕРВИС)
    SettingsEthernet    eth;
    SettingsCommon      common;                     // системные настройки
    SettingsDebug       debug;                      // настройки режима отладки       (меню ОТЛАДКА)
    int temp;
} Settings;

extern Settings set;

void    Settings_Load(void);    // Загрузить настройки
