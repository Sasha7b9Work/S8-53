#include "MenuItemsGraphics.h"
#include "MenuItemsLogic.h"
#include "Menu.h"
#include "Hardware/Hardware.h"
#include "Hardware/CLOCK.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include <string.h>
#include <stdio.h>

 
static void Governor_DrawOpened(Governor *governor, int x, int y);
static void ItemMACaddress_DrawOpened(MACaddress *mac, int x, int y);
static void ItemIPaddress_DrawOpened(IPaddress *ip, int x, int y);


void DrawGovernorChoiceColorFormulaHiPart(void *item, int x, int y, bool pressed, bool shade, bool opened)
{

    int delta = pressed && !shade ? 1 : 0;
    int width = MI_WIDTH_VALUE;

    if (Menu::TypeMenuItem(item) == Item_IP && opened && ((IPaddress*)item)->port != 0)
    {
        width += MOI_WIDTH_D_IP;
    }

    Color color = shade ? ColorMenuTitleLessBright() : (IS_COLOR_SCHEME_WHITE_LETTERS ? COLOR_WHITE : COLOR_BLACK);
    Painter::DrawHLineC(y + 1, x, x + width + 3, ColorBorderMenu(false));

    if (shade)
    {
        Painter::FillRegionC(x + 1, y + 2, width + 2, MI_HEIGHT_VALUE + 3, ColorMenuItem(false));
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 2, width + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuItem(false), ColorMenuItemBrighter(), ColorMenuItemLessBright(), pressed, shade);
    }

    Painter::DrawTextC(x + 6 + delta, y + 6 + delta, Menu::TitleItem(item), color);
    
    TypeItem type = Menu::TypeMenuItem(item);

    if(Menu::CurrentItem() == item)
    {
        char symbol = 0;

        if (type == Item_Governor)
        {
            symbol = GetSymbolForGovernor(*((Governor*)item)->cell);
            ADDRESS_GOVERNOR = (uint)item;
        }
        else if (type == Item_Governor || type == Item_ChoiceReg ||  (Menu::ItemIsOpened(item) && type == Item_Choice))
        {
            symbol = GetSymbolForGovernor(*((Choice*)item)->cell);
        }
        else if (type == Item_Time)
        {
            Time *time = (Time*)item;
            if ((Menu::OpenedItem() == item) && (*time->curField != iEXIT) && (*time->curField != iSET))
            {
                int8 values[7] =
                {
                    0,
                    *time->day,
                    *time->month,
                    *time->year,
                    *time->hours,
                    *time->minutes,
                    *time->seconds
                };
                symbol = GetSymbolForGovernor(values[*time->curField]);
            }
        }

        Painter::Draw4SymbolsInRectC(x + MI_WIDTH - 13, y + 5 + (Menu::ItemIsOpened(item) ? 0 : 15), symbol, IS_COLOR_SCHEME_WHITE_LETTERS ? COLOR_BACK : COLOR_FILL);
    }
}

void DrawGovernorLowPart(Governor *governor, int x, int y, bool pressed, bool shade)
{
    char buffer[20];
    
    Color colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if(shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    x = Painter::DrawTextC(x + 4, y + 21, "\x80", colorTextDown);
    if(Menu::OpenedItem() != governor)
    {
        float delta = Governor_Step(governor);
        if(delta == 0.0f)
        {
            x = Painter::DrawText(x + 1, y + 21, Int2String(*governor->cell, false, 1, buffer));
        }
        else
        {
            int drawX = x + 1;
            int limX = x + 1;
            int limY = y + 19;
            int limWidth = MI_WIDTH_VALUE;
            int limHeight = MI_HEIGHT_VALUE - 1;
            if(delta > 0.0f)
            {
                x = Painter::DrawTextWithLimitationC(drawX, y + 21 - delta, Int2String(*governor->cell, false, 1, buffer), 
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
                Painter::DrawTextWithLimitationC(drawX, y + 21 + 10 - delta, Int2String(Governor_NextValue(governor), false, 1, buffer),
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
            }
            if(delta < 0.0f)
            {
                x = Painter::DrawTextWithLimitationC(drawX, y + 21 - delta, Int2String(*governor->cell, false, 1, buffer), 
                                            COLOR_BACK, limX, limY, limWidth, limHeight);
                Painter::DrawTextWithLimitationC(drawX, y + 21 - 10 - delta, Int2String(Governor_PrevValue(governor), false, 1, buffer),
                    COLOR_BACK, limX, limY, limWidth, limHeight);
            }
        }
    }
    else
    {
        x = Painter::DrawTextC(x + 1, y + 21, Int2String(*governor->cell, false, 1, buffer), COLOR_FILL);
    }
    Painter::DrawTextC(x + 1, y + 21, "\x81", colorTextDown);
}

static void DrawIPaddressLowPart(IPaddress *ip, int x, int y, bool pressed, bool shade)
{
    char buffer[20];

    Color colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                             ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    sprintf(buffer, "%03d.%03d.%03d.%03d", *ip->ip0, *ip->ip1, *ip->ip2, *ip->ip3);

    if (Menu::OpenedItem() != ip)
    {
        
        Painter::DrawTextC(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        Painter::DrawTextC(x + 4, y + 21, buffer, COLOR_FILL);
    }
}

static void DrawMACaddressLowPart(MACaddress *mac, int x, int y, bool pressed, bool shade)
{
    char buffer[20];

    Color colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                             ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }

    sprintf(buffer, "%02X.%02X.%02X.%02X.%02X.%02X", *mac->mac0, *mac->mac1, *mac->mac2, *mac->mac3, *mac->mac4, *mac->mac5);

    if (Menu::OpenedItem() != mac)
    {

        Painter::DrawTextC(x + 4, y + 21, buffer, colorTextDown);
    }
    else
    {
        Painter::DrawTextC(x + 4, y + 21, buffer, COLOR_FILL);
    }
}

void WriteTextFormula(Formula *formula, int x, int y, bool opened)
{
    Function function = (Function)(*formula->function);
    
    if (function != Function_Mul && function != Function_Sum)
    {
        return;
    }

    bool funcIsMul = function == Function_Mul;
    int8 koeff1 = funcIsMul ? *formula->koeff1mul : *formula->koeff1add;
    int8 koeff2 = funcIsMul ? *formula->koeff2mul : *formula->koeff2add;
    if (koeff1 != 0)
    {
       Painter::DrawChar(x, y, koeff1 < 0 ? '-' : '+');
    }
    Painter::DrawChar(x + 5, y, (char)(koeff1 + 0x30));
    Painter::DrawChar(x + 10, y, '*');
    Painter::DrawText(x + 14, y, "K1");
    Painter::DrawChar(x + 27, y, funcIsMul ? '*' : '+');
    if (koeff2 != 0)
    {
       Painter::DrawChar(x + 30, y, koeff2 < 0 ? '-' : '+');
    }
    Painter::DrawChar(x + 39, y, (char)(koeff2 + 0x30));
    Painter::DrawChar(x + 44, y, '*');
    Painter::DrawText(x + 48, y, "K2");
}

void DrawFormulaLowPart(Formula *formula, int x, int y, bool pressed, bool shade)
{
    Color colorTextDown = COLOR_BACK;

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(),
                     ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    if (shade)
    {
        colorTextDown = ColorMenuItem(false);
    }
    Painter::SetColor(colorTextDown);
    WriteTextFormula(formula, x + 6, y + 21, false);
}

static void Governor_DrawClosed(Governor *governor, int x, int y)
{
    bool pressed = Menu::IsPressed(governor);
    bool shade = Menu::IsShade(governor) || !Menu::ItemIsActive(governor);
    DrawGovernorLowPart(governor, x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(governor, x, y, pressed, shade, false);
}

static void ItemIPaddress_DrawClosed(IPaddress *ip, int x, int y)
{
    bool pressed = Menu::IsPressed(ip);
    bool shade = Menu::IsShade(ip) || !Menu::ItemIsActive(ip);
    DrawIPaddressLowPart(ip, x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(ip, x, y, pressed, shade, false);
}

static void ItemMACaddress_DrawClosed(MACaddress *mac, int x, int y)
{
    bool pressed = Menu::IsPressed(mac);
    bool shade = Menu::IsShade(mac) || !Menu::ItemIsActive(mac);
    DrawMACaddressLowPart(mac, x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(mac, x, y, pressed, shade, false);
}

void Formula_DrawClosed(Formula *formula, int x, int y)
{
    bool pressed = Menu::IsPressed(formula);
    bool shade = Menu::IsShade(formula) || !Menu::ItemIsActive(formula);
    DrawFormulaLowPart(formula, x, y, pressed, shade);
    DrawGovernorChoiceColorFormulaHiPart(formula, x, y, pressed, shade, false);
}

static void DrawValueWithSelectedPosition(int x, int y, int value, int numDigits, int selPos, bool hLine, bool fillNull) // Если selPos == -1, подсвечивать не нужно
{
    int firstValue = value;
    int height = hLine ? 9 : 8;
    for (int i = 0; i < numDigits; i++)
    {
        int rest = value % 10;
        value /= 10;
        if (selPos == i)
        {
            Painter::FillRegionC(x - 1, y, 5, height, COLOR_FILL);
        }
        if (!(rest == 0 && value == 0) || (firstValue == 0 && i == 0))
        {
            Painter::DrawCharC(x, y, rest + 48, selPos == i ? COLOR_BACK : COLOR_FILL);
        }
        else if (fillNull)
        {
            Painter::DrawCharC(x, y, '0', selPos == i ? COLOR_BACK : COLOR_FILL);
        }
        if (hLine)
        {
            Painter::DrawLineC(x, y + 9, x + 3, y + 9, COLOR_FILL);
        }
        x -= 6;
    }
}

void DrawGovernorValue(int x, int y, Governor *governor)
{
    char buffer[20];

    int startX = x + 40;
    int16 value = *governor->cell;
    int signGovernor = *governor->cell < 0 ? -1 : 1;
    if(signGovernor == -1)
    {
        value = -value;
    }
    Painter::SetFont(TypeFont_5);
    bool sign = governor->minValue < 0;
    Painter::DrawTextC(x + 55, y - 5, Int2String(governor->maxValue, sign, 1, buffer), COLOR_FILL);
    Painter::DrawText(x + 55, y + 2, Int2String(governor->minValue, sign, 1, buffer));
    Painter::SetFont(TypeFont_8);

    DrawValueWithSelectedPosition(startX, y, value, Governor_NumDigits(governor), gCurDigit, true, true);

    if(sign)
    {
       Painter::DrawChar(startX - 1, y, signGovernor < 0 ? '\x9b' : '\x9a');
    }
}

static void DrawIPvalue(int x, int y, IPaddress *ip)
{
    if (gCurDigit > (ip->port == 0 ? 11 : 16))
    {
        gCurDigit = 0;
    }

    uint8 *bytes = ip->ip0;

    x += 15;

    y += 1;

    int numIP = 0;
    int selPos = 0;

    IPaddress_GetNumPosIPvalue(&numIP, &selPos);

    for (int i = 0; i < 4; i++)
    {
        DrawValueWithSelectedPosition(x, y, bytes[i], 3, numIP == i ? selPos : -1, false, true);
        if (i != 3)
        {
            Painter::DrawCharC(x + 5, y, '.', COLOR_FILL);
        }
        x += 19;
    }

    if (ip->port != 0)
    {
        Painter::DrawCharC(x - 13, y, ':', COLOR_FILL);
        DrawValueWithSelectedPosition(x + 14, y, *ip->port, 5, numIP == 4 ? selPos : -1, false, true);
    }
}

static void DrawMACvalue(int x, int y, MACaddress *mac)
{
    if (gCurDigit > 5)
    {
        gCurDigit = 0;
    }
    uint8 *bytes = mac->mac0;
    x += MOI_WIDTH - 14;
    y++;
    for (int num = 5; num >= 0; num--)
    {
        int value = (int)(*(bytes + num));
        if (gCurDigit == num)
        {
            Painter::FillRegionC(x - 1, y, 10, 8, COLOR_FILL);
        }
        char buffer[20];
        sprintf(buffer, "%02X", value);
        Painter::DrawTextC(x, y, buffer, gCurDigit == num ? COLOR_BACK : COLOR_FILL);
        x -= 12;
    }
}

void ItemGovernor_Draw(Governor *governor, int x, int y, bool opened)
{
    if (governor->funcBeforeDraw)
    {
        governor->funcBeforeDraw();
    }
    if(opened)
    {
        Governor_DrawOpened(governor, x, y);
    }
    else
    {
        Governor_DrawClosed(governor, x, y);
    }
}

void ItemIPaddress_Draw(IPaddress *ip, int x, int y, bool opened)
{
    if (opened)
    {
        ItemIPaddress_DrawOpened(ip, x - (ip->port == 0 ? 0 : MOI_WIDTH_D_IP), y);
    }
    else
    {
        ItemIPaddress_DrawClosed(ip, x, y);
    }
}

void ItemMACaddress_Draw(MACaddress *mac, int x, int y, bool opened)
{
    if (opened)
    {
        ItemMACaddress_DrawOpened(mac, x, y);
    }
    else
    {
        ItemMACaddress_DrawClosed(mac, x, y);
    }
}

void ItemFormula_Draw(Formula *formula, int x, int y, bool opened)
{
    if (opened)
    {
        
    }
    else
    {
        Formula_DrawClosed(formula, x, y);
    }
}

void DrawGovernorColorValue(int x, int y, GovernorColor *govColor, int delta)
{
    char buffer[20];
    
    ColorType *ct = govColor->colorType;
    int8 field = ct->currentField;
    char *texts[4] = {"Яр", "Сн", "Зл", "Кр"};
    uint16 color = set.display.colors[ct->color];
    int red = R_FROM_COLOR(color);
    int green = G_FROM_COLOR(color);
    int blue = B_FROM_COLOR(color);
    if(!ct->alreadyUsed)
    {
        Color_Init(ct);
    }

    int vals[4] = {ct->brightness * 100, blue, green, red};

    Painter::FillRegionC(x, y, MI_WIDTH + delta - 2, MI_HEIGHT / 2 - 3, COLOR_BLACK);
    x += 92;
    
    for(int i = 0; i < 4; i++)
    {
        Color colorBack = (field == i) ? COLOR_WHITE : COLOR_BLACK;
        Color colorDraw = (field == i) ? COLOR_BLACK : COLOR_WHITE;
        Painter::FillRegionC(x - 1, y + 1, 29, 10, colorBack);
        Painter::DrawTextC(x, y + 2, texts[i], colorDraw);
        Painter::DrawText(x + 14, y + 2, Int2String(vals[i], false, 1, buffer));
        x -= 30;
    }
    
}

static void GovernorColor_DrawOpened(GovernorColor *gov, int x, int y)
{
    static const int delta = 43;
    x -= delta;
    Color_Init(gov->colorType);
    Painter::DrawRectangleC(x - 1, y - 1, MI_WIDTH + delta + 2, MI_HEIGHT + 2, COLOR_BLACK);
    Painter::DrawRectangleC(x, y, MI_WIDTH + delta, MI_HEIGHT, ColorMenuTitle(false));
    Painter::DrawVolumeButton(x + 1, y + 1, MI_WIDTH_VALUE + 2 + delta, MI_HEIGHT_VALUE + 3, 2, ColorMenuItem(false), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), Menu::IsPressed(gov), Menu::IsShade(gov));
    Painter::DrawHLineC(y + MI_HEIGHT / 2 + 2, x, x + MI_WIDTH + delta, ColorMenuTitle(false));
    Painter::DrawStringInCenterRectC(x + (Menu::IsPressed(gov) ? 2 : 1), y + (Menu::IsPressed(gov) ? 2 : 1), MI_WIDTH + delta, MI_HEIGHT / 2 + 2, Menu::TitleItem(gov), COLOR_WHITE);
    DrawGovernorColorValue(x + 1, y + 19, gov, delta);
}

static void GovernorColor_DrawClosed(GovernorColor *gov, int x, int y)
{
    Color_Init(gov->colorType);
    DrawGovernorChoiceColorFormulaHiPart(gov, x, y, Menu::IsPressed(gov), Menu::IsShade(gov) || !Menu::ItemIsActive(gov), true);
    Painter::FillRegionC(x + 2, y + 20, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1, gov->colorType->color);
}

void GovernorColor_Draw(GovernorColor *govColor, int x, int y, bool opened)
{
    if(opened)
    {
        GovernorColor_DrawOpened(govColor, x, y);
    }
    else
    {
        GovernorColor_DrawClosed(govColor, x, y);
    }
}

void ItemChoice_DrawOpened(Choice *choice, int x, int y)
{
    int height = Menu::HeightOpenedItem(choice);

    Painter::DrawRectangleC(x - 1, y - 1, MP_TITLE_WIDTH + 2, height + 3, COLOR_BACK);
    
    DrawGovernorChoiceColorFormulaHiPart(choice, x - 1, y - 1, Menu::IsPressed(choice), false, true);
    Painter::DrawRectangleC(x - 1, y, MP_TITLE_WIDTH + 1, height + 1, ColorMenuTitle(false));
 
    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, COLOR_BLACK, ColorMenuTitleBrighter(),
                        ColorMenuTitleLessBright(), false, false);
    int index = *((int8*)choice->cell);
    for(int i = 0; i < choice->NumSubItems(); i++)
    {
        int yItem = y + MOI_HEIGHT_TITLE + i * MOSI_HEIGHT + 1;
        bool pressed = i == index;
        if(pressed)
        {
            Painter::DrawVolumeButton(x + 1, yItem, MOI_WIDTH - 2 , MOSI_HEIGHT - 2, 2, ColorMenuField(), ColorMenuTitleBrighter(),
                ColorMenuTitleLessBright(), pressed, false);
        }
        Painter::DrawTextC(x + 4, yItem + 2, choice->NameSubItem(i), pressed ? COLOR_BLACK : ColorMenuField());
    }
}

void ItemTime_DrawOpened(Time *time, int x, int y)
{
    char buffer[20];
    
    int width = MI_WIDTH_VALUE + 3;
    int height = 61;
    Painter::DrawRectangleC(x - 1, y - 1, width + 2, height + 3, COLOR_BACK);
    DrawGovernorChoiceColorFormulaHiPart(time, x - 1, y - 1, Menu::IsPressed(time), false, true);

    Painter::DrawRectangleC(x - 1, y, width + 1, height + 1, ColorMenuTitle(false));

    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1, height - MOI_HEIGHT_TITLE, 1, COLOR_BLACK, ColorMenuTitleBrighter(),
                             ColorMenuTitleLessBright(), false, false);

    int y0 = 21;
    int y1 = 31;
    int y2 = 41;
    int y3 = 51;

    int x0 = 41;
    int dX = 13;
    int wS = 10;

    struct StructPaint
    {
        int x;
        int y;
        int width;
    } strPaint[8] =
    {
        {3,             y3, 60},    // Не сохранять
        {x0,            y0, wS},    // день
        {x0 + dX,       y0, wS},    // месяц
        {x0 + 2 * dX,   y0, wS},    // год
        {x0,            y1, wS},    // часы
        {x0 + dX,       y1, wS},    // мин
        {x0 + 2 * dX,   y1, wS},    // сек
        {3,             y2, 46}     // Сохранить
    };

    char strI[8][20];
    strcpy(strI[iEXIT],     "Не сохранять");
    strcpy(strI[iDAY],      Int2String(*time->day,      false, 2, buffer));
    strcpy(strI[iMONTH],    Int2String(*time->month,    false, 2, buffer));
    strcpy(strI[iYEAR],     Int2String(*time->year,     false, 2, buffer));
    strcpy(strI[iHOURS],    Int2String(*time->hours,    false, 2, buffer));
    strcpy(strI[iMIN],      Int2String(*time->minutes,  false, 2, buffer));
    strcpy(strI[iSEC],      Int2String(*time->seconds,  false, 2, buffer));
    strcpy(strI[iSET],      "Сохранить");

    Painter::DrawTextC(x + 3, y + y0, "д м г - ", COLOR_FILL);
    Painter::DrawText(x + 3, y + y1, "ч м с - ");

    for (int i = 0; i < 8; i++)
    {
        if (*time->curField == i)
        {
            Painter::FillRegionC(x + strPaint[i].x - 1, y + strPaint[i].y, strPaint[i].width, 8, COLOR_FLASH_10);
        }
        Painter::DrawTextC(x + strPaint[i].x, y + strPaint[i].y, strI[i], *time->curField == i ? COLOR_FLASH_01 : COLOR_FILL);
    }
}

static void GovernorIpCommon_DrawOpened(void *item, int x, int y, int dWidth)
{
    int height = 34;
    Painter::DrawRectangleC(x - 1, y - 1, MP_TITLE_WIDTH + 2 + dWidth, height + 3, COLOR_BACK);
    Painter::DrawRectangleC(x - 1, y, MP_TITLE_WIDTH + 1 + dWidth, height + 1, ColorMenuTitle(false));
    Painter::DrawHLine(y + MOI_HEIGHT_TITLE - 1, x, x + MOI_WIDTH + dWidth);
    DrawGovernorChoiceColorFormulaHiPart(item, x - 1, y - 1, Menu::IsPressed(item), false, true);
    Painter::DrawVolumeButton(x, y + MOI_HEIGHT_TITLE, MOI_WIDTH - 1 + dWidth, height - MOI_HEIGHT_TITLE, 1, COLOR_BLACK, ColorMenuTitleBrighter(),
                             ColorMenuTitleLessBright(), false, false);
}

static void Governor_DrawOpened(Governor *governor, int x, int y)
{
    GovernorIpCommon_DrawOpened(governor, x, y, 0);
    DrawGovernorValue(x, y + 22, governor);
}

static void ItemIPaddress_DrawOpened(IPaddress *ip, int x, int y)
{
    GovernorIpCommon_DrawOpened(ip, x, y, ip->port == 0 ? 0 : MOI_WIDTH_D_IP);
    DrawIPvalue(x, y + 22, ip);
}

static void ItemMACaddress_DrawOpened(MACaddress *mac, int x, int y)
{
    GovernorIpCommon_DrawOpened(mac, x, y, 0);
    DrawMACvalue(x, y + 22, mac);
}

void ItemChoice_DrawClosed(Choice *choice, int x, int y)
{
    bool pressed = Menu::IsPressed(choice);
    bool shade = Menu::IsShade(choice) || !Menu::ItemIsActive(choice);
        
    if (shade)
    {
        Painter::FillRegionC(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, ColorMenuTitleLessBright());
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, ColorMenuField(), ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    }

    float deltaY = choice->Step();
    Color colorText = shade ? LightShadingTextColor() : COLOR_BACK;
    Painter::SetColor(colorText);
    if(deltaY == 0.0f)
    {
        Painter::DrawText(x + 4, y + 21, choice->NameCurrentSubItem());
    }
    else
    {
        Painter::DrawTextWithLimitationC(x + 4, y + 21 - deltaY, choice->NameCurrentSubItem(), colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
        Painter::DrawHLineC(y + (deltaY > 0 ? 31 : 19) - deltaY, x + 3, x + MI_WIDTH_VALUE + 1, COLOR_BLACK);
        Painter::DrawTextWithLimitationC(x + 4, y + (deltaY > 0 ? 33 : 9) - deltaY, deltaY > 0 ? choice->NameNextSubItem() : choice->NamePrevSubItem(), colorText, x, y + 19, MI_WIDTH_VALUE, MI_HEIGHT_VALUE - 1);
    }
    Painter::DrawHLineC(y + MI_HEIGHT + 1, x, x + MI_WIDTH, ColorBorderMenu(false));

    DrawGovernorChoiceColorFormulaHiPart(choice, x, y, pressed, shade, false);

    choice->FuncForDraw(x, y);
}

void ItemChoice_Draw(Choice *choice, int x, int y, bool opened)
{
    if(opened)
    {
        ItemChoice_DrawOpened(choice, x, y);
    }
    else
    {
        ItemChoice_DrawClosed(choice, x, y);
    }
}

void ItemTime_DrawClosed(Time *item, int x, int y)
{
    char buffer[20];
    
    bool pressed = Menu::IsPressed(item);
    bool shade = Menu::IsShade(item);
    DrawGovernorChoiceColorFormulaHiPart(item, x, y, pressed, shade, false);

    Painter::DrawVolumeButton(x + 1, y + 17, MI_WIDTH_VALUE + 2, MI_HEIGHT_VALUE + 3, 2, shade ? ColorMenuTitleLessBright() : ColorMenuField(), 
        ColorMenuItemBrighter(), ColorMenuItemLessBright(), true, shade);
    //int delta = 0;

    int deltaField = 10;
    int deltaSeparator = 2;
    int startX = 3;
    y += 21;
    PackedTime time = Clock::GetPackedTime();
    Painter::DrawTextC(x + startX, y, Int2String((int)time.hours, false, 2, buffer), COLOR_BACK);
    Painter::DrawText(x + startX + deltaField, y, ":");
    Painter::DrawText(x + startX + deltaField + deltaSeparator, y, Int2String((int)time.minutes, false, 2, buffer));
    Painter::DrawText(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    Painter::DrawText(x + startX + 2 * deltaField + 2 * deltaSeparator, y, Int2String((int)time.seconds, false, 2, buffer));

    startX = 44;
    Painter::DrawText(x + startX, y, Int2String((int)time.day, false, 2, buffer));
    Painter::DrawText(x + startX + deltaField, y, ":");
    Painter::DrawText(x + startX + deltaField + deltaSeparator, y, Int2String((int)time.month, false, 2, buffer));
    Painter::DrawText(x + startX + 2 * deltaField + deltaSeparator, y, ":");
    Painter::DrawText(x + startX + 2 * deltaField + 2 * deltaSeparator, y, Int2String((int)time.year, false, 2, buffer));
}

void ItemTime_Draw(Time *time, int x, int y, bool opened)
{
    if(opened)
    {
        ItemTime_DrawOpened(time, x, y);
    }
    else
    {
        ItemTime_DrawClosed(time, x, y);
    }
}

void ItemButton_Draw(Button *button, int x, int y)
{
    bool pressed = Menu::IsPressed(button);
    bool shade = Menu::IsShade(button) || !Menu::ItemIsActive(button);

    Painter::DrawHLineC(y + 1, x, x + MI_WIDTH, ColorMenuTitle(shade));
    Color color = shade ? COLOR_MENU_SHADOW : COLOR_WHITE;
    Painter::FillRegionC(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, ColorMenuItem(false));
    Painter::DrawVolumeButton(x + 4, y + 5, MI_WIDTH - 8, MI_HEIGHT - 8, 3, ColorMenuItem(false), ColorMenuItemBrighter(), 
                            ColorMenuItemLessBright(), pressed, shade);

    int delta = (pressed && (!shade)) ? 2 : 1;
    
    Painter::DrawStringInCenterRectC(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, Menu::TitleItem(button), color);
}

void ItemSmallButton_Draw(SmallButton *smallButton, int x, int y)
{
    if (Menu::ItemIsActive(smallButton))
    {
        if (Menu::IsPressed(smallButton))
        {
            Painter::FillRegionC(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
            Painter::SetColor(COLOR_BACK);
        }
        else
        {
            Painter::DrawRectangleC(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
            Painter::SetColor(COLOR_FILL);
        }
        smallButton->funcOnDraw(x, y);
    }
    else
    {
        Painter::DrawRectangleC(x, y, WIDTH_SB, WIDTH_SB, COLOR_FILL);
    }
}

void ItemPage_Draw(Page *page, int x, int y)
{
    bool isShade = Menu::IsShade(page) || !Menu::ItemIsActive(page);
    bool isPressed = Menu::IsPressed(page);
    Painter::DrawHLineC(y + 1, x, x + MI_WIDTH, ColorBorderMenu(false));
    if (isShade)
    {
        Painter::FillRegionC(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, ColorMenuTitleLessBright());
    }
    else
    {
        Painter::DrawVolumeButton(x + 1, y + 2, MI_WIDTH - 2, MI_HEIGHT - 2, 2, ColorMenuItem(isShade), ColorMenuItemBrighter(), ColorMenuItemLessBright(), isPressed, isShade);
    }
    Color colorText = isShade ? LightShadingTextColor() : COLOR_BLACK;
    int delta = 0;
    if(isPressed && (!isShade))
    {
        colorText = COLOR_FILL;
        delta = 1;
    }
    Painter::DrawStringInCenterRectC(x + delta, y + delta, MI_WIDTH, MI_HEIGHT, Menu::TitleItem(page), colorText);
}
