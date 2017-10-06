#include "Hardware/Hardware.h"
#include "Display/Display.h"


int main()
{
    Hardware_Init();
    Display_Init();
    
    while(1)
    {
        Display_Update();
    }
}
