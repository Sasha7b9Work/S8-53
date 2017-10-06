#include "Colors.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Painter.h"
#include "Log.h"

#include <math.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorChannel(Channel chan)
{
    static const Color colors[4] = {COLOR_DATA_A, COLOR_DATA_B, COLOR_WHITE, COLOR_WHITE};
    return colors[chan];
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorCursors(Channel chan)
{
    static const Color colors[4] = {COLOR_CURSORS_A, COLOR_CURSORS_B, COLOR_WHITE, COLOR_WHITE};
    return colors[chan];
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorMenuTitle(bool inShade)
{
    return inShade ? COLOR_MENU_ITEM : COLOR_MENU_TITLE;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorMenuItem(bool inShade)
{
    return inShade ? COLOR_MENU_ITEM_DARK : COLOR_MENU_ITEM;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorBorderMenu(bool inShade)
{
    return ColorMenuTitle(inShade);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color ColorContrast(Color color)
{
    uint16 colorValue = set.display.colors[color];
    if (R_FROM_COLOR(colorValue) > 16 || G_FROM_COLOR(colorValue) > 32 || B_FROM_COLOR(colorValue) > 16)    //-V112
    {
        return COLOR_BLACK;
    }
    return COLOR_WHITE;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Color LightShadingTextColor()
{
    return ColorMenuTitle(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Color_Log(Color color)
{
}

/*
    јлгоритм изменени€ €ркости.
    1. »нициализаци€.
    а. –ассчитать €ркость по принципу - €ркость равна относительной интенсивности самого €ркого цветового канала
    б. –ассчитать шаг изменени€ цветовой составл€ющей каждого канала на 1% €ркости по формуле:
    Ўаг = »нтенсивность канала * яркость, где яркость - относительна€ величина общей €ркости
    в. ≈сли интенсивность всех каналов == 0, то дать полный шаг каждому каналу
    2. ѕри изменнении €ркости на 1% мен€ть интенсивность каждого канала на Ўаг, расчитанный в предыдущем пункте.
    3. ѕри изменени€ интенсивности цветового канала пересчитывать €ркость и шаг изменени€ каждого канала.
*/


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ColorType_SetBrightness(ColorType *colorType)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Color_SetBrightness(ColorType *colorType, float brightness)
{
    int delta = (int)((brightness + 0.0005f) * 100.0f) - (int)(colorType->brightness * 100.0f);

    if (delta > 0)
    {
        for (int i = 0; i < delta; i++)
        {
            Color_BrightnessChange(colorType, 1);
        }
    }
    else
    {
        for (int i = 0; i < -delta; i++)
        {
            Color_BrightnessChange(colorType, -1);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Color_BrightnessChange(ColorType *colorType, int delta)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Color_Init(ColorType *colorType)
{
    if (!colorType->alreadyUsed)
    {
        colorType->alreadyUsed = true;                  // ѕризнак того, что начальные установки уже произведены

        uint16 colorValue = set.display.colors[colorType->color];

        colorType->red = (float)R_FROM_COLOR(colorValue);
        colorType->green = (float)G_FROM_COLOR(colorValue);
        colorType->blue = (float)B_FROM_COLOR(colorValue);

        ColorType_SetBrightness(colorType);

        if (colorType->red == 0.0f && colorType->green == 0.0f && colorType->blue == 0.0f)
        {
            colorType->stepRed = 0.31f;
            colorType->stepGreen = 0.63f;
            colorType->stepBlue = 0.31f;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Color_ComponentChange(ColorType *colorType, int delta)
{

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char* colorNames[] =
{
    "COLOR_BLACK",
    "COLOR_WHITE",
    "COLOR_GRID",
    "COLOR_DATA_A",
    "COLOR_DATA_B",
    "COLOR_MENU_FIELD",
    "COLOR_MENU_TITLE",
    "COLOM_MENU_TITLE_DARK",
    "COLOR_MENU_TITLE_BRIGHT",
    "COLOR_MENU_ITEM",
    "COLOR_MENU_ITEM_DARK",
    "COLOR_MENU_ITEM_BRIGHT",
    "COLOR_MENU_SHADOW",
    "NUM_COLORS",
    "COLOR_FLASH_10",
    "COLOR_FLASH_01",
    "INVERSE"
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
const char* NameColorFromValue(uint16 colorValue)
{
    for (int i = 0; i < NUM_COLORS; i++)
    {
        if (set.display.colors[(Color)i] == colorValue)
        {
            return colorNames[i];
        }
    }
    return "Sorry, this color is not in the palette";
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
const char* NameColor(Color color)
{
    return colorNames[color];
}
