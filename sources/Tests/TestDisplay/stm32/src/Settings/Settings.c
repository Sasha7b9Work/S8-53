#include "Settings.h"
#include "Display/Display.h"
#include "Display/Colors.h"
#include "Log.h"


#include <string.h>


static const Settings defaultSettings =
{
    // Display
    {
        5,                          // timeShowLevels
        512 - 140,                  // shiftInMemory
        5,                          // timeMessages
        100,                        // brightness
        20,                         // brightnessGrid
        {
            MAKE_COLOR(0, 0, 0),
            MAKE_COLOR(31, 63, 31),
            MAKE_COLOR(15, 31, 15),
            MAKE_COLOR(1, 50, 31),
            MAKE_COLOR(0, 63, 0),
            MAKE_COLOR(26, 54, 26),
            MAKE_COLOR(31, 44, 0),
            MAKE_COLOR(15, 22, 0),
            MAKE_COLOR(31, 63, 0),
            MAKE_COLOR(26, 34, 0),
            MAKE_COLOR(13, 17, 0),
            MAKE_COLOR(31, 51, 0),
            MAKE_COLOR(0, 0, 0),
            MAKE_COLOR(0, 0, 0),
            MAKE_COLOR(1, 40, 25),
            MAKE_COLOR(0, 51, 0)
        },
        ModeDrawSignal_Lines,       // modeDrawSignal
        TypeGrid_1,                 // typeGrid
        NumAccumulation_1,          // numAccumulation
        NumAveraging_1,             // numAveraging
        Averaging_Accurately,       // modeAveraging
        NumMinMax_1,                // numMinMax
        DisplaySmooth_Disable,      // smoothing
        NumSignalsInSec_25,         // num signals in one second
        ChanA,                      // lastAffectedChannel
        ModeAccumulation_NoReset,   // modeAccumulation
        AM_Show,                    // altMarkers
        MenuAutoHide_None,          // menuAutoHide
        true,                       // showFullMemoryWindow
        false                       // showStringNavigation
    }
};

Settings set;

void LoadDefaultColors() 
{
    for(int color = 0; color < NUM_COLORS; color++) 
    {
        set.display.colors[color] = defaultSettings.display.colors[color];
    }
}

void Settings_Load(bool _default)
{

}

void Settings_Save()
{
}

bool Settings_DebugModeEnable()
{
    return true;
}





bool MenuIsShown()
{
    return false;
}

void ShowMenu(bool show)
{
}
