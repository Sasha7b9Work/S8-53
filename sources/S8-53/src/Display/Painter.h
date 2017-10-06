#pragma once
#include "Colors.h"
#include "DisplayTypes.h"
#include "PainterC.h"


class Painter 
{
public:
    void SendToDisplay(uint8 *bytes, int numBytes);

    void SendToVCP(uint8 *pointer, int size);

    void BeginScene(Color color);

    void EndScene();

    void SendFrame(bool first);

    void ResetFlash();

    void SetColor(Color color);

    Color CurrentColor();

    void LoadPalette();

    void SetPalette(Color color);

    void SetPoint(int x, int y);

    void DrawHLine(int y, int x0, int x1);

    void DrawHLineC(int y, int x0, int x1, Color color);

    void DrawVLine(int x, int y0, int y1);

    void DrawVLineC(int x, int y0, int y1, Color color);

    void DrawVPointLine(int x, int y0, int y1, float delta, Color color);

    void DrawHPointLine(int y, int x0, int x1, float delta);

    void DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color color);

    void DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color color);

    void DrawLine(int x0, int y0, int x1, int y1);

    void DrawLineC(int x0, int y0, int x1, int y1, Color color);
    /// \brief Рисует прерывистую горизонтальную линию. dFill - длина штриха, dEmpty - расст. между штрихами.
    /// Линия всегда начинается со штриха. dStart указывает смещение первой рисуемой точки относительно начала штриха.
    void DrawDashedHLine(int y, int x0, int x1, int dFill, int dEmpty, int dStart);
    ///  Рисует прерывистую вертикальную линию.
    void DrawDashedVLine(int x, int y0, int y1, int dFill, int dEmpty, int dStart);

    void DrawRectangle(int x, int y, int width, int height);

    void DrawRectangleC(int x, int y, int width, int height, Color color);

    void FillRegion(int x, int y, int width, int height);

    void FillRegionC(int x, int y, int width, int height, Color color);

    void DrawVolumeButton(int x, int y, int width, int height, int thickness, Color normal, Color bright, Color dark, bool isPressed, bool inShade);
    /// Установить яркость дисплея.
    void SetBrightnessDisplay(int16 brightness);

    uint16 ReduceBrightness(uint16 colorValue, float newBrightness);
    /// Нарисовать массив вертикальных линий. Линии рисуются одна за другой. y0y1 - массив вертикальных координат.
    void DrawVLineArray(int x, int numLines, uint8 *y0y1, Color color);
    /// modeLines - true - точками, false - точками
    void DrawSignal(int x, uint8 data[281], bool modeLines);

    void DrawPicture(int x, int y, int width, int height, uint8 *address);

#if _USE_LFN > 0
    void SaveScreenToFlashDrive(TCHAR *fileName);
#else
    bool SaveScreenToFlashDrive();
#endif
    void SetFont(TypeFont typeFont);
    /// Загрузить шрифта в дисплей.
    void LoadFont(TypeFont typeFont);

    int DrawChar(int x, int y, char symbol);

    int DrawCharC(int x, int y, char symbol, Color color);

    int DrawText(int x, int y, const char *text);

    int DrawTextC(int x, int y, const char *text, Color color);
    /// Выводит текст на прямоугольнике цвета colorBackgound
    int DrawTextOnBackground(int x, int y, const char *text, Color colorBackground);

    int DrawTextWithLimitationC(int x, int y, const char* text, Color color, int limitX, int limitY, int limitWidth, int limitHeight);
    /// Возвращает нижнюю координату прямоугольника.
    int DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color colorBackground, Color colorFill);

    int DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color color);

    int DrawFormatText(int x, int y, Color color, char *text, ...);

    int DrawStringInCenterRect(int x, int y, int width, int height, const char *text);

    int DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color color);
    /// Пишет строку текста в центре области(x, y, width, height)цветом ColorText на прямоугольнике с шириной бордюра widthBorder цвета colorBackground.
    void DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color colorText, int widthBorder, Color colorBackground);

    int DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color colorBackground, Color colorFill);

    void DrawHintsForSmallButton(int x, int y, int width, void *smallButton);

    void DrawTextInRect(int x, int y, int width, int height, char *text);

    void DrawTextRelativelyRight(int xRight, int y, const char *text);

    void DrawTextRelativelyRightC(int xRight, int y, const char *text, Color color);

    void Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color color1, Color color2);

    void Draw4SymbolsInRect(int x, int y, char eChar);

    void Draw4SymbolsInRectC(int x, int y, char eChar, Color color);

    void Draw10SymbolsInRect(int x, int y, char eChar);
    /// Пишет текст с переносами.
    int DrawTextInRectWithTransfers(int x, int y, int width, int height, const char *text);

    void DrawBigText(int x, int y, int size, const char *text);

private:

    void RunDisplay();

    void CalculateCurrentColor();

    Color GetColor(int x, int y);

    void Get8Points(int x, int y, uint8 buffer[4]);

    void DrawCharHardCol(int x, int y, char symbol);

    int DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight);

    int DrawSubString(int x, int y, char *text);

    int DrawSpaces(int x, int y, char *text, int *numSymbols);

    int DrawPartWord(char *word, int x, int y, int xRight, bool draw);

    bool GetHeightTextWithTransfers(int left, int top, int right, const char *text, int *height);
};

extern Painter painter;
