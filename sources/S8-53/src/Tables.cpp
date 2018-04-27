#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Log.h"
#include "Tables.h"
#include "Utils/GlobalFunctions.h"
#include "Display/font/Font.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Ñòğóêòóğà äëÿ îïèñàíèÿ äèàïàçîíà ìàñøòàáà ïî âğåìåíè.
typedef struct
{
    const char* name[2];    ///< Íàçâàíèå äèàïàçîíà â òåêñòîâîì âèäå, ïğèãîäíîì äëÿ âûâîäà íà ıêğàí.
} TBaseStruct;


/// Ìàññèâ ñòğóêòóğ îïèñàíèé ìàñøòàáîâ ïî âğåìåíè.
static const TBaseStruct tBases[TBaseSize] =
{
    {"2\x10íñ",     "2\x10ns"},
    {"5\x10íñ",     "5\x10ns"},
    {"10\x10íñ",    "10\x10ns"},
    {"20\x10íñ",    "20\x10ns"},
    {"50\x10íñ",    "50\x10ns"},
    {"0.1\x10ìêñ",  "0.1\x10us"},
    {"0.2\x10ìêñ",  "0.2\x10us"},
    {"0.5\x10ìêñ",  "0.5\x10us"},
    {"1\x10ìêñ",    "1\x10us"},
    {"2\x10ìêñ",    "2\x10us"},
    {"5\x10ìêñ",    "5\x10us"},
    {"10\x10ìêñ",   "10\x10us"},
    {"20\x10ìêñ",   "20\x10us"},
    {"50\x10ìêñ",   "50\x10us"},
    {"0.1\x10ìñ",   "0.1\x10ms"},
    {"0.2\x10ìñ",   "0.2\x10ms"},
    {"0.5\x10ìñ",   "0.5\x10ms"},
    {"1\x10ìñ",     "1\x10ms"},
    {"2\x10ìñ",     "2\x10ms"},
    {"5\x10ìñ",     "5\x10ms"},
    {"10\x10ìñ",    "10\x10ms"},
    {"20\x10ìñ",    "20\x10ms"},
    {"50\x10ìñ",    "50\x10ms"},
    {"0.1\x10ñ",    "0.1\x10s"},
    {"0.2\x10ñ",    "0.2\x10s"},
    {"0.5\x10ñ",    "0.5\x10s"},
    {"1\x10ñ",      "1\x10s"},
    {"2\x10ñ",      "2\x10s"},
    {"5\x10ñ",      "5\x10s"},
    {"10\x10ñ",     "10\x10s"}
};

/// Òàáëèöà ñèìâîëîâ àëôàâèòà è ñïåöñèìâîëîâ äëÿ ââîäà òåêñòà
const char* symbolsAlphaBet[0x48] =
{
    /* 0x00 */ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0x19 */
    /* 0x1a */ " ", "_", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /* 0x26 */
    /* 0x27 */ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0x40 */
    //         0x41  0x42  0x43  0x44  0x45  0x46   0x47   - ıòî ïîğÿäêîâûé íîìåğ ıëåìåíòà â ıòîì ìàññèâå
    /* 0x41 */ "%y", "%m", "%d", "%H", "%M", "%S", "%N" /* 0x47 */
    //         0x01  0x02  0x03  0x04  0x05  0x06   0x07   - ïîä ıòèìè çíà÷åíèÿìè ıëåìåíòû õğàíÿòñÿ â set.memory.fileNameMask
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ENumSignalsInSec Tables_NumSignalsInSecToENUM(int enumSignalsInSec)
{
    if(enumSignalsInSec == 1)
    {
        return NumSignalsInSec_1;
    }
    else if(enumSignalsInSec == 2)
    {
        return NumSignalsInSec_2;
    }
    else if(enumSignalsInSec == 5)
    {
        return NumSignalsInSec_5;
    }
    else if(enumSignalsInSec == 10)
    {
        return NumSignalsInSec_10;
    }
    else if(enumSignalsInSec == 25)
    {
        return NumSignalsInSec_25;
    }
    LOG_ERROR("×èñëî ñèãíàëîâ â ñåêóíäó ğàâíî %d", enumSignalsInSec);
    return NumSignalsInSec_1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_ENUMtoNumSignalsInSec(ENumSignalsInSec enumSignalsInSec)
{
    static const int fps[] = {25, 10, 5, 2, 1};
    if(enumSignalsInSec <= NumSignalsInSec_1)
    {
        return fps[enumSignalsInSec];
    }
    LOG_ERROR("Íåïğàâèëüíûé ïàğàìåòğ %d", (int)enumSignalsInSec);
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_GetWarning(Warning warning)
{
    static const char *warnings[Warning_NumWarnings][2] =
    {
        {"ÏĞÅÄÅË ÊÀÍÀË 1 - ÂÎËÜÒ/ÄÅË",      "LIMIT CHANNEL 1 - VOLTS/DIV"},
        {"ÏĞÅÄÅË ÊÀÍÀË 2 - ÂÎËÜÒ/ÄÅË",      "LIMIT CHANNEL 2 - VOLTS/DIV"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - ÂĞÅÌß/ÄÅË",    "LIMIT SWEEP - TIME/DIV"},
        {"ÂÊËŞ×ÅÍ ÏÈÊÎÂÛÉ ÄÅÒÅÊÒÎĞ",        "SET_ENABLED PEAK. DET."},
        {"ÏĞÅÄÅË ÊÀÍÀË 1 - \x0d",           "LIMIT CHANNEL 1 - \x0d"},
        {"ÏĞÅÄÅË ÊÀÍÀË 2 - \x0d",           "LIMIT CHANNEL 2 - \x0d"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - ÓĞÎÂÅÍÜ",      "LIMIT SWEEP - LEVEL"},
        {"ÏĞÅÄÅË ĞÀÇÂÅĞÒÊÀ - \x97",         "LIMIT SWEEP - \x97"},
        {"ÏÈÊ. ÄÅÒ. ÍÅ ĞÀÁÎÒÀÅÒ ÍÀ ĞÀÇÂÅĞÒÊÀÕ ÌÅÍÅÅ 0.5ìêñ/äåë", "PEAK. DET. NOT WORK ON SWEETS LESS THAN 0.5us/div"},
        {"ÑÀÌÎÏÈÑÅÖ ÍÅ ÌÎÆÅÒ ĞÀÁÎÒÀÒÜ ÍÀ ÁÛÑÒĞÛÕ ĞÀÇÂ¨ĞÒÊÀÕ", "SELF-RECORDER DOES NOT WORK FAST SCANS"},
        {"ÔÀÉË ÑÎÕĞÀÍÅÍ",                   "FILE IS SAVED"},
        {"ÑÈÃÍÀË ÑÎÕĞÀÍÅÍ",                 "SIGNAL IS SAVED"},
        {"ÑÈÃÍËÀ ÓÄÀËÅÍ",                   "SIGNAL IS DELETED"},
        {"ÌÅÍŞ ÎÒËÀÄÊÀ ÂÊËŞ×ÅÍÎ",           "MENU DEBUG IS SET_ENABLED"},
        {"ÏÎÄÎÆÄÈÒÅ ÍÅÑÊÎËÜÊÎ ÑÅÊÓÍÄ, ÈÄÅÒ ÏÅĞÅÌÅÙÅÍÈÅ ÄÀÍÍÛÕ", "WAIT A FEW SECONDS, GOES A MOVEMENT DATA"},
        {"ÂĞÅÌß ÍÅ ÓÑÒÀÍÎÂËÅÍÎ. ÌÎÆÅÒÅ ÓÑÒÀÍÎÂÈÒÜ ÅÃÎ ÑÅÉ×ÀÑ", "TIME IS NOT SET. YOU CAN INSTALL IT NOW"},
        {"ÑÈÃÍÀË ÍÅ ÍÀÉÄÅÍ",                "SIGNAL IS NOT FOUND"},
        {"ÍÀ ĞÀÇÂÅĞÒÊÀÕ ÌÅÄËÅÍÍÅÅ 10ìñ/äåë ÆÅËÀÒÅËÜÍÎ ÓÑÒÀÍÀÂ-", "AT SCANNER SLOWLY 10ms/div DESIRABLY SET \"SCAN - \x7b\" IN"},
        {"ËÈÂÀÒÜ \"ĞÀÇÂÅĞÒÊÀ - \x7b\" Â ÏÎËÎÆÅÍÈÅ \"Ëåâî\" ÄËß ÓÑÊÎĞÅ-", "SWITCH \"Left\" FOR TO ACCELERATE THE OUTPUT SIGNAL"},
        {"ÍÈß ÂÛÂÎÄÀ ÑÈÃÍÀËÀ",              ""},
        {"Î×ÅÍÜ ÌÀËÎ ÈÇÌÅĞÅÍÈÉ",            "VERY SMALL MEASURES"},
        {"ÄËß ÂÑÒÓÏËÅÍÈß ÈÇÌÅÍÅÍÈÉ Â ÑÈËÓ", "FOR THE INTRODUCTION OF CHANGES"},
        {"ÂÛÊËŞ×ÈÒÅ ÏĞÈÁÎĞ",                "IN FORCE SWITCH OFF THE DEVICE"},
        {"ÎÒÊËŞ×ÈÒÅ ÂÛ×ÈÑËÅÍÈÅ ÁÏÔ",        "DISCONNECT CALCULATION OF FFT"},
        {"ÎÒÊËŞ×ÈÒÅ ÌÀÒÅÌÀÒÈ×ÅÑÊÓŞ ÔÓÍÊÖÈŞ", "DISCONNECT MATHEMATICAL FUNCTION"},
        {"ÏĞÎØÈÂÊÀ ÑÎÕĞÀÍÅÍÀ",              "FIRMWARE SAVED"},
        {"Ïàìÿòü OTP ïîëíîñòüş çàïîëåíà",   "OPT memory fully completed"}
    };
    return warnings[warning][set.common.lang];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_GetTBaseString(TBase tBase)
{
    return tBases[tBase].name[set.common.lang];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_GetTBaseStringEN(TBase tBase)
{
    return tBases[tBase].name[English];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* Tables_RangeNameFromValue(Range range)
{
    static const char* names[RangeSize] =
    {
        "Range_2mV",
        "Range_5mV",
        "Range_10mV",
        "Range_20mV",
        "Range_50mV",
        "Range_100mV",
        "Range_200mV",
        "Range_500mV",
        "Range_1V",
        "Range_2V",
        "Range_5V",
        "Range_10V",
        "Range_20V"
    };
    return names[range];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Tables_GetNumChannel(Channel chan)
{
    return (chan == A) ? 1 : 2;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawStr(int index, int x, int y)
{
    const char *str = symbolsAlphaBet[index];
    if (index == INDEX_SYMBOL)
    {
        Painter::FillRegionC(x - 1, y, Font_GetLengthText(str), 9, COLOR_FLASH_10);
    }
    Painter::DrawTextC(x, y, symbolsAlphaBet[index], index == INDEX_SYMBOL ? COLOR_FLASH_01 : COLOR_FILL);
}
