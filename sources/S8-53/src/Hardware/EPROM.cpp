#include "EPROM.h"
#include "Hardware.h"
#include "Sound.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Utils/GlobalFunctions.h"
#include "Log.h"
#include <stm32f2xx_hal.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_FLAGS \
__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)


struct RecordConfig
{
    uint addrData;  ///< Начиная с этого адреса записаны данные. Если addrData == MAX_UINT, то это пустая запись, сюда можно писать данные
    int  sizeData;  ///< Размер в байтах записанных данных
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIZE_ARRAY_POINTERS_IN_ELEMENTS 1024
#define ADDR_ARRAY_POINTERS (ADDR_SECTOR_SETTINGS + 4)
#define SIZE_ARRAY_POINTERS_IN_BYTES (SIZE_ARRAY_POINTERS_IN_ELEMENTS * sizeof(RecordConfig))
static const uint ADDR_FIRST_SET = ADDR_ARRAY_POINTERS + SIZE_ARRAY_POINTERS_IN_BYTES;      // По этому адресу записаны первые настройки
static const uint SIZE_MEMORY = 1024 * 1024 + 0x08000000;


// Признак того, что запись в этоу область флэш уже производилась. Если нулевое слово области (данных, ресурсов или настроек) имеет это значение, 
// запись уже была произведена как минимум один раз
static const uint MARK_OF_FILLED = 0x1234;
static const uint MAX_UINT = 0xffffffff;

#define READ_WORD(address) (*((volatile uint*)(address)))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint startDataInfo = ADDR_SECTOR_DATA_MAIN;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EPROM::PrepareSectorForData()
{
    EraseSector(ADDR_SECTOR_DATA_MAIN);
    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        WriteWord(startDataInfo + i * 4, 0);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EPROM::LoadSettings(void)
{
    /*
        1. Проверка на первое включение. Выполняется тем, что в первом слове сектора настроек хранится MAX_UINT, если настройки ещё не сохранялись.
        2. Проверка на старую версию хранения настроек. Определяется тем, что в старой версии в первом слове сектора настроек хранится значение
        MARK_OF_FILLED, а в новой - размер структуры Settings (раньше этого поля не было в структуре Settings).
        3. Если старая версия - чтение настроек и сохранение в новом формате.
            1. Чтобы прочитать, нужно сначала найти адрес последних сохранённых настроек.
            2. Если (адрес + sizeof(Settings) >= ADDR_SECTORSETTINGS + (1024 * 128)), то эти нстройки повреждены и нужно считывать предпоследние
               сохранённые настройки.
            3. Когда нашли адрес последних действующих настроек, читаем их по адресу &set.display, записываем в set.size полный размер структуры
            Settings и вызываем Flash_SaveSettings().
    */

    CLEAR_FLAGS;

    if(TheFirstInclusion())                                         // Если это первое включение
    {                                                               // то делаем предварительные приготовления
        set.common.countErasedFlashSettings = 0;
        set.common.countEnables = 0;
        set.common.countErasedFlashData = 0;
        set.common.workingTimeInSecs = 0;
        PrepareSectorForData();
    }
    
    if(READ_WORD(ADDR_SECTOR_SETTINGS) == 0x12345)
    {
        EraseSector(ADDR_SECTOR_SETTINGS);
    }
    else if (READ_WORD(ADDR_SECTOR_SETTINGS) == MARK_OF_FILLED)                             // Если старый алгоритм хранения настроек
    {
        RecordConfig *record = RecordConfigForRead();
        if (record->sizeData + record->addrData >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS))   // Если последние сохранённые настройки выходят
        {                                                                                   // за пределы сектора (глюк предыдущей версии сохранения)
            --record;                                                                       // то воспользуемся предыдущими сохранёнными настройками
        }
        memcpy(&set, (const void *)(record->addrData - 4), record->sizeData);               // Считываем их
        EraseSector(ADDR_SECTOR_SETTINGS);                                                  // Стираем сектор настроек
        EPROM::SaveSettings(true);                                                           // И сохраняем настройки в новом формате
    }
    else
    {
        uint addressPrev = 0;
        uint address = ADDR_SECTOR_SETTINGS;
        while (READ_WORD(address) != MAX_UINT)  // Пока по этому адресу есть значение, отличное от (-1) (сюда производилась запись)
        {
            addressPrev = address;              // сохраняем этот адрес
            address += READ_WORD(address);      // И переходим к следующему прибавлением значения, хранящегося по этому адресу (первый элемент
        }                                       // структуры Settings - её размер. Все настройки хранятся последовательно в памяти, одна структура
                                                // за другой
        
        if (addressPrev != 0)                   // Если по этому адресу что-то записано
        {
            memcpy(&set, (const void *)addressPrev, (int)READ_WORD(addressPrev));    // Счтываем сохранённые настройки
            return true;
        }
    }
    set.common.countEnables++;
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::WriteAddressDataInRecord(RecordConfig *record)
{
    uint address = (record == FirstRecord()) ? ADDR_FIRST_SET : (record - 1)->addrData + (record - 1)->sizeData;

    WriteWord((uint)(&record->addrData), address);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::SaveSettings(bool verifyLoadede)
{
    if (!verifyLoadede && !SETTINGS_IS_LOADED)
    {
        return;
    }

    CLEAR_FLAGS;

    set.size = sizeof(set);

    uint address = ADDR_SECTOR_SETTINGS;                                        // Находим первый свободный байт

    while (READ_WORD(address) != MAX_UINT)
    {
        address += READ_WORD(address);
    }
                                                                                // В этой точке address указывает на первый незаписанный байт

    if (address + sizeof(set) >= (ADDR_SECTOR_SETTINGS + SIZE_SECTOR_SETTINGS)) // Если условие выполняется, то при записи данные выйдут за пределы
    {                                                                           // сектора
        EraseSector(ADDR_SECTOR_SETTINGS);                                      // В этом случае стираем сектор настроек
        address = ADDR_SECTOR_SETTINGS;                                         // и сохранять настройки будем прямо в начало сектора
    }

    WriteBufferBytes(address, (uint8 *)&set, sizeof(set));                      // И банально сохраняем настройки
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EPROM::TheFirstInclusion()
{
    return READ_WORD(ADDR_SECTOR_SETTINGS) == MAX_UINT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig* EPROM::RecordConfigForRead()
{
    if (!TheFirstInclusion())
    {
        RecordConfig *record = FirstEmptyRecord();
        return --record;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig *EPROM::FirstRecord()
{
    return (RecordConfig*)ADDR_ARRAY_POINTERS;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EPROM::RecordExist()
{
    return READ_WORD(ADDR_ARRAY_POINTERS) != MAX_UINT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
RecordConfig *EPROM::FirstEmptyRecord()
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
uint EPROM::CalculatFreeMemory()
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
uint EPROM::FindAddressNextDataInfo()
{
    uint addressNextInfo = startDataInfo + MAX_NUM_SAVED_WAVES * 4;

    while (*((uint*)addressNextInfo) != 0xffffffff)
    {
        addressNextInfo = *((uint*)addressNextInfo) + MAX_NUM_SAVED_WAVES * 4;
    }

    return addressNextInfo;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint EPROM::FindActualDataInfo()
{
    return FindAddressNextDataInfo() - MAX_NUM_SAVED_WAVES * 4;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::GetDataInfo(bool existData[MAX_NUM_SAVED_WAVES])
{
    uint address = FindActualDataInfo();

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        existData[i] = READ_WORD(address + i * 4) != 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EPROM::ExistData(int num)
{
    uint address = FindActualDataInfo();
    return READ_WORD(address + num * 4) != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::DeleteData(int num)
{
    uint address = FindActualDataInfo();
    WriteWord(address + num * 4, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::EraseData()
{
    CLEAR_FLAGS;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = FLASH_SECTOR_8;
    flashITD.NbSectors = 2;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint error = 0;

    while(SOUND_IS_BEEP) {};

    HAL_FLASHEx_Erase(&flashITD, &error);

    PrepareSectorForData();

    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int EPROM::CalculateSizeData(DataSettings *ds)
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
uint EPROM::FreeMemory()
{
    return ADDR_SECTOR_DATA_MAIN + 128 * 1024 - FindAddressNextDataInfo() - 1 - 4 * MAX_NUM_SAVED_WAVES - 3000;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::CompactMemory()
{
    Display::ClearFromWarnings();
    Display::ShowWarningGood(MovingData);
    Display::Update();
    uint dataInfoRel = FindActualDataInfo() - ADDR_SECTOR_DATA_MAIN;

    EraseSector(ADDR_SECTOR_DATA_HELP);
    WriteBufferBytes(ADDR_SECTOR_DATA_HELP, (uint8*)ADDR_SECTOR_DATA_MAIN, 1024 * 128);
    PrepareSectorForData();

    uint addressDataInfo = ADDR_SECTOR_DATA_HELP + dataInfoRel;

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        uint addrDataOld = READ_WORD(addressDataInfo + i * 4);
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
            EPROM::SaveData(i, ds, data0, data1);
        }
    }
    Display::ClearFromWarnings();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::SaveData(int num, DataSettings *ds, uint8 *data0, uint8 *data1)
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

    if (ExistData(num))
    {
        DeleteData(num);
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
        WriteBufferBytes(address, (uint8*)data0, (int)ds->length1channel);       // Сохраняем первый канал
        address += ds->length1channel;
    }

    if (ds->enableCh1 == 1)
    {
        WriteBufferBytes(address, (uint8*)data1, (int)ds->length1channel);       // Сохраняем второй канал
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
            WriteWord(addressForWrite, READ_WORD(addrDataInfo + i * 4));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool EPROM::GetData(int num, DataSettings **ds, uint8 **data0, uint8 **data1)
{
    uint addrDataInfo = FindActualDataInfo();
    if (READ_WORD(addrDataInfo + 4 * num) == 0)
    {
        *ds = 0;
        *data0 = 0;
        *data1 = 0;
        return false;
    }

    uint addrDS = READ_WORD(addrDataInfo + 4 * num);

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
uint EPROM::GetSector(uint startAddress)
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
void EPROM::EraseSector(uint startAddress)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flashITD;
    flashITD.TypeErase = TYPEERASE_SECTORS;
    flashITD.Sector = GetSector(startAddress);
    flashITD.NbSectors = 1;
    flashITD.VoltageRange = VOLTAGE_RANGE_3;

    uint32_t error = 0;
    while (SOUND_IS_BEEP) {};
    HAL_FLASHEx_Erase(&flashITD, &error);

    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void EPROM::WriteWord(uint address, uint word)
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
void EPROM::WriteBufferBytes(uint address, uint8 *buffer, int size)
{
    HAL_FLASH_Unlock();
    for (int i = 0; i < size; i++)
    {
        if (HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, (uint64_t)(buffer[i])) != HAL_OK)
        {
            HARDWARE_ERROR;
        }
        address++;
    }
    HAL_FLASH_Lock();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool OTP::SaveSerialNumber(char *serialNumber)
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
        EPROM::WriteBufferBytes((uint)address, (uint8*)serialNumber, strlen(serialNumber) + 1);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int OTP::GetSerialNumber(char buffer[17])
{
    /// \todo улучшить - нельзя разбрасываться байтами. Каждая запись должна занимать столько места, сколько в ней символов, а не 16, как сейчас.

    const int allShotsMAX = 512 / 16;   // Максимальное число записей в OPT серийного номера.

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
