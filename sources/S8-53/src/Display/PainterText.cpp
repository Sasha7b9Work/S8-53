#include "defines.h"
#include "Log.h"
#include "Painter.h"
#include "Font/font.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static TypeFont currentTypeFont = TypeFont_None;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Painter::SetFont(TypeFont typeFont)
{
    if (typeFont == currentTypeFont)
    {
        return;
    }
    font = fonts[typeFont];
    uint8 command[4];
    command[0] = SET_FONT;
    command[1] = (uint8)typeFont;
    Painter::SendToDisplay(command, 4);
    Painter::SendToVCP(command, 2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::LoadFont(TypeFont typeFont)
{
    uint8 *pFont = (uint8 *)fonts[typeFont];

    uint8 command[2 + 4];
    command[0] = LOAD_FONT;
    command[1] = typeFont;
    *(uint*)(command + 2) = (uint)fonts[typeFont]->height;
    Painter::SendToVCP(command, 2 + 4);

    //Painter::SendToVCP(pFont + 4, sizeof(Font) - 4);

    pFont += 4;

    for(int i = 0; i < 256; i++)
    {
        Painter::SendToVCP(pFont, sizeof(Symbol));
        pFont += sizeof(Symbol);
        Timer::PauseOnTicks(10000);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool ByteFontNotEmpty(int eChar, int byte)
{
    static const uint8 *bytes = 0;
    static int prevChar = -1;
    if (eChar != prevChar)
    {
        prevChar = eChar;
        bytes = font->symbol[prevChar].bytes;
    }
    return bytes[byte];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool BitInFontIsExist(int eChar, int numByte, int bit)
{
    static uint8 prevByte = 0;      // WARN ????? ????? ??????? ??????
    static int prevChar = -1;
    static int prevNumByte = -1;
    if (prevNumByte != numByte || prevChar != eChar)
    {
        prevByte = font->symbol[eChar].bytes[numByte];
        prevChar = eChar;
        prevNumByte = numByte;
    }
    return prevByte & (1 << bit);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawCharInColorDisplay(int eX, int eY, uchar symbol)
{
    int8 width = (int8)font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    Painter::SetPoint(x, y);
                }
                x++;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int Painter_DrawBigChar(int eX, int eY, int size, char symbol)
{
    int8 width = (int8)font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b * size + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    for (int i = 0; i < size; i++)
                    {
                        for (int j = 0; j < size; j++)
                        {
                            Painter::SetPoint(x + i, y + j);
                        }
                    }
                }
                x += size;
            }
        }
    }

    return eX + width * size;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawCharHardCol(int x, int y, char symbol)
{
    char str[2] = {symbol, 0};
    DrawText(x, y, str);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
extern void CalculateCurrentColor();

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawChar(int x, int y, char symbol)
{
    CalculateCurrentColor();
    if (Font_GetSize() == 5)
    {
        DrawCharHardCol(x, y + 3, symbol);
    }
    else if (Font_GetSize() == 8)
    {
        DrawCharHardCol(x, y, symbol);
    }
    else
    {
        DrawCharInColorDisplay(x, y, (uint8)symbol);
    }
    return x + Font_GetLengthSymbol((uint8)symbol);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawCharC(int x, int y, char symbol, Color color)
{
    Painter::SetColor(color);
    return DrawChar(x, y, symbol);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawText(int x, int y, const char *text)
{
#undef SIZE_BUFFER
#define SIZE_BUFFER 100

    if (*text == 0)
    {
        return x;
    }
    CalculateCurrentColor();

    int retValue = x;
    y += (8 - Font_GetSize());
    uint8 command[SIZE_BUFFER];
    command[0] = DRAW_TEXT;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)(y + 1);
    uint8 *pointer = command + 5;
    uint8 length = 0;

    int counter = 0;
    while (*text && length < (SIZE_BUFFER - 7))
    {
        *pointer = (uint8)(*text);
        retValue += Font_GetLengthSymbol((uint8)*text);
        text++;
        pointer++;
        length++;
        counter++;
    }

    if (*text != 0)
    {
        LOG_WRITE("big string - %s", text);
    }

    *pointer = 0;
    *(command + 4) = length;
    int numBytes = ((length + 4) / 4) * 4 + 4;
    Painter::SendToDisplay(command, numBytes);
    Painter::SendToVCP(command, 1 + 2 + 1 + 1 + length);
    return retValue;
#undef SIZE_BUFFER

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextC(int x, int y, const char *text, Color color)
{
    SetColor(color);
    return Painter::DrawText(x, y, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextOnBackground(int x, int y, const char *text, Color colorBackground)
{
    int width = Font_GetLengthText(text);
    int height = Font_GetSize();

    Color colorText = Painter::CurrentColor();
    FillRegionC(x - 1, y, width, height, colorBackground);
    SetColor(colorText);

    return DrawText(x, y, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawCharWithLimitation(int eX, int eY, uchar symbol, int limitX, int limitY, int limitWidth, int limitHeight)
{
    int8 width = (int8)font->symbol[symbol].width;
    int8 height = (int8)font->height;

    for (int b = 0; b < height; b++)
    {
        if (ByteFontNotEmpty(symbol, b))
        {
            int x = eX;
            int y = eY + b + 9 - height;
            int endBit = 8 - width;
            for (int bit = 7; bit >= endBit; bit--)
            {
                if (BitInFontIsExist(symbol, b, bit))
                {
                    if ((x >= limitX) && (x <= (limitX + limitWidth)) && (y >= limitY) && (y <= limitY + limitHeight))
                    {
                        SetPoint(x, y);
                    }
                }
                x++;
            }
        }
    }

    return eX + width + 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextWithLimitationC(int x, int y, const char* text, Color color, int limitX, int limitY, int limitWidth, int limitHeight)
{
    SetColor(color);
    int retValue = x;
    while (*text)
    {
        x = DrawCharWithLimitation(x, y, (uint8)*text, limitX, limitY, limitWidth, limitHeight);
        retValue += Font_GetLengthSymbol((uint8)*text);
        text++;
    }
    return retValue + 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsLetter(char symbol)
{
    static const bool isLetter[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
        true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
    };

    return isLetter[(uint8)symbol];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static char *GetWord(const char *firstSymbol, int *length, char buffer[20])
{
    int pointer = 0;
    *length = 0;

    while (IsLetter(*firstSymbol))
    {
        buffer[pointer] = *firstSymbol;
        pointer++;
        firstSymbol++;
        (*length)++;
    }
    buffer[pointer] = '\0';

    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsConsonant(char symbol)
{
    static const bool isConsonat[256] =
    {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false,
        false, true, true, true, true, false, true, true, false, false, true, true, true, true, false, true,
        true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false
    };

    return isConsonat[(uint8)symbol];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool CompareArrays(const bool *array1, const bool *array2, int numElems)
{
    for (int i = 0; i < numElems; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// ??????? ????????? ???????. C letters ?????????? ????? ?????, ??? ????? ????? ???????, ? lettersInSyllable ????? ???????? ????? ???? ? ????????? 
// ?????. ???? ????? ???????????, ??????? ?????????? false
static bool FindNextTransfer(char *letters, int8 *lettersInSyllable)
{

#define VOWEL       0   // ???????
#define CONSONANT   1   // ?????????

    *lettersInSyllable = (int8)strlen(letters);
    if (strlen(letters) <= 3)
    {
        return false;
    }

    static const bool template1[3] = {false, true, true};               //     011     2   // ????? ??????? ??????? ???????
    static const bool template2[4] = {true, false, true, false};        //     1010    2
    static const bool template3[4] = {false, true, false, true};        //     0101    3
    static const bool template4[4] = {true, false, true, true};         //     1011    3
    static const bool template5[4] = {false, true, false, false};       //     0100    3
    static const bool template6[4] = {true, false, true, true};         //     1011    3
    static const bool template7[5] = {true, true, false, true, false};  //     11010   3
    static const bool template8[6] = {true, true, false, true, true};   //     11011   4

    bool consonant[20];

    int size = strlen(letters);
    for (int i = 0; i < size; i++)
    {
        consonant[i] = IsConsonant(letters[i]);
    }

    if (CompareArrays(template1, consonant, 3))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (CompareArrays(template2, consonant, 4))
    {
        *lettersInSyllable = 2;
        return true;
    }
    if (strlen(letters) < 5)
    {
        return false;
    }
    if (CompareArrays(template3, consonant, 4) || CompareArrays(template4, consonant, 4) || CompareArrays(template5, consonant, 4) || CompareArrays(template6, consonant, 4))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (strlen(letters) < 6)
    {
        return false;
    }
    if (CompareArrays(template7, consonant, 5))
    {
        *lettersInSyllable = 3;
        return true;
    }
    if (CompareArrays(template8, consonant, 5))
    {
        *lettersInSyllable = 4;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int8* BreakWord(char *word)
{
    int num = 0;
    static int8 lengthSyllables[10];
    char *position = word;
    while (FindNextTransfer(position, &(lengthSyllables[num])))
    {
        position += lengthSyllables[num];
        num++;
    }
    lengthSyllables[num + 1] = 0;
    if (strcmp(word, "?????????") == 0)
    {
        int8 lengths[] = {5, 2, 2, 0};
        memcpy(lengthSyllables, lengths, 4);
    }
    else if (strcmp(word, "???????????????") == 0)
    {
        int8 lenghts[] = {4, 3, 4, 5, 3, 0};
        memcpy(lengthSyllables, lenghts, 6);
    }
    return lengthSyllables;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// ?????????? ????? ????? ?? ????? numSyllable(???????????) ?????? ?? ?????? ????????
static char* PartWordForTransfer(char *word, int8* lengthSyllables, int numSyllables, int numSyllable, char buffer[30])
{
    int length = 0;
    for (int i = 0; i <= numSyllable; i++)
    {
        length += lengthSyllables[i];
    }
    memcpy((void*)buffer, (void*)word, length);
    buffer[length] = '-';
    buffer[length + 1] = '\0';
    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// ???? draw == false, ?? ???????? ?????? ?? ????, ?????? ???????????? ?????? ??? ??????????
int Painter::DrawPartWord(char *word, int x, int y, int xRight, bool draw)
{
    int8 *lengthSyllables = BreakWord(word);
    int numSyllabels = 0;
    char buffer[30];
    for (int i = 0; i < 10; i++)
    {
        if (lengthSyllables[i] == 0)
        {
            numSyllabels = i;
            break;
        }
    }

    for (int i = numSyllabels - 2; i >= 0; i--)
    {
        char *subString = PartWordForTransfer(word, lengthSyllables, numSyllabels, i, buffer);
        int length = Font_GetLengthText(subString);
        if (xRight - x > length - 5)
        {
            if (draw)
            {
                DrawText(x, y, subString);
            }
            return strlen(subString) - 1;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextInRectWithTransfers(int eX, int eY, int eWidth, int eHeight, const char *text)
{
    int top = eY;
    int left = eX;
    int right = eX + eWidth;
    int bottom = eY + eHeight;

    char buffer[20];
    int numSymb = strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < bottom && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            char *word = GetWord(text + curSymbol, &length, buffer);

            if (length <= 1)                            // ??? ????????? ???????? ??? ????, ?.?. ????? ?? ???????
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x = DrawChar(x, y, symbol);
            }
            else                                            // ? ????? ??????? ?? ??????? ???? ??? ????????? ???????, ?.?. ??????? ?????
            {
                int lengthString = Font_GetLengthText(word);
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word, x, y, right, true);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x = DrawText(x, y, word);
                }
            }
        }
        x = left;
        y += 9;
    }

    return y;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// ?????????? ?????? ??????, ??????? ?????? ????? text, ??? ?????? ?? left ?? right ? ?????????? height. ???? bool == false, ?? ????? ?? ?????? ?? ????? 
bool Painter::GetHeightTextWithTransfers(int left, int top, int right, const char *text, int *height)
{
    char buffer[20];
    int numSymb = strlen(text);

    int y = top - 1;
    int x = left;

    int curSymbol = 0;

    while (y < 231 && curSymbol < numSymb)
    {
        while (x < right - 1 && curSymbol < numSymb)
        {
            int length = 0;
            char *word = GetWord(text + curSymbol, &length, buffer);

            if (length <= 1)                            // ??? ????????? ???????? ??? ????, ?.?. ????? ?? ???????
            {
                char symbol = text[curSymbol++];
                if (symbol == '\n')
                {
                    x = right;
                    continue;
                }
                if (symbol == ' ' && x == left)
                {
                    continue;
                }
                x += Font_GetLengthSymbol((uint8)symbol);
            }
            else                                            // ? ????? ??????? ?? ??????? ???? ??? ????????? ???????, ?.?. ??????? ?????
            {
                int lengthString = Font_GetLengthText(word);
                if (x + lengthString > right + 5)
                {
                    int numSymbols = DrawPartWord(word, x, y, right, false);
                    x = right;
                    curSymbol += numSymbols;
                    continue;
                }
                else
                {
                    curSymbol += length;
                    x += Font_GetLengthText(word);
                }
            }
        }
        x = left;
        y += 9;
    }

    LIMITATION(*height, y - top + 4, 0, 239);

    return curSymbol == numSymb;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextInBoundedRectWithTransfers(int x, int y, int width, const char *text, Color colorBackground, Color colorFill)
{
    int height = 0;
    GetHeightTextWithTransfers(x + 3, y + 3, x + width - 8, text, &height);

    DrawRectangleC(x, y, width, height, colorFill);
    FillRegionC(x + 1, y + 1, width - 2, height - 2, colorBackground);
    DrawTextInRectWithTransfersC(x + 3, y + 3, width - 8, height, text, colorFill);
    return y + height;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawTextInRectWithTransfersC(int x, int y, int width, int height, const char *text, Color color)
{
    SetColor(color);
    return DrawTextInRectWithTransfers(x, y, width, height, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawFormatText(int x, int y, Color color, char *text, ...)
{
#undef SIZE_BUFFER
#define SIZE_BUFFER 200
    char buffer[SIZE_BUFFER];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);
    return DrawTextC(x, y, buffer, color);
#undef SIZE_BUFFER
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawStringInCenterRect(int eX, int eY, int width, int eHeight, const char *text)
{
    int lenght = Font_GetLengthText(text);
    int height = Font_GetHeightSymbol(text[0]);
    int x = eX + (width - lenght) / 2;
    int y = eY + (eHeight - height) / 2;
    return DrawText(x, y, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawStringInCenterRectC(int x, int y, int width, int height, const char *text, Color color)
{
    SetColor(color);
    return DrawStringInCenterRect(x, y, width, height, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawStringInCenterRectOnBackgroundC(int x, int y, int width, int height, const char *text, Color colorText, int widthBorder, 
                                                 Color colorBackground)
{
    int lenght = Font_GetLengthText(text);
    int eX = DrawStringInCenterRectC(x, y, width, height, text, colorBackground);
    int w = lenght + widthBorder * 2 - 2;
    int h = 7 + widthBorder * 2 - 1;
    FillRegion(eX - lenght - widthBorder, y - widthBorder + 1, w, h);
    DrawStringInCenterRectC(x, y, width, height, text, colorText);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawStringInCenterRectAndBoundItC(int x, int y, int width, int height, const char *text, Color colorBackground, Color colorFill)
{
    DrawRectangleC(x, y, width, height, colorFill);
    FillRegionC(x + 1, y + 1, width - 2, height - 2, colorBackground);
    SetColor(colorFill);
    return DrawStringInCenterRect(x, y, width, height, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawHintsForSmallButton(int x, int y, int width, void *smallButton)
{
    SmallButton *sb = (SmallButton*)smallButton;
    FillRegionC(x, y, width, 239 - y, COLOR_BACK);
    DrawRectangleC(x, y, width, 239 - y, COLOR_FILL);
    const StructHelpSmallButton *structHelp = &(*sb->hintUGO)[0];
    x += 3;
    y += 3;
    while (structHelp->funcDrawUGO)
    {
        DrawRectangle(x, y, WIDTH_SB, WIDTH_SB);
        structHelp->funcDrawUGO(x, y);
        int yNew = DrawTextInRectWithTransfers(x + 23, y + 1, width - 30, 20, structHelp->helpUGO[set.common.lang]);
        y = ((yNew - y) < 22) ? (y + 22) : yNew;
        structHelp++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static int GetLenghtSubString(char *text)
{
    int retValue = 0;
    while (((*text) != ' ') && ((*text) != '\0'))
    {
        retValue += Font_GetLengthSymbol((uint8)*text);
        text++;
    }
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawSubString(int x, int y, char *text)
{
    int numSymbols = 0;
    while (((*text) != ' ') && ((*text) != '\0'))
    {
        x = DrawChar(x, y, *text);
        numSymbols++;
        text++;
    }
    return numSymbols;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Painter::DrawSpaces(int x, int y, char *text, int *numSymbols)
{
    *numSymbols = 0;
    while (*text == ' ')
    {
        x = DrawChar(x, y, *text);
        text++;
        (*numSymbols)++;
    }
    return x;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawTextInRect(int x, int y, int width, int height, char *text)
{
    int xStart = x;
    int xEnd = xStart + width;

    while (*text != 0)
    {
        int length = GetLenghtSubString(text);
        if (length + x > xEnd)
        {
            x = xStart;
            y += Font_GetHeightSymbol(*text);
        }
        int numSymbols = 0;
        numSymbols = DrawSubString(x, y, text);
        text += numSymbols;
        x += length;
        x = DrawSpaces(x, y, text, &numSymbols);
        text += numSymbols;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawTextRelativelyRight(int xRight, int y, const char *text)
{
    int lenght = Font_GetLengthText(text);
    DrawText(xRight - lenght, y, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawTextRelativelyRightC(int xRight, int y, const char *text, Color color)
{
    SetColor(color);
    DrawTextRelativelyRight(xRight, y, text);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::Draw2SymbolsC(int x, int y, char symbol1, char symbol2, Color color1, Color color2)
{
    DrawCharC(x, y, symbol1, color1);
    DrawCharC(x, y, symbol2, color2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::Draw4SymbolsInRect(int x, int y, char eChar)
{
    for (int i = 0; i < 2; i++)
    {
        DrawChar(x + 8 * i, y, eChar + i);
        DrawChar(x + 8 * i, y + 8, eChar + i + 16);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::Draw4SymbolsInRectC(int x, int y, char eChar, Color color)
{
    SetColor(color);
    Draw4SymbolsInRect(x, y, eChar);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::Draw10SymbolsInRect(int x, int y, char eChar)
{
    for (int i = 0; i < 5; i++)
    {
        DrawChar(x + 8 * i, y, eChar + i);
        DrawChar(x + 8 * i, y + 8, eChar + i + 16);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Painter::DrawBigText(int eX, int eY, int size, const char *text)
{
    int numSymbols = strlen(text);

    int x = eX;

    for (int i = 0; i < numSymbols; i++)
    {
        x = Painter_DrawBigChar(x, eY, size, text[i]);
        x += size;
    }
}
