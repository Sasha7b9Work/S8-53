#include "painter.h"
#include "main.h"
#include "Graphics/DisplayDriver.h"
#include "Graphics/mchpGfxDrv.h"


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
void Painter_EndScene(void)
{
    MemoryTrans();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetPixel(SHORT x, SHORT y)
{
    PutPixelN(x, y);
}
