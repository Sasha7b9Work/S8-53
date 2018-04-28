#include "FileManager.h"
#include "Menu/Menu.h"
#include "Pages/PageMemory.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Grid.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/CLOCK.h"
#include "FlashDrive/FlashDrive.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RECS_ON_PAGE 23
#define WIDTH_COL 135


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static char currentDir[255] = "\\";
static int numFirstDir = 0;         // Номер первого выведенного каталога в левой панели. Всего может быть выведено RECS_ON_PAGE каталогов
static int numCurDir = 0;           // Номер подсвеченного каталога
static int numFirstFile = 0;        // Номер первого выведенного файла в правой панели. Всего может быть выведено RECS_ON_PAGE файлов.
static int numCurFile = 0;          // Номер подсвеченного файла
static int numDirs = 0;
static int numFiles = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FM::Init(void)
{
    strcpy(currentDir, "\\");
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DrawLongString(int x, int y, char *string, bool hightlight)
{
    int length = Font_GetLengthText(string);

    Color color = COLOR_FILL;
    if (hightlight)
    {
        Painter::FillRegionC(x - 1, y, WIDTH_COL + 9, 8, color);
        color = COLOR_BACK;
    }

    if (length <= WIDTH_COL)
    {
        Painter::DrawTextC(x, y, string, color);
    }
    else
    {
        Painter::DrawTextWithLimitationC(x, y, string, color, x, y, WIDTH_COL, 10);
        Painter::DrawText(x + WIDTH_COL + 3, y, "...");
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DrawHat(int x, int y, char *string, int num1, int num2)
{
    Painter::FillRegionC(x - 1, y, WIDTH_COL + 9, RECS_ON_PAGE * 9 + 11, COLOR_BACK);
    Painter::DrawFormatText(x + 60, y, COLOR_FILL, string, num1, num2);
    Painter::DrawHLine(y + 10, x + 2, x + 140);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DrawDirs(int x, int y)
{
    FlashDrive::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    DrawHat(x, y, "Каталог : %d/%d", numCurDir + ((numDirs == 0) ? 0 : 1), numDirs);
    char nameDir[255];
    StructForReadDir sfrd;
    y += 12;
    if (FlashDrive::GetNameDir(currentDir, numFirstDir, nameDir, &sfrd))
    {
        int  drawingDirs = 0;
        DrawLongString(x, y, nameDir, CURSORS_IN_DIRS && ( numFirstDir + drawingDirs == numCurDir));
        while (drawingDirs < (RECS_ON_PAGE - 1) && FlashDrive::GetNextNameDir(nameDir, &sfrd))
        {
            drawingDirs++;
            DrawLongString(x, y + drawingDirs * 9, nameDir, CURSORS_IN_DIRS && ( numFirstDir + drawingDirs == numCurDir));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DrawFiles(int x, int y)
{
    DrawHat(x, y, "Файл : %d/%d", numCurFile + ((numFiles == 0) ? 0 : 1), numFiles);
    char nameFile[255];
    StructForReadDir sfrd;
    y += 12;
    if (FlashDrive::GetNameFile(currentDir, numFirstFile, nameFile, &sfrd))
    {
        int drawingFiles = 0;
        DrawLongString(x, y, nameFile, CURSORS_IN_DIRS == 0 && (numFirstFile + drawingFiles == numCurFile));
        while (drawingFiles < (RECS_ON_PAGE - 1) && FlashDrive::GetNextNameFile(nameFile, &sfrd))
        {
            drawingFiles++;
            DrawLongString(x, y + drawingFiles * 9, nameFile, CURSORS_IN_DIRS == 0 && (numFirstFile + drawingFiles == numCurFile));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FM::FileIsExist(char name[255])
{
    char nameFile[255];
    FlashDrive::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    StructForReadDir sfrd;
    if(FlashDrive::GetNameFile(currentDir, 0, nameFile, &sfrd))
    {
        while(FlashDrive::GetNextNameFile(nameFile, &sfrd))
        {
            if(strcmp(name + 2, nameFile) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DrawNameCurrentDir(int left, int top)
{
    Painter::SetColor(COLOR_FILL);
    int length = Font_GetLengthText(currentDir);
    if (length < 277)
    {
        Painter::DrawText(left + 1, top + 1, currentDir);
    }
    else
    {
        char *pointer = currentDir + 2;
        while (length > 277)
        {
            while (*pointer != '\\' && pointer < currentDir + 255)
            {
                pointer++;
            }
            if (pointer >= currentDir + 255)
            {
                return;
            }
            length = Font_GetLengthText(++pointer);
        }
        Painter::DrawText(left + 1, top + 1, pointer);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::Draw(void)
{
    if (NEED_REDRAW_FILEMANAGER == 0)
    {
        return;
    }

    int left = 1;
    int top = 1;
    int width = 297;
    int left2col = width / 2;

    if (NEED_REDRAW_FILEMANAGER == 1)
    {
        Painter::BeginScene(COLOR_BACK);
        Menu::Draw();
        Painter::DrawRectangleC(1, 0, width, 239, COLOR_FILL);
        FlashDrive::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
        DrawNameCurrentDir(left + 1, top + 2);
        Painter::DrawVLineC(left2col, top + 16, 239, COLOR_FILL);
        Painter::DrawHLine(top + 15, 0, width);
    }

    if (NEED_REDRAW_FILEMANAGER != 3)
    {
        DrawDirs(left + 3, top + 18);
    }

    if (NEED_REDRAW_FILEMANAGER != 2)
    {
        DrawFiles(left2col + 3, top + 18);
    }

    Painter::EndScene();

    NEED_REDRAW_FILEMANAGER = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::PressTab(void)
{
    NEED_REDRAW_FILEMANAGER = 1;

    if (CURSORS_IN_DIRS)
    {
        if (numFiles != 0)
        {
            CURSORS_IN_DIRS = 0;
        }
    }
    else
    {
        if (numDirs != 0)
        {
            CURSORS_IN_DIRS = 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::PressLevelDown(void)
{
    NEED_REDRAW_FILEMANAGER = 1;
    if (CURSORS_IN_DIRS == 0)
    {
        return;
    }
    char nameDir[100];
    StructForReadDir sfrd;
    if (FlashDrive::GetNameDir(currentDir, numCurDir, nameDir, &sfrd))
    {
        if (strlen(currentDir) + strlen(nameDir) < 250)
        {
            FlashDrive::CloseCurrentDir(&sfrd);
            strcat(currentDir, "\\");
            strcat(currentDir, nameDir);
            numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
        }

    }
    FlashDrive::CloseCurrentDir(&sfrd);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::PressLevelUp(void)
{
    NEED_REDRAW_FILEMANAGER = 1;
    if (strlen(currentDir) == 1)
    {
        return;
    }
    char *pointer = currentDir + strlen(currentDir);
    while (*pointer != '\\')
    {
        pointer--;
    }
    *pointer = '\0';
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
    CURSORS_IN_DIRS = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::IncCurrentDir(void)
{
    if (numDirs > 1)
    {
        numCurDir++;
        if (numCurDir > numDirs - 1)
        {
            numCurDir = 0;
            numFirstDir = 0;
        }
        if (numCurDir - numFirstDir > RECS_ON_PAGE - 1)
        {
            numFirstDir++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DecCurrentDir(void)
{
    if (numDirs > 1)
    {
        numCurDir--;
        if (numCurDir < 0)
        {
            numCurDir = numDirs - 1;
            LIMITATION(numFirstDir, numDirs - RECS_ON_PAGE, 0, numCurDir);
        }
        if (numCurDir < numFirstDir)
        {
            numFirstDir = numCurDir;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::IncCurrentFile(void)
{
    if (numFiles > 1)
    {
        numCurFile++;
        if (numCurFile > numFiles - 1)
        {
            numCurFile = 0;
            numFirstFile = 0;
        }
        if (numCurFile - numFirstFile > RECS_ON_PAGE - 1)
        {
            numFirstFile++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::DecCurrentFile(void)
{
    if (numFiles > 1)
    {
        numCurFile--;
        if (numCurFile < 0)
        {
            numCurFile = numFiles - 1;
            LIMITATION(numFirstFile, numFiles - RECS_ON_PAGE, 0, numCurFile);
        }
        if (numCurFile < numFirstFile)
        {
            numFirstFile = numCurFile;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FM::RotateRegSet(int angle)
{
    Sound::RegulatorSwitchRotate();
    if (CURSORS_IN_DIRS)
    {
        angle > 0 ? DecCurrentDir() : IncCurrentDir();
        NEED_REDRAW_FILEMANAGER = 2;
    }
    else
    {
        angle > 0 ? DecCurrentFile() : IncCurrentFile();
        NEED_REDRAW_FILEMANAGER = 3;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FM::GetNameForNewFile(char name[255])
{
    char buffer[20];
    int number = 1;

LabelNextNumber:

    strcpy(name, currentDir);
    strcat(name, "\\");

    int size = strlen(FILE_NAME);
    if (size == 0)
    {
        return false;
    }

    if (FILE_NAMING_MODE_IS_HAND)
    {
        LIMITATION(size, size, 1, 95);
        strcat(name, FILE_NAME);
        strcat(name, ".");
        strcat(name, MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");
        return true;
    }
    else
    {
        PackedTime time = Clock::GetPackedTime();
                           //  1          2           3         4           5             6
        uint values[] = {0, time.year, time.month, time.day, time.hours, time.minutes, time.seconds};

        char *ch = FILE_NAME_MASK;
        char *wr = name;

        while (*wr != '\0')
        {
            wr++;
        }

        while (*ch)
        {
            if (*ch >= 0x30)        // Если текстовый символ
            {
                *wr = *ch;          // то записываем его в имя файла
                wr++;
            }
            else
            {
                if (*ch == 0x07)    // Если здесь надо записать порядковый номер
                {
                    strcpy(wr, Int2String(number, false, *(ch + 1), buffer));
                    wr += strlen(buffer);
                    ch++;
                }
                else
                {
                    if (*ch >= 0x01 && *ch <= 0x06)
                    {
                        strcpy(wr, Int2String((int)values[*ch], false, 2, buffer));
                        wr += strlen(buffer);
                    }
                }
            }
            ch++;
        }

        *wr = '.';
        *(wr + 1) = '\0';

        strcat(name, MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");

        if(FileIsExist(name))
        {
            number++;
            goto LabelNextNumber;
        }

        return true;
    }
}
