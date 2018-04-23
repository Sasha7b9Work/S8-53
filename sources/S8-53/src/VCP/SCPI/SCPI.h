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
        else {LOG_WRITE("Error");}                      \
    }


#define SCPI_SEND(...)                                  \
    if(gBF.connectToHost == 1)                          \
    {                                                   \
        VCP::SendFormatStringAsynch(__VA_ARGS__);        \
    }                                                   \
    if (CLIENT_LAN_IS_CONNECTED)                        \
    {                                                   \
        TCPSocket_SendFormatString(__VA_ARGS__);        \
    }

#define ENTER_PARSE_FUNC(funcName)                      \
void funcName(uint8 *buffer)                            \
{                                                       \
    static const StructCommand commands[] =             \
    {

#define LEAVE_PARSE_FUNC                                \
        {0}                                             \
    };                                                  \
    SCPI::ProcessingCommand(commands, buffer);           \
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    char        *name;
    pFuncpU8    func;
} StructCommand;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SCPI
{
public:
    static void ParseNewCommand(uint8 *buffer);   ///< \todo Временно. Потом доделать
    static void AddNewData(uint8 *buffer, uint length);
    static void ProcessingCommand(const StructCommand *commands, uint8 *buffer);
    static bool FirstIsInt(uint8 *buffer, int *value, int min, int max);

private:
    static void ProcessDISPLAY(uint8 *buffer);
    static void ProcessCHANNEL(uint8 *buffer);
    static void ProcessTRIG(uint8 *buffer);
    static void ProcessTBASE(uint8 *buffer);
};




/** @}
 */
