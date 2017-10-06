#include "Controls.h"


static const char* namesButtons[] = 
{
    "B_Empty",
    "B_Channel0",
    "B_Service",
    "B_Channel1",
    "B_Display",
    "B_Time",
    "B_Memory",
    "B_Sinchro",
    "B_Start",
    "B_Cursors",
    "B_Measures",
    "B_Power",
    "B_Info",
    "B_Menu",
    "B_F1",
    "B_F2",
    "B_F3",
    "B_F4",
    "B_F5"
};


const char* NameButton(PanelButton button) 
{
    return namesButtons[button];
}
