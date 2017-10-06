#pragma once
#include "defines.h"


// Программа и константные данные
#define ADDR_FLASH_SECTOR_0     ((uint)0x08000000)  ///< Base @ of Sector 0, 16 Kbytes
#define ADDR_FLASH_SECTOR_1     ((uint)0x08004000)  ///< Base @ of Sector 1, 16 Kbytes
#define ADDR_FLASH_SECTOR_2     ((uint)0x08008000)  ///< Base @ of Sector 2, 16 Kbytes
#define ADDR_FLASH_SECTOR_3     ((uint)0x0800C000)  ///< Base @ of Sector 3, 16 Kbytes
#define ADDR_FLASH_SECTOR_4     ((uint)0x08010000)  ///< Base @ of Sector 4, 64 Kbytes
#define ADDR_SECTOR_PROGRAM_0   ((uint)0x08020000)  ///< |
#define ADDR_SECTOR_PROGRAM_1   ((uint)0x08040000)  ///< | Сюда зашивается прошивка
#define ADDR_SECTOR_PROGRAM_2   ((uint)0x08060000)  ///< /
#define ADDR_FLASH_SECTOR_8     ((uint)0x08080000)  ///< Base @ of Sector 8, 128 Kbytes
#define ADDR_FLASH_SECTOR_9     ((uint)0x080A0000)  ///< Base @ of Sector 9, 128 Kbytes
#define ADDR_FLASH_SECTOR_10    ((uint)0x080C0000)  ///< Base @ of Sector 10, 128 Kbytes
#define ADDR_FLASH_SECTOR_11    ((uint)0x080E0000)  ///< Base @ of Sector 11, 128 Kbytes


/// Ощищает место для последующей заливки прошивки
void FLASH_Prepare(void);

void FLASH_WriteData(uint address, uint8 *data, int size);
