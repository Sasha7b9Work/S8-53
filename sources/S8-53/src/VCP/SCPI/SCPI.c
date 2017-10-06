#include "defines.h"
#include "SCPI.h"
#include "commonSCPI.h"
#include "controlSCPI.h"
#include "Utils/Strings.h"
#include "Utils/GlobalFunctions.h"


#include <ctype.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    WAIT,
    SAVE_SYMBOLS
} StateProcessing;

static int FindNumSymbolsInCommand(uint8 *buffer);

#undef SIZE_BUFFER
#define SIZE_BUFFER 100
static uint8 buffer[SIZE_BUFFER];
static int pointer = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SCPI_AddNewData(uint8 *data, uint length)
{
    memcpy(&buffer[pointer], data, length);
    pointer += length;

label_another:

    for (int i = 0; i < pointer; i++)
    {
        buffer[i] = toupper(buffer[i]);

        if (buffer[i] == 0x0d || buffer[i] == 0x0a)
        {
            uint8 *pBuffer = buffer;
            while (*pBuffer == ':')
            {
                ++pBuffer;
            }

            SCPI_ParseNewCommand(pBuffer);
            if (i == pointer - 1)
            {
                pointer = 0;                // Если буфер пуст - выходим
                return;
            }
            else                            // Если в буфере есть есть данные
            {
                uint8 *pBuffer = buffer;
                for (++i; i < pointer; i++)
                {
                    *pBuffer = buffer[i];   // копируем их в начало
                    ++pBuffer;
                    pointer = pBuffer - buffer;
                }
                goto label_another;         // и проверяем буфер ещё раз
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI_ParseNewCommand(uint8 *buffer)
{
    static const StructCommand commands[] =
    {
    {"*IDN ?",      Process_IDN},
    {"*IDN?",       Process_IDN},
    {"RUN",         Process_RUN},
    {"STOP",        Process_STOP},
    {"RESET",       Process_RESET},
    {"AUTOSCALE",   Process_AUTOSCALE}, 
    {"REQUEST ?",   Process_REQUEST},

    {"DISPLAY",     Process_DISPLAY},       // Вначале всегда идёт полное слово, потом сокращение.
    {"DISP",        Process_DISPLAY},       // Это нужно для правильного парсинга.

    {"CHANNEL1",    Process_CHANNEL},
    {"CHAN1",       Process_CHANNEL},

    {"CHANNEL2",    Process_CHANNEL},
    {"CHAN2",       Process_CHANNEL},

    {"TRIGGER",     Process_TRIG},
    {"TRIG",        Process_TRIG},

    {"TBASE",       Process_TBASE},
    {"TBAS" ,       Process_TBASE},

    {"KEY",         Process_KEY},
    {"GOVERNOR",    Process_GOVERNOR},
    {0}
    };
    
    SCPI_ProcessingCommand(commands, buffer);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer) 
{
    int sizeNameCommand = FindNumSymbolsInCommand(buffer);
    if (sizeNameCommand == 0) 
    {
        return;
    }
    for (int i = 0; i < sizeNameCommand; i++)
    {
        buffer[i] = toupper(buffer[i]);
    }
    int numCommand = -1;
    char *name = 0;
    do 
    {
        numCommand++;   
        name = commands[numCommand].name;
    } while (name != 0 && (!EqualsStrings((char*)buffer, name, sizeNameCommand)));

    if (name != 0) 
    {
        commands[numCommand].func(buffer + sizeNameCommand + 1);
    }
    else
    {
        SCPI_SEND(":COMMAND ERROR");
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int FindNumSymbolsInCommand(uint8 *buffer)
{
    int pos = 0;
    while ((buffer[pos] != ':') && (buffer[pos] != ' ') && (buffer[pos] != '\x0d'))
    {
        pos++;
    }
    return pos;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SCPI_FirstIsInt(uint8 *buffer, int *value, int min, int max)
{
    Word param;
    if (GetWord(buffer, &param, 0))
    {
        char *n = (char *)malloc(param.numSymbols + 1);
        memcpy(n, param.address, param.numSymbols);
        n[param.numSymbols] = '\0';
        bool res = String2Int(n, value) && *value >= min && *value <= max;
        free(n);
        return res;
    }
    return false;
}
