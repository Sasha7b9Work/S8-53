#pragma once
#include "Globals.h"
#include "VCP/VCP.h"
#include "Ethernet/TcpSocket.h"


/** @defgroup SCPI
 *  @{
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENTER_ANALYSIS                                  \
    Word parameter;                                     \
    if (GetWord(buffer, &parameter, 0)) {               \
        uint8 value = GetValueFromMap(map, &parameter); \
        if (value < 255) {

#define LEAVE_ANALYSIS   }                              \
        else SCPI_SEND(":DATA ERROR")                   \
        }


#define SCPI_SEND(...)                                  \
    if(gBF.connectToHost == 1)                          \
    {                                                   \
        VCP_SendFormatStringAsynch(__VA_ARGS__);        \
    }                                                   \
    if (gBF.ethIsConnected == 1)                        \
    {                                                   \
        ETH_SendFormatString(__VA_ARGS__);              \
    }

#define ENTER_PARSE_FUNC(funcName)                      \
void funcName(uint8 *buffer)                            \
{                                                       \
    static const StructCommand commands[] =             \
    {

#define LEAVE_PARSE_FUNC                                \
        {0}                                             \
    };                                                  \
    SCPI_ProcessingCommand(commands, buffer);           \
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    char        *name;
    pFuncpU8    func;
} StructCommand;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SCPI_ParseNewCommand(uint8 *buffer);   ///< \todo Временно. Потом доделать
void SCPI_AddNewData(uint8 *buffer, uint length);
void SCPI_ProcessingCommand(const StructCommand *commands, uint8 *buffer);
void Process_DISPLAY(uint8 *buffer);
void Process_CHANNEL(uint8 *buffer);
void Process_TRIG(uint8 *buffer);
void Process_TBASE(uint8 *buffer);
bool SCPI_FirstIsInt(uint8 *buffer, int *value, int min, int max);

/** @}
 */
