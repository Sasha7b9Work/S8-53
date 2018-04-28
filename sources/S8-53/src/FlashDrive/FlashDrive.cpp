#include "defines.h"
#include "Log.h"
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include <usbh_def.h>
#include <ff_gen_drv.h>
#include <usbh_diskio.h>
#include <usbh_core.h>
#include <usbh_msc.h>
#include "ffconf.h"
#include "Hardware/Hardware.h"
#include "Hardware/CLOCK.h"
#include "FlashDrive.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static FATFS USBDISKFatFs;
static char USBDISKPath[4];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FlashDrive::Init(void)
{
    if(FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == FR_OK) 
    {
        USBH_Init(&handleUSBH, USBH_UserProcess, 0);
        USBH_RegisterClass(&handleUSBH, USBH_MSC_CLASS);
        USBH_Start(&handleUSBH);
    }
    else
    {
        LOG_ERROR("Can not %s", __FUNCTION__);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FlashDrive::Update(void)
{
    USBH_Process(&handleUSBH);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8 id)
{
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_CLASS_ACTIVE:
            FLASH_DRIVE_IS_CONNECTED = 1;
            FM::Init();
            Menu::ChangeStateFlashDrive();
            if (f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
            {
                LOG_ERROR("Не могу примонтировать диск");
            }
            break;
        case HOST_USER_CLASS_SELECTED:
            break;
        case HOST_USER_CONNECTION:
            f_mount(NULL, (TCHAR const*)"", 0);
            break;
        case HOST_USER_DISCONNECTION:
            FLASH_DRIVE_IS_CONNECTED = 0;
            Menu::ChangeStateFlashDrive();
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::AppendStringToFile(const char* string)
{
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WriteToFile(FIL *file, char *string)
{
    //    uint bytesWritten;
    f_open(file, "list.txt", FA_OPEN_EXISTING);
    //f_write(file, string, strlen(string), (void*)&bytesWritten);
    f_puts(string, file);
    f_close(file);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FlashDrive::GetNumDirsAndFiles(const char* fullPath, int *numDirs, int *numFiles)
{
    FILINFO fno;
    DIR dir;

    *numDirs = 0;
    *numFiles = 0;
    

    char nameDir[_MAX_LFN + 1];
    memcpy(nameDir, fullPath, strlen(fullPath));
    nameDir[strlen(fullPath)] = '\0';

    char lfn[(_MAX_LFN + 1)];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);

    if (f_opendir(&dir, nameDir) == FR_OK)
    {
        int numReadingElements = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(&dir, &fno) != FR_OK)
            {
                break;
            }
            if (fno.fname[0] == 0)
            {
                if(alreadyNull)
                {
                    break;
                }
                alreadyNull = true;
                continue;
            }
            numReadingElements++;
            char *fn = *fno.lfname ? fno.lfname : fno.fname;
            if (fn[0] != '.')
            {
                if (fno.fattrib & AM_DIR)
                {
                    (*numDirs)++;
                }
                else
                {
                    (*numFiles)++;
                }
            }
        }
        f_closedir(&dir);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::GetNameDir(const char *fullPath, int numDir, char *nameDirOut, StructForReadDir *s)
{
    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numDirs = 0;
        FILINFO *pFNO = &s->fno;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameDirOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (numDir == numDirs && (pFNO->fattrib & AM_DIR))
            {
                strcpy(nameDirOut, fn);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) && (pFNO->fname[0] != '.'))
            {
                numDirs++;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::GetNextNameDir(char *nameDirOut, StructForReadDir *s)
{
    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    while (true)
    {
        if (f_readdir(pDir, pFNO) != FR_OK)
        {
            *nameDirOut = '\0';
            f_closedir(pDir);
            return false;
        }
        else if (pFNO->fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameDirOut = '\0';
                f_closedir(pDir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (pFNO->fattrib & AM_DIR)
            {
                strcpy(nameDirOut, fn);
                return true;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FlashDrive::CloseCurrentDir(StructForReadDir *s)
{
    f_closedir(&s->dir);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::GetNameFile(const char *fullPath, int numFile, char *nameFileOut, StructForReadDir *s)
{
    memcpy(s->nameDir, fullPath, strlen(fullPath));
    s->nameDir[strlen(fullPath)] = '\0';

    s->fno.lfname = s->lfn;
    s->fno.lfsize = sizeof(s->lfn);

    DIR *pDir = &s->dir;
    FILINFO *pFNO = &s->fno;
    if (f_opendir(pDir, s->nameDir) == FR_OK)
    {
        int numFiles = 0;
        bool alreadyNull = false;
        while (true)
        {
            if (f_readdir(pDir, pFNO) != FR_OK)
            {
                *nameFileOut = '\0';
                f_closedir(pDir);
                return false;
            }
            if (pFNO->fname[0] == 0)
            {
                if (alreadyNull)
                {
                    *nameFileOut = '\0';
                    f_closedir(pDir);
                    return false;
                }
                alreadyNull = true;
            }
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if (numFile == numFiles && (pFNO->fattrib & AM_DIR) == 0)
            {
                strcpy(nameFileOut, fn);
                return true;
            }
            if ((pFNO->fattrib & AM_DIR) == 0 && (pFNO->fname[0] != '.'))
            {
                numFiles++;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::GetNextNameFile(char *nameFileOut, StructForReadDir *s)
{
    FILINFO *pFNO = &s->fno;
    bool alreadyNull = false;
    while (true)
    {
        if (f_readdir(&s->dir, &s->fno) != FR_OK)
        {
            *nameFileOut = '\0';
            f_closedir(&s->dir);
            return false;
        }
        if (s->fno.fname[0] == 0)
        {
            if (alreadyNull)
            {
                *nameFileOut = '\0';
                f_closedir(&s->dir);
                return false;
            }
            alreadyNull = true;
        }
        else
        {
            char *fn = *(pFNO->lfname) ? pFNO->lfname : pFNO->fname;
            if ((pFNO->fattrib & AM_DIR) == 0 && pFNO->fname[0] != '.')
            {
                strcpy(nameFileOut, fn);
                return true;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::OpenNewFileForWrite(const char* fullPathToFile, StructForWrite *structForWrite)
{
    if (f_open(&structForWrite->fileObj, fullPathToFile, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        return false;
    }
    strcpy(structForWrite->name, fullPathToFile);
    structForWrite->sizeData = 0;
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::WriteToFile(uint8* data, int sizeData, StructForWrite *structForWrite)
{
    while (sizeData > 0)
    {
        int dataToCopy = sizeData;
        if (sizeData + structForWrite->sizeData > SIZE_FLASH_TEMP_BUFFER)
        {
            dataToCopy = SIZE_FLASH_TEMP_BUFFER - structForWrite->sizeData;
        }
        sizeData -= dataToCopy;
        memcpy(structForWrite->tempBuffer + structForWrite->sizeData, data, dataToCopy);
        data += dataToCopy;
        structForWrite->sizeData += dataToCopy;
        if (structForWrite->sizeData == SIZE_FLASH_TEMP_BUFFER)
        {
            uint wr = 0;
            if (f_write(&structForWrite->fileObj, structForWrite->tempBuffer, (uint)structForWrite->sizeData, &wr) != FR_OK || (uint)structForWrite->sizeData != wr)
            {
                return false;
            }
            structForWrite->sizeData = 0;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FlashDrive::CloseFile(StructForWrite *structForWrite)
{
    if (structForWrite->sizeData != 0)
    {
        uint wr = 0;
        if (f_write(&structForWrite->fileObj, structForWrite->tempBuffer, (uint)structForWrite->sizeData, &wr) != FR_OK || (uint)structForWrite->sizeData != wr)
        {
            f_close(&structForWrite->fileObj);
            return false;
        }
    }
    f_close(&structForWrite->fileObj);

    FILINFO fno;
    PackedTime time = Clock::GetPackedTime();
    fno.fdate = (WORD)(((time.year + 20) * 512) | (time.month * 32) | time.day);
    fno.ftime = (WORD)((time.hours * 2048) | (time.minutes * 32) | (time.seconds / 2));
    f_utime(structForWrite->name, &fno);

    return true;
}
