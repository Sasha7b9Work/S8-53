#include "defines.h"
#include "Globals.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Display.h"
#include "Painter.h"
#include "Colors.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/Timer.h"
#include "Log.h"


#include <math.h>
#include <limits.h>


#include "../Hardware/DisplayHardware.inc"


void Display_Init() 
{
    Painter_ResetFlash();

    InitHardware();

    Painter_LoadPalette();
}

void Display_Update()
{
    Painter_BeginScene(COLOR_FILL);
    Painter_EndScene();
}
