#include "defines.h"
#include "main.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Display/WelcomeScreen.h" 
#include "Ethernet/Ethernet.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "FPGA/DataStorage.h"
#include "Hardware/Hardware.h"
#include "Hardware/Sound.h"
#include "Hardware/FLASH.h"
#include "Menu/Pages/PageCursors.h"
#include "Menu/Menu.h"
#include "Panel/Panel.h"
#include "VCP/VCP.h"
#include "Settings/Settings.h"
#include "Utils/ProcessingSignal.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FuncOfDraw(void) 
{
    painter.BeginScene(COLOR_FILL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static void FuncConnect(int _id);
//static void FuncReciever(int _id, const char *_buffer, uint _length);
static void ProcessingSignal();
static void Init();

#define TICS ((gTimerTics - time) / 120.0f)

extern void main3();

//----------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
    //main3();
    Hardware_Init();
    Init();
    Settings_Load(false);
    fpga.Init();    
    HAL_Delay(250);
    fpga.OnPressStartStop();
    Ethernet_Init();
    display.Init();
    if (gBF.tuneTime == 1)
    {
        //menu.OpenItemTime();
    }

    while(1)
    {
        Timer_StartMultiMeasurement();      // Сброс таймера для замера длительности временных интервалов в течение одной итерации цикла.
        
        flashDrive.Update();

        Ethernet_Update(0);

        fpga.Update();                      // Обновляем аппаратную часть.

        ProcessingSignal();

        if (!WelcomeScreen_Run())
        {
            display.Update();               // Рисуем экран.
        }

        menu.UpdateInput();                 // Обновляем состояние меню
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessingSignal() 
{
    uint8 **data0 = &gData0;
    uint8 **data1 = &gData1;
    DataSettings **ds = &gDSet;

    int first = 0;
    int last = 0;
    sDisplay_PointsOnDisplay(&first, &last);

    if (MODE_WORK_IS_DIRECT)
    {
        dataStorage.GetDataFromEnd(0, &gDSet, &gData0, &gData1);
        if (sDisplay_NumAverage() != 1 || sTime_RandomizeModeEnabled())
        {
            gData0 = dataStorage.GetAverageData(A);
            gData1 = dataStorage.GetAverageData(B);
        }
    }
    else if (MODE_WORK_IS_LATEST)
    {
        data0 = &gData0memLast;
        data1 = &gData1memLast;
        ds = &gDSmemLast;
        dataStorage.GetDataFromEnd(gMemory.currentNumLatestSignal, &gDSmemLast, &gData0memLast, &gData1memLast);
    }
    else if (MODE_WORK_IS_MEMINT)
    {
        data0 = &gData0memInt;
        data1 = &gData1memInt;
        ds = &gDSmemInt;
        FLASH_GetData(gMemory.currentNumIntSignal, &gDSmemInt, &gData0memInt, &gData1memInt);
    }

    if (MODE_WORK_IS_MEMINT)
    { 
        if (!MODE_SHOW_MEMINT_IS_SAVED)
        {
            Processing_SetSignal(gData0, gData1, gDSet, first, last);
        }
    }
    else
    {
        Processing_SetSignal(*data0, *data1, *ds, first, last);
    }

    Cursors_Update();    // В случае, если находимся в режиме курсорных измерений, обновляем их положение, если нужно.
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Init() 
{
    VCP_Init();
}

/*
void FuncConnect(int _id) {
    static char message[100];
    message[0] = '\0'; 
    strcpy(message, "Connection ");
    IntToStrCat(message, _id);
    strcat(message, " is establishes. All ");
    IntToStrCat(message, TCP_SOCKET_AllConnections());
    strcat(message, " connections");
    TCP_SOCKET_SendToOne(_id, message, strlen(message));
}
*/

/*
void FuncReciever(int _id, const char *eBuffer, uint length) {
    TCP_SOCKET_SendToOne(_id, eBuffer, length);
}
*/
