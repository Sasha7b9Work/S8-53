#pragma once 
#include "defines.h"


/// Вызывается один раз при обнаружении новой флешки.
class FM
{
public:
    static void Init();
    
    static void Draw();
    
    static void PressLevelUp();
    
    static void PressLevelDown();
    
    static void RotateRegSet(int angle);
    
    static bool GetNameForNewFile(char name[255]);

    static void PressTab(void);

private:
    
    static bool FileIsExist(char name[255]);

    static void DrawLongString(int x, int y, char *string, bool hightlight);

    static void DrawDirs(int x, int y);

    static void DrawFiles(int x, int y);

    static void DecCurrentDir(void);

    static void DecCurrentFile(void);

    static void DrawHat(int x, int y, char *string, int num1, int num2);

    static void DrawNameCurrentDir(int left, int top);

    static void IncCurrentDir(void);

    static void IncCurrentFile(void);
};
