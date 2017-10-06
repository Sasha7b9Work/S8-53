#include "Flash.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_FLASH_FLAGS   \
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLASH_Prepare(void)
{
    CLEAR_FLASH_FLAGS;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = FLASH_SECTOR_5;
    flashITD.NbSectors = 3;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint error = 0;
    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_WriteData(uint address, uint8 *data, int size)
{
    CLEAR_FLASH_FLAGS;

    HAL_FLASH_Unlock();

    for (int i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address++, (uint64_t)data[i]);
    }

    HAL_FLASH_Lock();
}
