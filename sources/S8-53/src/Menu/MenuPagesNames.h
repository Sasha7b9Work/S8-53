#pragma once

// Здесь ID всех представленных в меню страниц
typedef enum
{
    Page_Display,               // ДИСПЕЙ
    Page_DisplayAccumulation,   // ДИСПЛЕЙ - НАКОПЛЕНИЕ
    Page_DisplayAverage,        // ДИСПЛЕЙ - УСРЕДНЕНИЕ
    Page_DisplayGrid,           // ДИСПЛЕЙ - СЕТКА
    Page_DisplayRefresh,        // ДИСПЛЕЙ - ОБНОВЛЕНИЕ
    Page_MainPage,              // Главная страница меню. Из неё растут все остальные
    Page_Debug,                 // ОТЛАДКА
    Page_DebugConsole,          // ОТЛАДКА - КОНСОЛЬ
    Page_DebugADC,              // ОТЛАДКА - АЦП
    Page_DebugADCbalance,       // ОТЛАДКА - АЦП - БАЛАНС
    Page_DebugADCstretch,       // ОТЛАДКА - АЦП - РАСТЯЖКА
    Page_DebugADCrShift,        // ОТЛАДКА - АЦП - ДОП. СМЕЩЕНИЕ
    Page_DebugADCaltShift,      // ОТЛАДКА - АЦП - ДОП. СМЕЩ. ПАМ.
    Page_DebugShowRegisters,    // ОТЛАДКА - РЕГИСТРЫ
    Page_DebugRandomizer,       // ОТЛАДКА - РАНД-ТОР
    Page_DebugInformation,      // ОТЛАДКА - ИНФОРМАЦИЯ
    Page_DebugSound,            // ОТЛАДКА - ЗВУК
    Page_DebugAltera,           // ОТЛАДКА - АЛЬТЕРА
    Page_Channel0,              // КАНАЛ 1
    Page_Channel1,              // КАНАЛ 2
    Page_Cursors,               // КУРСОРЫ
    Page_Trig,                  // СИНХР.
    Page_Memory,                // ПАМЯТЬ
    Page_MemoryExt,             // ПАМЯТЬ - ВНЕШН. ЗУ
    Page_Measures,              // ИЗМЕРЕНИЯ
    Page_MeasuresField,         // ИЗМЕРЕНИЯ - ЗОНА
    Page_Service,               // СЕРВИС
    Page_ServiceDisplay,        // СЕРВИС - ДИСПЛЕЙ
    Page_ServiceDisplayColors,  // СЕРВИС - ДИСПЛЕЙ - ЦВЕТА
    Page_ServiceCalibrator,     // СЕРВИС - КАЛИБРАТОР
    Page_ServiceEthernet,       // СЕРВИС - ETHERNET
    Page_Help,                  // ПОМОЩЬ
    Page_Time,                  // РАЗВЕРТКА
    Page_Math,                  // СЕРВИС - МАТЕМАТИКА
    Page_MathFFT,
    Page_TrigAuto,              // СИНХР - АВТ ПОИСК
    Page_SB_Curs,               // Открыта страница КУРСОРЫ - Измерения
    Page_SB_MeasTuneMeas,       // Открыта страница ИЗМЕРЕНИЯ - Настроить
    Page_SB_MeasTuneField,      // Открыта страница ИЗМЕРЕНИЯ - ЗОНА - Задать
    Page_SB_MemLatest,          // Открыта страница ПАМЯТЬ - Последние
    Page_SB_MemInt,             // Открыта страница ПАМЯТЬ - Внутр ЗУ
    Page_SB_MemExtSetMask,      // Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - Маска
    Page_SB_MemExtSetName,      // Появляется перед сохранением файла на флешку для задания имени файла при соответствующей опции ВНЕШН ЗУ - Имя файла
    Page_SB_FileManager,        // Открыта страница ПАМЯТЬ - ВНЕШН ЗУ - Обзор
    Page_SB_Help,               // Открыта страница HELP
    Page_SB_MathCursorsFFT,     // Открыта страница СЕРВИС - Математика - Курсоры
    Page_SB_MathFunction,       // СЕРВИС - МАТЕМАТИКА - ФУНКЦИЯ
    Page_SB_Information,        // СЕРВИС - ИНФОРМАЦИЯ
    Page_SB_SerialNumber,       // ОТАДКА - С/Н
    Page_NumPages,
    Page_NoPage
} NamePage;             // При добавлении страниц нужно изменять инициализацию SettingsMenu в SSettings.c
