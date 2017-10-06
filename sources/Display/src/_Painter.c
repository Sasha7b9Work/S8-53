#include "Painter.h"
#include "Graphics/mchpGfxDrv.h"
#include "main.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Painter_SetPalette(BYTE numColor, SHORT valueColor)
{
    if (IsPaletteEnabled() != 0)
    {
        DisablePalette();
    }

    SetPaletteBpp(COLOR_DEPTH);
    mSetPaletteColor(valueColor, numColor); //_VMRGN
    EnablePalette();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetColor(BYTE numColor)
{
    SetColor(numColor);
    SetColorN();
    curColor = numColor;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_FillRegion(SHORT x, SHORT y, SHORT width, SHORT height)
{
    FillRectN(x, y, width, height);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawText(SHORT x, SHORT y, UXCHAR *text)
{
    TextOutN(x, y, text, (WORD)(curFont));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_EndScene(void)
{
    MemoryTrans();
}
