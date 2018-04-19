#pragma once
#include "Display/Display.h"
#include "Display/Colors.h"
#include "defines.h"

                                            
#define MAX_NUM_SUBITEMS_IN_CHOICE  12  // Максимальное количество вариантов выбора в элементе Choice.
#define MAX_NUM_ITEMS_IN_PAGE       15  // Максимальное количество пунктов на странице.
#define MENU_ITEMS_ON_DISPLAY       5   // Сколько пунктов меню помещается на экране по вертикали.
#define MAX_NUM_CHOICE_SMALL_BUTTON 6   // Максимальное количество вариантов маленькой кнопки + 1


// Разные виды пунктов меню
typedef enum
{
    Item_None,           
    Item_Choice,        // Пункт выбора - позволяет выбрать одно из нескольких заданных значений.
    Item_Button,        // Кнопка.
    Item_Page,          // Страница.
    Item_Governor,      // Регулятор - позволяет выбрать любое целое числовое значение из заранее заданного диапазаона.
    Item_Time,          // Позволяет ввести время.
    Item_IP,            // Позволяет ввести IP-адрес.
    Item_SwitchButton,
    Item_GovernorColor, // Позволяет выбрать цвет.
    Item_Formula,       // Позволяет выбрать знак и коэффициенты для математической формулы (умножение и сложение)
    Item_MAC,           // MAC-адрес
    Item_ChoiceReg,     // Элемент выбора, в котором выбор осуществляется не кнопкой, а ручкой
    Item_SmallButton,   // Кнопка для режима малых кнопок
    Item_NumberItems
} TypeItem;

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

class Page;

#define COMMON_PART_MENU_ITEM                                                                                     \
    TypeItem                type;             /* Тип итема */                                                     \
    const struct Page*      keeper;           /* Адрес страницы, которой принадлежит. Для Page_MainPage = 0 */    \
    pFuncBV                 funcOfActive;     /* Активен ли данный элемент */                                     \
    const char*             titleHint[4];     /* Название страницы на русском и английском языках. Также подсказка для режима помощи */


//----------------------------------------------------------------------------------------------------------------------------------------------------
struct ControlStruct
{
    COMMON_PART_MENU_ITEM
};
class Control
{
public:
    COMMON_PART_MENU_ITEM

public:
    Control(const ControlStruct *str);
    Control(TypeItem type_, const Page* keeper_, pFuncBV funcOfActive_, const char *tileRU, const char *titleEN, const char *hintRU, const char *hintEN);
    /// Возвращает true, если кнопка, соответствующая данному элементу меню, находится в нажатом положении.
    bool IsPressed();
};


#define TITLE(item) ((item)->titleHint[LANG])
#define HINT(item) ((item)->titleHint[2 + LANG])

typedef void * pVOID;
typedef pVOID arrayItems[MAX_NUM_ITEMS_IN_PAGE];

//----------------------------------------------------------------------------------------------------------------------------------------------------
struct PageStruct
{
    COMMON_PART_MENU_ITEM
    NamePage            name;
    const arrayItems    items;
    pFuncVV             funcOnPress;
    pFuncVV             funcOnDraw;
    pFuncVI             funcRegSetSB;
};

/// Описывает страницу меню.
class Page : public     Control
{
public:
    NamePage            name;                               // Имя из перечисления NamePage
    const arrayItems   *items;                              // Здесь указатели на пункты этой страницы (в обычной странице)
                                                            // для страницы малых кнопок  здесь хранятся 6 указателей на SmallButton : 0 - B_Menu, 1...5 - B_F1...B_F5
    pFuncVV             funcOnPress;                        // Будет вызываться при нажатии на свёрнутую страницу
    pFuncVV             funcOnDraw;                         // Будет вызываться после отрисовки кнопок
    pFuncVI             funcRegSetSB;                       // В странице малых кнопок вызывается при повороте ручки установка

    //Page(PageStruct *pageStruct);
    
    Page(const Page *keeper_, pFuncBV funcOfActive_,
         const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN, NamePage name_,
         const arrayItems *items_, pFuncVV funcOnPress_ = 0, pFuncVV funcOnDraw_ = 0, pFuncVI funcRegSetSB_ = 0);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// Описывает кнопку.
class Button : public Control
{
public:
    pFuncVV         funcOnPress;    // Функция, которая вызывается при нажатии на кнопку.

    Button(const Page *keeper_, pFuncBV funcOfActive_,
            const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN, pFuncVV funcOnPress_);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
class StructHelpSmallButton
{
public:
    pFuncVII    funcDrawUGO;    ///< Указатель на функцию отрисовки изображения варианта кнопки
    const char *helpUGO[2];     ///< Подпись к данному изображению.
};

typedef StructHelpSmallButton arrayHints[MAX_NUM_CHOICE_SMALL_BUTTON];

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// Описывает кнопку для дополнительного режима меню.
class SmallButton : public Control
{
public:
    pFuncVV             funcOnPress;    ///< Эта функция вызвается для обработки нажатия кнопки. Возвращает true, если надо за
    pFuncVII            funcOnDraw;     ///< Эта функция вызывается для отрисовки кнопки в месте с координатами x, y.
    const arrayHints   *hintUGO;

    SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
                const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN,
                pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_ = 0);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// Описывает регулятор.
class Governor : public Control
{ 
public:
    int16*      cell;
    int         minValue;               ///< Минмальное значение, которое может принимать регулятор.
    int16       maxValue;               ///< Максимальное значение.
    pFuncVV     funcOfChanged;          ///< Функция, которую нужно вызывать после того, как значение регулятора изменилось.
    pFuncVV     funcBeforeDraw;         ///< Функция, которая вызывается перед отрисовкой

    Governor(const Page *keeper_, pFuncBV funcOfActive_,
             const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN,
             int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_ = 0, pFuncVV funcBeforeDraw_ = 0);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
struct IPaddressStruct
{
    ControlStruct str;
    uint8* ip0;
    uint8* ip1;
    uint8* ip2;
    uint8* ip3;
    pFuncVB funcOfChanged;
    uint16 *port;
};

class  IPaddress : public Control
{
public:
    uint8*  ip0;
    uint8*  ip1;
    uint8*  ip2;
    uint8*  ip3;
    pFuncVB funcOfChanged;
    uint16* port;

    IPaddress(const IPaddressStruct *str);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
class  MACaddress
{
public:
    COMMON_PART_MENU_ITEM
    uint8*  mac0;
    uint8*  mac1;
    uint8*  mac2;
    uint8*  mac3;
    uint8*  mac4;
    uint8*  mac5;
    pFuncVB funcOfChanged;
};

// Описывает элемент меню для установки коэффициентов и знаков математической формулы
#define FIELD_SIGN_MEMBER_1_ADD 0
#define FIELD_SIGN_MEMBER_1_MUL 1
#define FIELD_SIGN_MEMBER_2_ADD 2
#define FIELD_SIGN_MEMBER_2_MUL 3
#define POS_SIGN_MEMBER_1       0
#define POS_KOEFF_MEMBER_1      1
#define POS_SIGN_MEMBER_2       2
#define POS_KOEFF_MEMBER_2      3
//----------------------------------------------------------------------------------------------------------------------------------------------------
class Formula
{
public:
    COMMON_PART_MENU_ITEM
    int8*           function;                   // Адрес ячейки, где хранится Function, из которой берётся знак операции
    int8*           koeff1add;                  // Адрес коэффициента при первом члене для сложения
    int8*           koeff2add;                  // Адрес коэффициента при втором члене для сложения
    int8*           koeff1mul;                  // Адрес коэффициента при первом члене для умножения
    int8*           koeff2mul;                  // Адрес коэффициента при втором члене для умножения
    int8*           curDigit;                   // Текущий разряд : 0 - знак первого члена, 1 - коэффициент первого члена,
                                                //  2 - знак второго члена, 3 - коэффициент второго члена
    pFuncVV         funcOfChanged;              // Эта функция вызывается после изменения состояния элемента управления.
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
class  GovernorColor
{
public:
    COMMON_PART_MENU_ITEM
    ColorType*      colorType;                  // Структура для описания цвета.
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
class Choice
{
public:
    COMMON_PART_MENU_ITEM
    const char *names[MAX_NUM_SUBITEMS_IN_CHOICE][2];   // Варианты выбора на русском и английском языках.
    int8*       cell;                                   // Адрес ячейки, в которой хранится позиция текущего выбора.
    pFuncVB	    funcOnChanged;                          // Функция должна вызываться после изменения значения элемента.
    pFuncVII    funcForDraw;                            // Функция вызывается после отрисовки элемента. 
    const char *NameSubItem(int i);
    /// Возвращает имя текущего варианта выбора элемента choice, как оно записано в исходном коде программы.
    const char *NameCurrentSubItem();
    /// Возвращает имя следующего варианта выбора элемента choice, как оно записано в исходном коде программы.
    const char *NameNextSubItem();

    const char *NamePrevSubItem();
    /// Возвращает количество вариантов выбора в элементе по адресу choice.
    int NumSubItems();

    void StartChange(int delta);
    /// Рассчитывает следующий кадр анимации.
    float Step();
    /// Изменяет значение choice в зависимости от величины и знака delta.
    void ChangeValue(int delta);

    void FuncOnChanged(bool active);

    void FuncForDraw(int x, int y);
};


#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

// Устанавливает и показывает время.
typedef struct
{
    COMMON_PART_MENU_ITEM
    int8*       curField;       // Текущее поле установки. 0 - выход, 1 - сек, 2 - мин, 3 - часы, 4 - день, 5 - месяц, 6 - год, 7 - установить.
    int8*       hours;
    int8*       minutes;
    int8*       seconds;
    int8*       month;
    int8*       day;
    int8*       year;
} Time;

