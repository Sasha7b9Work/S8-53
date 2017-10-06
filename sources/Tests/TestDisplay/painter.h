#ifndef _PAINTER_H_
#define _PAINTER_H_


#include "GenericTypeDefs.h"


void Painter_SetColor(BYTE numColor);
void Painter_FillRegion(SHORT x, SHORT y, SHORT width, SHORT height);
void Painter_EndScene(void);
void Painter_SetPixel(SHORT x, SHORT y);


#endif
