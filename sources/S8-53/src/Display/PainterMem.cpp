

#include "PainterMem.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8 *buffer;
static int width;
volatile static int height;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8* FindByte(int x, int y)
{
    return buffer + (y * width + x) / 2;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 GetClearMask(int x, int y)
{
    if (((y * width + x) % 2) == 0)
    {
        return 0xf0;
    }
    return 0x0f;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 GetMask(int x, int y, int color)
{
    if (((y * width + x) % 2) == 0)
    {
        return (uint8)(color & 0xf);
    }
    return (uint8)((color << 4) & 0xf0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_SetBuffer(uint8 *address, int eWidth, int eHeight)
{
    buffer = address;
    width = eWidth;
    height = eHeight;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_SetPoint(int x, int y, int color)
{
    uint8 *byte = FindByte(x, y);

    uint8 clearMask = GetClearMask(x, y);

    *byte &= clearMask;

    uint8 drawMask = GetMask(x, y, color);

    *byte |= drawMask;

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_FillRect(int x, int y, int _width, int _height, int color)
{
    for(int i = 0; i <= _height; i++)
    {
        PainterMem_DrawHLine(y + i, x, x + _width, color);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_DrawVLine(int x, int y0, int y1, int color)
{
    for (int y = y0; y <= y1; y++)
    {
        PainterMem_SetPoint(x, y, color);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_DrawHLine(int y, int x0, int x1, int color)
{
    for(int x = x0; x <= x1; x++)
    {
        PainterMem_SetPoint(x, y, color);
    }
    /*
    uint8 *byte = FindByte(x0, y);
    uint8 clearMask = GetClearMask(x0, y);
    uint8 drawMask = GetMask(x0, y, color);
    int x = x0;

    do
    {
        *byte &= clearMask;
        *byte |= drawMask;
        if(clearMask & 0xf0)
        {
            byte++;
        }
        clearMask = (clearMask == 0xf0) ? 0x0f : 0xf0;

        if ((drawMask & 0x0f) == 0)
        {
            drawMask = (drawMask << 4);
        }
        else
        {
            drawMask = (drawMask >> 4);
        }
        x++;
    } while (x <= x1);
    */
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PainterMem_DrawRectangle(int x, int y, int _width, int _height, int color)
{
    PainterMem_DrawVLine(x, y, y + _height, color);
    PainterMem_DrawVLine(x + _width, y, y + _height, color);
    PainterMem_DrawHLine(y, x, x + _width, color);
    PainterMem_DrawHLine(y + _height, x, x + _width, color);
}
