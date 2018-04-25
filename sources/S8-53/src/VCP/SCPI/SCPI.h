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
    if(CLIENT_VCP_IS_CONNECTED)                             \
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
    static void AddNewData(uint8 *buffer, uint length);

private:
    static void ParseNewCommand(uint8 *buffer);   ///< \todo Временно. Потом доделать
    static void ProcessingCommand(const StructCommand *commands, uint8 *buffer);
    static bool FirstIsInt(uint8 *buffer, int *value, int min, int max);
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

    class DISPLAY
    {
    public:
        static void AUTOSEND(uint8 *buffer);
        static void MAPPING(uint8 *buffer);
        static void ACCUM(uint8 *buffer);
        static void ACCUM_NUMBER(uint8 *buffer);
        static void ACCUM_MODE(uint8 *buffer);
        static void ACCUM_CLEAR(uint8 *buffer);
        static void AVERAGE(uint8 *buffer);
        static void AVERAGE_NUMBER(uint8 *buffer);
        static void AVERAGE_MODE(uint8 *buffer);
        static void MINMAX(uint8 *buffer);
        static void FILTR(uint8 *buffer);
        static void FPS(uint8 *buffer);
        static void WINDOW(uint8 *buffer);
        static void GRID(uint8 *buffer);
        static void GRID_TYPE(uint8 *buffer);
        static void GRID_BRIGHTNESS(uint8 *buffer);
    };

    class TBASE
    {
    public:
        static void RANGE(uint8 *buffer);
        static void OFFSET(uint8 *buffer);
        static void SAMPLING(uint8 *buffer);
        static void PEACKDET(uint8 *buffer);
        static void TPOS(uint8 *buffer);
        static void SELFRECORDER(uint8 *buffer);
        static void FUNCTIMEDIV(uint8 *buffer);
    };

    class TRIGGER
    {
    public:
        static void MODE(uint8 *buffer);
        static void SOURCE(uint8 *buffer);
        static void POLARITY(uint8 *buffer);
        static void INPUT(uint8 *buffer);
        static void FIND(uint8 *buffer);
        static void OFFSET(uint8 *buffer);
    };
};


/** @}
 */
