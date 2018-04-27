#pragma once
#include "Colors.h"
#include "DisplayTypes.h"
#include "PainterC.h"
//#include <ff.h>


class Painter 
{
public:
    static void SendToDisplay(uint8 *bytes, int numBytes);

    static void SendToVCP(uint8 *pointer, int size);

    static void BeginScene(Color color);

    static void EndScene(bool endScene = true);
    // По умолчанию запрашиваем первый фрейм с шрифтами
    static void SendFrame(bool first, bool noFonts = false);

    static void ResetFlash();

    static void SetColor(Color color);

    static Color CurrentColor();

    static void LoadPalette();

    static void SetPalette(Color color);

    static void SetPoint(int x, int y);

    static void DrawHLine(int y, int x0, int x1);

    static void DrawHLineC(int y, int x0, int x1, Color color);

    static void DrawVLine(int x, int y0, int y1);

    static void DrawVLineC(int x, int y0, int y1, Color color);

    static void DrawVPointLine(int x, int y0, int y1, float delta, Color color);

    static void DrawHPointLine(int y, int x0, int x1, float delta);

    static void DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color color);

    static void DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color color);

    static void DrawLine(int x0, int y0, int x1, int y1);

    static void DrawLineC(int x0, int y0, int x1, int y1, Color color);
    /// \brief Рисует прерывистую горизонтальную линию. dFill - длина штриха, dEmpty - расст. между штрихами.
    /// Линия всегда начинается со штриха. dStart указывает смещение первой рисуемой точки относительно начала штриха.
    static void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);
    ///  Рисует прерывистую вертикальную линию.
    static void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    static void DrawRectangle(int x, int y, int width, int height);

    static void DrawRectangleC(int x, int y, int width, int height, Color color);

    static void FillRegion(int x, int y, int width, int height);

    static void FillRegionC(int x, int y, int width, int height, Color color);

    static void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color normal, Color bright, Color dark, bool isPressed, bool inShade);
    /// Установить яркость дисплея.
    static void SetBrightnessDisplay(int16 brightness);

    static uint16 ReduceBrightness(uint16 colorValue, float newBrightness);
    /// Нарисовать массив вертикальных линий. Линии рисуются одна за другой. y0y1 - массив вертикальных координат.
    static void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color color);
    /// modeLines - true - точками, false - точками
    static void DrawSignal(int x, uint8 data[281], bool modeLines);

    static void DrawPicture(int x, int y, int width, int height, uint8 *address);

    static bool SaveScreenToFlashDrive();

    static void SetFont(TypeFont typeFont);
    /// Загрузить шрифта в дисплей.
    static void LoadFont(TypeFont typeFont);

    static int DrawChar(int x, int y, char symbol);

    static int DrawCharC(int x, int y, char symbol, Color color);

    static int DrawText(int x, int y, const char *text);

    static int DrawTextC(int x, int y, const char *text, Color color);
    /// Выводит текст на прямоугольнике цвета colorBackgound
    static int DrawTextOnBackground(int x, int y, const char *text, Color colorBackground);

    static int DrawTextWithLimitationC(int x, int y, const char* text, Color color, int limitX, int limitY, int limitWidth, int limitHeight);
    /// Возвращает нижнюю координату прямоугольника.
    static int DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color colorBackground, Color colorFill);

    static int DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color color);

    static int DrawFormatText(int x, int y, Color color, char *text, ...);

    static int DrawStringInCenterRect(int x, int y, int width, int height, const char *text);

    static int DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color color);
    /// Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра widthBorder цвета colorBackground.
    static void DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color colorText, int widthBorder, Color colorBackground);

    static int DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color colorBackground, Color colorFill);

    static void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

    static void DrawTextInRect(int x, int y, int width, int height, char *text);

    static void DrawTextRelativelyRight(int xRight, int y, const char *text);

    static void DrawTextRelativelyRightC(int xRight, int y, const char *text, Color color);

    static void Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color color1, Color color2);

    static void Draw4SymbolsInRect(int x, int y, char eChar);

    static void Draw4SymbolsInRectC(int x, int y, char eChar, Color color);

    static void Draw10SymbolsInRect(int x, int y, char eChar);
    /// Пишет текст с переносами.
    static int DrawTextInRectWithTransfers(int x, int y, int width, int height, const char *text);

    static void DrawBigText(int x, int y, int size, const char *text);

private:

    static void RunDisplay();

    static void CalculateCurrentColor();

    static Color GetColor(int x, int y);

    static void Get8Points(int x, int y, uint8 buffer[4]);

    static void DrawCharHardCol(int x, int y, char symbol);

    static int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    static int DrawSubString(int x, int y, char *text);

    static int DrawSpaces(int x, int y, char *text, int *numSymbols);

    static int DrawPartWord(char *word, int x, int y, int xRight, bool draw);

    static bool GetHeightTextWithTransfers(int left, int top, int right, const char *text, int *height);
};
