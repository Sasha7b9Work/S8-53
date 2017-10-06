#pragma once
#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "DisplayTypes.h"


/// Возвращает цвет синхронизации.
Color ColorTrig(void);
/// Цвет заголовка страницы, inShade - затенена ли страница.
Color ColorMenuTitle(bool inShade);
/// Цвет элемента меню.
Color ColorMenuItem(bool inShade);
/// Цвет окантовки меню.
Color ColorBorderMenu(bool inShade);
/// Возвращает цвет, контрастный к color. Может быть белым или чёрным.
Color ColorContrast(Color color);
/// Светлый цвет в тени.
Color LightShadingTextColor(void);


#define ColorMenuField()            COLOR_MENU_FIELD
/// Чуть менее светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemLessBright()   COLOR_MENU_ITEM_DARK
/// Чуть более светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleBrighter()    COLOR_MENU_TITLE_BRIGHT
/// Чуть менее светлый цвет, чем цвет заголовка страницы. Используется для создания эффекта объёма.
#define ColorMenuTitleLessBright()  COLOR_MENU_TITLE_DARK
/// Чуть более светлый цвет, чем цвет элемента меню. Используется для создания эффекта объёма.
#define ColorMenuItemBrighter()     COLOR_MENU_ITEM_BRIGHT
/// Тёмный цвет в тени.
#define DarkShadingTextColor()      COLOR_MENU_TITLE_DARK


typedef struct
{
    float   red;
    float   green;
    float   blue;
    float   stepRed;
    float   stepGreen;
    float   stepBlue;
    float   brightness;
    Color   color;
    int8    currentField;
    bool    alreadyUsed;
} ColorType;

/// Вывести в лог значение цвета.
void Color_Log(Color color);

void Color_BrightnessChange(ColorType *colorType, int delta);

void Color_SetBrightness(ColorType *colorType, float brightness);
/// Если forced == false, то инициализация происходит только при первом вызове функции
void Color_Init(ColorType *colorType, bool forced);

void Color_ComponentChange(ColorType *colorType, int delta);

const char* NameColorFromValue(uint16 colorValue);

const char* NameColor(Color color);

#define MAKE_COLOR(r, g, b) (((b) & 0x1f) + (((g) & 0x3f) << 5) + (((r) & 0x1f) << 11))
#define R_FROM_COLOR(color) (((uint16)(color) >> 11) & (uint16)0x1f)
#define G_FROM_COLOR(color) (((uint16)(color) >> 5) & (uint16)0x3f)
#define B_FROM_COLOR(color) ((uint16)(color) & 0x1f)

extern Color gColorFill;
extern Color gColorBack;
extern Color gColorGrid;
extern Color gColorChan[4];
