// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "FLASH.h"
#include "Hardware.h"
#include "Sound.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include <stm32f2xx_hal.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint addrData;  ///< Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
    int  sizeData;  ///< Размер в байтах записанных данных
} RecordConfig;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)
#define SIZE_ARRAY_POINTERS_IN_BYTES (SIZE_ARRAY_POINTERS_IN_ELEMENTS * sizeof(RecordConfig))
static const uint ADDR_FIRST_SET = ADDR_ARRAY_POINTERS + SIZE_ARRAY_POINTERS_IN_BYTES;      // По этому адресу записаны первые настройки
static const uint SIZE_MEMORY = 1024 * 1024 + 0x08000000;


// Признак того, что запись в этоу область флэш уже производилась. Если нулевое слово области (данных, ресурсов или настроек) имеет это значение, запись уже была произведена как минимум один раз
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint             ReadWord(uint address);
static bool             TheFirstInclusion(void);
static RecordConfig*    FindRecordConfigForWrite(void);
static RecordConfig*    RecordConfigForRead(void);
static void             ReadBuffer(uint address, uint *buffer, int size);
static void             EraseSector(uint startAddress);
static void             WriteWord(uint address, uint word);
static void             WriteBuffer(uint address, uint *buffer, int size);
static void             WriteBufferBytes(uint address, uint8 *buffer, int size);
static RecordConfig*    FirstEmptyRecord(void);
static RecordConfig*    FirstRecord(void);
static void             EraseSectorSettings(void);

static const uint startDataInfo = ADDR_SECTOR_DATA_MAIN;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void PrepareSectorForData(void)
{
    EraseSector(ADDR_SECTOR_DATA_MAIN);
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_LoadSettings(void)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    if(TheFirstInclusion())
    {
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        EraseSectorSettings();
        PrepareSectorForData();
    }
    else
    {
        RecordConfig *record = RecordConfigForRead();
        int size = sizeof(set);
        if (record && size == record->sizeData)
        {
            uint addrData = record->addrData;
            uint *addrSet = (uint*)(&set);
            int numWodsForCopy = record->sizeData / 4;
            ReadBuffer(addrData, addrSet, numWodsForCopy);
            return true;
        }
    }
    set.common.countEnables++;
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteAddressDataInRecord(RecordConfig *record)
{
    uint address = (record == FirstRecord()) ? ADDR_FIRST_SET : (record - 1)->addrData + (record - 1)->sizeData;

    WriteWord((uint)(&record->addrData), address);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveSettings(void)
{
    if (gBF.settingsLoaded == 0)
    {
        return;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    RecordConfig *record = FindRecordConfigForWrite();
    if (record == 0)
    {
        set.common.countErasedFlashSettings++;
        EraseSectorSettings();
        record = FirstRecord();
    }

    set.common.workingTimeInSecs += gTimerMS / 1000;

    WriteAddressDataInRecord(record);

    int size = sizeof(set);

    WriteWord((uint)(&record->sizeData), size);

    WriteBuffer(record->addrData, (uint*)(&set), record->sizeData / 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint ReadWord(uint address)
{
    return(*((volatile uint*)address));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool TheFirstInclusion(void)
{
    return ReadWord(ADDR_SECTOR_SETTINGS) != MARK_OF_FILLED;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig* RecordConfigForRead(void)
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmptyRecord();
        return --record;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig *FirstRecord(void)
{
    return (RecordConfig*)ADDR_ARRAY_POINTERS;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool RecordExist(void)
{
    return ReadWord(ADDR_ARRAY_POINTERS) != MAX_UINT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig *FirstEmptyRecord(void)
{
    RecordConfig *record = FirstRecord();
    int numRecord = 0;

    while (record->addrData != MAX_UINT)
    {
        record++;
        if ((++numRecord) == SIZE_ARRAY_POINTERS_IN_ELEMENTS)
        {
            return 0;
        }
    }

    return record;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint CalculatFreeMemory(void)
{
    if (!RecordExist())
    {
        return SIZE_MEMORY - ADDR_FIRST_SET;
    }

    RecordConfig *firstEmptyRecord = FirstEmptyRecord();

    if (firstEmptyRecord == 0)  // Если все записи заняты
    {
        return 0;
    }

    return SIZE_MEMORY - (firstEmptyRecord - 1)->addrData - (firstEmptyRecord - 1)->sizeData - 4;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig* FindRecordConfigForWrite(void)
{
    RecordConfig *record = FirstEmptyRecord();

    int size = sizeof(set);

    if (record == 0 || CalculatFreeMemory() < size)
    {
        return 0;
    }

    return record;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint *addr = 0;

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ReadBuffer(uint address, uint *buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        addr = ((uint*)address);
        //buffer[i] = *((uint*)address);
        buffer[i] = *addr;
        address += 4;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint FindAddressNextDataInfo(void)
{
    uint addressNextInfo = startDataInfo + MAX_NUM_SAVED_WAVES * 4;

    while (*((uint*)addressNextInfo) != 0xffffffff)
    {
        addressNextInfo = *((uint*)addressNextInfo) + MAX_NUM_SAVED_WAVES * 4;
    }

    return addressNextInfo;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint FindActualDataInfo(void)
{
    return FindAddressNextDataInfo() - MAX_NUM_SAVED_WAVES * 4;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    uint address = FindActualDataInfo();

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = ReadWord(address + i * 4) != 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_ExistData(int num)
{
    uint address = FindActualDataInfo();
    return ReadWord(address + num * 4) != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_DeleteData(int num)
{
    uint address = FindActualDataInfo();
    WriteWord(address + num * 4, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateSizeRecordConfig(void)
{
    int size = sizeof(RecordConfig);
    while (size % 4)
    {
        size++;
    }
    return size;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateSizeDataSettings(void)
{
    int size = sizeof(DataSettings);
    while (size % 4)
    {
        size++;
    }
    return size;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int CalculateSizeData(DataSettings *ds)
{
    int size = sizeof(DataSettings);
    if (ds->enableCh0 == 1)
    {
        size += ds->length1channel;
    }
    if (ds->enableCh1 == 1)
    {
        size += ds->length1channel;
    }
    return size;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint FreeMemory(void)
{
    return ADDR_SECTOR_DATA_MAIN + 128 * 1024 - FindAddressNextDataInfo() - 1 - 4 * MAX_NUM_SAVED_WAVES - 3000;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CompactMemory(void)
{
    Display_ClearFromWarnings();
    Display_ShowWarningGood(MovingData);
    Display_Update();
    uint dataInfoRel = FindActualDataInfo() - ADDR_SECTOR_DATA_MAIN;

    EraseSector(ADDR_SECTOR_DATA_HELP);
    WriteBufferBytes(ADDR_SECTOR_DATA_HELP, (uint8*)ADDR_SECTOR_DATA_MAIN, 1024 * 128);
    PrepareSectorForData();

    uint addressDataInfo = ADDR_SECTOR_DATA_HELP + dataInfoRel;

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addrDataOld = ReadWord(addressDataInfo + i * 4);
        if (addrDataOld != 0)
        {
            uint addrDataNew = addrDataOld + 1024 * 128;
            DataSettings *ds = (DataSettings*)addrDataNew;
            addrDataNew += sizeof(DataSettings);
            uint8 *data0 = 0;
            uint8 *data1 = 0;
            if (ds->enableCh0 == 1)
            {
                data0 = (uint8*)addrDataNew;
                addrDataNew += ds->length1channel;
            }
            if (ds->enableCh1 == 1)
            {
                data1 = (uint8*)addrDataNew;
            }
            FLASH_SaveData(i, ds, data0, data1);
        }
    }
    Display_ClearFromWarnings();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FLASH_SaveData(int num, DataSettings *ds, uint8 *data0, uint8 *data1)
{
    /*
        1. Узнаём количество оставшейся памяти.
        2. Если не хватает для записи данных и массива информации - уплотняем память.
        3. Находим последний сохранённый массив информации.
        4. В хвост ему пишем адрес, в котором будет храниться след. массив информации.
        5. Записываем за ним данные.
        6. За данными записываем обновлённый массив информации.
    */

    /*
        Алгоритм сохранения данных.
        ADDR_SECTOR_DATA_HELP используется для временного хранения данных, когда полностью заполнен ADDR_SECTOR_DATA_MAIN и некуда писать очередной сигнал

        Данные расположены в памяти следующим образом.
    
        Начинаются с ADDR_SECTOR_DATA_MAIN.

        uint[MAX_NUM_SAVED_WAVES] - адреса, по которым записаны соответствующие сигналы. Если 0 - сигнал стёрт.
        uint - адрес первой свободной ячейки памяти (следующего массива адресов). В неё будет записан адрес первого сигнала.
    */

    if (FLASH_ExistData(num))
    {
        FLASH_DeleteData(num);
    }

    int size = CalculateSizeData(ds);

// 2
    if (FreeMemory() < size)
    {
        CompactMemory();
    }

// 3
    uint addrDataInfo = FindActualDataInfo();          // Находим начало действующего информационного массива

// 4
    uint addrForWrite = addrDataInfo + MAX_NUM_SAVED_WAVES * 4;             // Это - адрес, по которому будет храниться адрес следующего информационного массива
    uint valueForWrite = addrForWrite + 4 + size;                           // Это - адрес следующего информационного массива
    WriteWord(addrForWrite, valueForWrite);

// 5
    uint address = addrDataInfo + MAX_NUM_SAVED_WAVES * 4 + 4;              // Адрес, по которому будет сохранён сигнал с настройками
    uint addressNewData = address;
    WriteBufferBytes(address, (uint8*)ds, sizeof(DataSettings));            // Сохраняем настройки сигнала
    address += sizeof(DataSettings);
    
    if (ds->enableCh0 == 1)
    {
        WriteBufferBytes(address, (uint8*)data0, ds->length1channel);       // Сохраняем первый канал
        address += ds->length1channel;
    }

    if (ds->enableCh1 == 1)
    {
        WriteBufferBytes(address, (uint8*)data1, ds->length1channel);       // Сохраняем второй канал
        address += ds->length1channel;
    }

// 6
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addressForWrite = address + i * 4;
        if (i == num)
        {
            WriteWord(addressForWrite, addressNewData);
        }
        else
        {
            WriteWord(addressForWrite, ReadWord(addrDataInfo + i * 4));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FLASH_GetData(int num, DataSettings **ds, uint8 **data0, uint8 **data1)
{
    uint addrDataInfo = FindActualDataInfo();
    if (ReadWord(addrDataInfo + 4 * num) == 0)
    {
        *ds = 0;
        *data0 = 0;
        *data1 = 0;
        return false;
    }

    uint addrDS = ReadWord(addrDataInfo + 4 * num);

    uint addrData0 = 0;
    uint addrData1 = 0;

    *ds = (DataSettings*)addrDS;
    
    if ((*ds)->enableCh0 == 1)
    {
        addrData0 = addrDS + sizeof(DataSettings);
    }

    if ((*ds)->enableCh1 == 1)
    {
        if (addrData0 != 0)
        {
            addrData1 = addrData0 + (*ds)->length1channel;
        }
        else
        {
            addrData1 = addrDS + sizeof(DataSettings);
        }
    }

    *data0 = (uint8*)addrData0;
    *data1 = (uint8*)addrData1;
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint GetSector(uint startAddress)
{
    switch (startAddress)
    {
        case ADDR_SECTOR_DATA_MAIN:
            return FLASH_SECTOR_8;
        case ADDR_SECTOR_DATA_HELP:
            return FLASH_SECTOR_9;
        case ADDR_SECTOR_RESOURCES:
            return FLASH_SECTOR_10;
        case ADDR_SECTOR_SETTINGS:
            return FLASH_SECTOR_11;
    }
    LOG_ERROR("Недопустимый сектор");
    return FLASH_SECTOR_11;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EraseSector(uint startAddress)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;
    while (gBF.soundIsBeep == 1) {};
    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EraseSectorSettings(void)
{
    EraseSector(ADDR_SECTOR_SETTINGS);
    WriteWord(ADDR_SECTOR_SETTINGS, MARK_OF_FILLED);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteWord(uint address, uint word)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)word) != HAL_OK)
    {
        LOG_ERROR("Не могу записать в память");
    }
    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteBuffer(uint address, uint *buffer, int size)
{
    HAL_FLASH_Unlock();
    for (int i = 0; i < size; i++)
    {
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, address, (uint64_t)(buffer[i])) != HAL_OK)
        {
            LOG_ERROR("Не могу записать в флеш-память");
        }
        address += sizeof(uint);
    }
    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteBufferBytes(uint address, uint8 *buffer, int size)
{
    HAL_FLASH_Unlock();
    for (int i = 0; i < size; i++)
    {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(buffer[i]));
        address++;
    }
    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool OTP_SaveSerialNumber(char *serialNumber)
{
    // Находим первую пустую строку длиной 16 байт в области OPT, начиная с начала.
    uint8 *address = (uint8*)FLASH_OTP_BASE;

    while ((*address) != 0xff &&                // *address != 0xff означает, что запись в эту строку уже производилась
           address < (uint8*)FLASH_OTP_END - 16)
    {
        address += 16;
    }

    if (address < (uint8*)FLASH_OTP_END - 16)
    {
        WriteBufferBytes((uint)address, (uint8*)serialNumber, strlen(serialNumber) + 1);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int OTP_GetSerialNumber(char buffer[17])
{
    /// \todo улучшить - нельзя разбрасываться байтами. Каждая запись должна занимать столько места, сколько в ней символов, а не 16, как сейчас.

    const int allShotsMAX = 512 / 16;   ///< Максимальное число записей в OPT серийного номера.

    uint8* address = (uint8*)FLASH_OTP_END - 15;

    do
    {
        address -= 16;
    } while(*address == 0xff && address > (uint8*)FLASH_OTP_BASE);

    if (*address == 0xff)   // Не нашли строки с информацией, дойдя до начального адреса OTP
    {
        buffer[0] = 0;
        return allShotsMAX;
    }

    strcpy(buffer, (char*)address);

    return allShotsMAX - (address - (uint8*)FLASH_OTP_BASE) / 16 - 1;
}
