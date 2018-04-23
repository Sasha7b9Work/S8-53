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
        VCP::SendFormatStringAsynch(__VA_ARGS__);       \
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
    SCPI::ProcessingCommand(commands, buffer);          \
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
    static void Processing(uint8 *data, uint length);
    static void ProcessDISPLAY(uint8 *buffer);
    static void ProcessCHANNEL(uint8 *buffer);
    static void ProcessTRIG(uint8 *buffer);
    static void ProcessTBASE(uint8 *buffer);

    class COMMON
    {
    public:
        static void IDN(uint8 *buffer);
        static void RUN(uint8 *buffer);
        static void STOP(uint8 *buffer);
        static void RESET(uint8 *buffer);
        static void AUTOSCALE(uint8 *buffer);
        static void REQUEST(uint8 *buffer);
    };

    class CONTROL
    {
    public:
        static void KEY(uint8 *buffer);
        static void GOVERNOR(uint8 *buffer);
    };

    class CHANNEL
    {
    public:
        static void INPUT(uint8 *buffer);
        static void COUPLE(uint8 *buffer);
        static void FILTR(uint8 *buffer);
        static void INVERSE(uint8 *buffer);
        static void RANGE(uint8 *buffer);
        static void OFFSET(uint8 *buffer);
        static void FACTOR(uint8 *buffer);
    };

    // DISPlay
    static void ProcessAUTOSEND(uint8 *buffer);
    static void ProcessMAPPING(uint8 *buffer);
    static void ProcessACCUM(uint8 *buffer);
    static void ProcessACCUM_NUMBER(uint8 *buffer);
    static void ProcessACCUM_MODE(uint8 *buffer);
    static void ProcessACCUM_CLEAR(uint8 *buffer);
    static void ProcessAVERAGE(uint8 *buffer);
    static void ProcessAVERAGE_NUMBER(uint8 *buffer);
    static void ProcessAVERAGE_MODE(uint8 *buffer);
    static void ProcessMINMAX(uint8 *buffer);
    static void ProcessDispFILTR(uint8 *buffer);
    static void ProcessFPS(uint8 *buffer);
    static void ProcessWINDOW(uint8 *buffer);
    static void ProcessGRID(uint8 *buffer);
    static void ProcessGRID_TYPE(uint8 *buffer);
    static void ProcessGRID_BRIGHTNESS(uint8 *buffer);
};




/** @}
 */
