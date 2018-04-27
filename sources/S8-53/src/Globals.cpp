#include "defines.h"
#include "Globals.h"
#include "Display/DisplayTypes.h"

void *extraMEM = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HCD_HandleTypeDef handleHCD;

USBH_HandleTypeDef handleUSBH;

SPI_HandleTypeDef handleSPI =
{
    SPI1,
    {
        SPI_MODE_SLAVE,                 // Init.Mode
        SPI_DIRECTION_2LINES,           // Init.Direction
        SPI_DATASIZE_8BIT,              // Init.DataSize
        SPI_POLARITY_HIGH,              // Init.CLKPolarity
        SPI_PHASE_1EDGE,                // Init.CLKPhase
        SPI_NSS_SOFT,                   // Init.NSS
        SPI_BAUDRATEPRESCALER_2,        // Init.BaudRatePrescaler
        SPI_FIRSTBIT_MSB,               // Init.FirstBit
        SPI_TIMODE_DISABLED,            // Init.TIMode
        SPI_CRCCALCULATION_DISABLED,    // Init.CRCCalculation
        7                               // InitCRCPolynomial
    }
};


ADC_HandleTypeDef handleADC;


DAC_HandleTypeDef handleDAC =
{
    DAC
};


BitField gBF =
{
    0,  // ethTimeLastEthifInput
    0,  // cableEthIsConnected
    0,  // ethIsConnected

    0,  // showLevelRShift0
    0,  // showLevelRShift1
    0,  // showLevelTrigLev
    0,  // trigEnable
    0,  // drawRShiftMarkers
    1,  // needFinishDraw
    0,  // framesElapsed
    0,  // numDrawingSignals

  
    // FPGA
    0,  // FPGAtrigAutoFind
    0,  // FPGAautoFindInProgress
    0,  // FPGAtemporaryPause
    0,  // FPGAinProcessingOfRead
    1,  // FPGAcanReadData
    0,  // FPGAcritiacalSituation
    0,  // FPGAfirstAfterWrite

    0,  // soundIsBeep
    0,  // soundWarnIsBeep
    0,  // buttonIsPressed
    
    1,  // panelIsRunning
    
    // Governor
    0,
    0,
    0,
    0,

    0,  // cableVCPisConnected
    0,  // connectToHost

    // Flash drive
    0,  // flashDriveIsConnected
    1,  // cursorInDirs
    1,  // needRedrawFileManager

    0,  // settingsLoaded
    0,  // showHelpHints

    0,  // showDebugMenu

    GRID_BOTTOM, // topMeasures
 
    0,  // needOpenFileManager
    1,  // needClosePageSB
    0   // needResetSettings
};


GMemory gMemory =
{
    0,  // currentNumLatestSignal
    0,  // currentNumIntSignal
    0,  // showAlways
    0,  // runningFPGAbeforeSmallButtons
    0,  // exitFromIntToLast
    0,  // после установки имени и сохранения на флешку возвращаемся в основное рабочее окно
    0   // needForSaveToFlashDrive
};


StateFPGA gStateFPGA = 
{
    false,
    StateWorkFPGA_Stop,
    StateCalibration_None
};

const char *gStringForHint = 0;
void *gItemHint = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetItemForHint(void *item)
{
    gStringForHint = 0;
    gItemHint = item;
}

uint8           *gData0 = 0;
uint8           *gData1 = 0;
DataSettings    *gDSet = 0;

uint8           *gData0memInt = 0;
uint8           *gData1memInt = 0;
DataSettings    *gDSmemInt = 0;

uint8           *gData0memLast = 0;
uint8           *gData1memLast = 0;
DataSettings    *gDSmemLast = 0;

int transmitBytes = 0;
