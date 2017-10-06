#include "FSMC.h"
#include "Hardware.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Log.h"

#include <stm32f2xx_hal.h>
#include <stm32f2xx_ll_fsmc.h>
#include <stm32f2xx_hal_sram.h>


void FSMC_Init(void);
uint8 FSMC_Read(uint8 *address);
void FSMC_Write(uint8 *address, uint8 value);


uint8 FSMC_Read(uint8 *address)
{
    return(*(address));
}


void FSMC_Write(uint8 *address, uint8 value)
{
    *address = value;
}

void FSMC_Init()
{
    static const FSMC_NORSRAM_TimingTypeDef sramTiming =
    {
        5,                  // FSMC_AddressSetupTime
        7,                  // FSMC_AddressHoldTime
        13,                 // FSMC_DataSetupTime
        11,                 // FSMC_BusTurnAroundDuration
        3,                  // FSMC_CLKDivision
        0,                  // FSMC_DataLatency
        FSMC_ACCESS_MODE_B  // FSMC_AccessMode
    };

    static const SRAM_HandleTypeDef sramHandle =
    {
        FSMC_NORSRAM_DEVICE,
        FSMC_NORSRAM_EXTENDED_DEVICE,
        {
            FSMC_NORSRAM_BANK1,                 // Init.NSBank
            FSMC_DATA_ADDRESS_MUX_ENABLE,       // Init.DataAddressMux
            FSMC_MEMORY_TYPE_NOR,               // Init.MemoryType
            FSMC_NORSRAM_MEM_BUS_WIDTH_8,       // Init.MemoryDataWidth
            FSMC_BURST_ACCESS_MODE_DISABLE,     // Init.BurstAccessMode
            FSMC_WAIT_SIGNAL_POLARITY_LOW,      // Init.WaitSignalPolarity
            FSMC_WRAP_MODE_DISABLE,             // Init.WrapMode
            FSMC_WAIT_TIMING_BEFORE_WS,         // Init.WaitSignalActive
            FSMC_WRITE_OPERATION_ENABLE,        // Init.WriteOperation
            FSMC_WAIT_SIGNAL_DISABLE,           // Init.WaitSignal
            FSMC_EXTENDED_MODE_DISABLE,         // Init.ExtendedMode
            FSMC_ASYNCHRONOUS_WAIT_DISABLE,     // Init.AsynchronousWait
            FSMC_WRITE_BURST_DISABLE            // Init.WriteBurst
        }
    };

    if (HAL_SRAM_Init((SRAM_HandleTypeDef*)(&sramHandle), (FSMC_NORSRAM_TimingTypeDef*)(&sramTiming), (FSMC_NORSRAM_TimingTypeDef*)(&sramTiming)) != HAL_OK)
    {
        HARDWARE_ERROR
    }
}
