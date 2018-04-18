

#pragma once

#define Painter_SetPointC(x, y, color)          Painter_SetColor(color); Painter_SetPoint(x, y);

#define Painter_DrawLineC(x0, y0, x1, y1, color)                            Painter_SetColor(color);Painter_DrawLine(x0, y0, x1, y1);
#define Painter_DrawDashedHLineC(y, x0, x1, dFill, dEmpty, dStart, color);  Painter_SetColor(color); Painter_DrawDashedHLine(y, x0, x1, dFill, dEmpty, dStart);
#define Painter_DrawDashedVLineC(x, y0, y1, dFill, dEmpty, dStart, color);  Painter_SetColor(color); Painter_DrawDashedVLine(x, y0, y1, dFill, dEmpty, dStart);

#define Painter_Draw4SymbolsInRectC(x, y, eChar, color)             Painter_SetColor(color); Painter_Draw4SymbolsInRect(x, y, eChar);
#define Painter_DrawTextInRectC(x, y, width, height, text, color)   Painter_SetColor(color); Painter_DrawTextInRect(x, y, width, height, text);
#define Painter_DrawTextRelativelyRightC(x, y, text, color)         Painter_SetColor(color); Painter_DrawTextRelativelyRight(x, y, text);
