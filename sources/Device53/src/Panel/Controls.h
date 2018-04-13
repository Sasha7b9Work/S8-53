#pragma once


// Идентификаторы кнопок.
typedef enum
{
    B_Empty     = 0,        // кнопка не нажата
    B_Channel0  = 1,        // КАНАЛ 1
    B_Service   = 2,        // СЕРВИС
    B_Channel1  = 3,        // КАНАЛ 2
    B_Display   = 4,        // ДИСПЛЕЙ
    B_Time      = 5,        // РАЗВ
    B_Memory    = 6,        // ПАМЯТЬ
    B_Trig      = 7,        // СИНХР
    B_Start     = 8,        // ПУСК/СТАРТ
    B_Cursors   = 9,        // КУРСОРЫ
    B_Measures  = 10,       // ИЗМЕР
    B_Power     = 11,       // ПИТАНИЕ
    B_Help      = 12,       // ПОМОЩЬ
    B_Menu      = 13,       // МЕНЮ
    B_F1        = 14,       // 1
    B_F2        = 15,       // 2
    B_F3        = 16,       // 3
    B_F4        = 17,       // 4
    B_F5        = 18,       // 5
    B_NumButtons            // общее количество кнопок
} PanelButton;

PanelButton& operator++(PanelButton &button);

// Идентификаторы регуляторов.
typedef enum
{
    R_Empty     = 0,    // регулятор не вращался
    R_Range0    = 20,   // ВОЛЬТ/ДЕЛ кан. 1
    R_RShift0   = 21,   
    R_Range1    = 22,   // ВОЛЬТ/ДЕЛ кан. 2
    R_RShift1   = 23,
    R_TBase     = 24,   // ВРЕМЯ/ДЕЛ
    R_TShift    = 25,
    R_TrigLev   = 26,   // УРОВЕНЬ
    R_Set       = 27    // УСТАНОВКА
} Regulator;

// Направления вращения регуляторов.
typedef enum
{
    Direction_Left,     // вращение регулятора влево
    Direction_Right,    // вращение регулятора вправо
    Direction_None      // вращение отсутствует
} RegulatorDirection;


#define NUM_REGULATORS (R_Set - R_Range0 + 1)


const char* NameButton(PanelButton button);
