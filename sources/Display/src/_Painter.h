#pragma once
#include "GenericTypeDefs.h"
#include "Graphics/Primitive.h"


/// Назначает цвет valueColor цвету номер numColor
void Painter_SetPalette(BYTE numColor, SHORT valueColor);
/// Устанавливает текущий цвет рисования
void Painter_SetColor(BYTE numColor);
/// Залить прямоугольную область текщим цветом
void Painter_FillRegion(SHORT x, SHORT y, SHORT width, SHORT height);
/// Вывести строку текста
void Painter_DrawText(SHORT x, SHORT y, UXCHAR *text);
///
void Painter_EndScene(void);
