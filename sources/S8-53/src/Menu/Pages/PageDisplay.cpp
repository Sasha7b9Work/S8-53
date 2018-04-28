#include "Menu/MenuItems.h"
#include "defines.h"
#include "Menu/Menu.h"
#include "Tables.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
//#include "FPGA/DataStorage.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageDisplay
 *  @{
 */

extern const Page pDisplay;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Choice mcMapping;                              ///<     ÄÈÑÏËÅÉ - Îòîáðàæåíèå

extern const Page mspAccumulation;                          ///<     ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ
static bool IsActive_Accumulation();                        ///< Àêòèâíà ëè ñòðàíèöà ÄÈÑÏËÅÉ-ÍÀÊÎÏËÅÍÈÅ
extern const Choice mcAccumulation_Number;                  ///< ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Êîëè÷åñòâî
extern const Choice mcAccumulation_Mode;                    ///< ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Ðåæèì
extern const Button mcAccumulation_Clear;                   ///< ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Î÷èñòèòü
static bool IsActive_Accumulation_Clear();                  ///< Àêòèâíà ëè êíîïêà ÄÈÑÏËÅÉ-ÍÀÊÎÏËÅÍÈÅ-Î÷èñòèòü
void OnPress_Accumulation_Clear();                          ///< Îáðàáîòêà íàæàòèÿ ÄÈÑÏËÅÉ-ÍÀÊÎÏËÅÍÈÅ-Î÷èñòèòü

extern const Page mspAveraging;                             ///<     ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ
static bool IsActive_Averaging();                           ///< Àêòèâíà ëè ñòðàíèöà ÄÈÑÏËÅÉ-ÓÑÐÅÄÍÅÍÈÅ
extern const Choice mcAveraging_Number;                     ///< ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Êîëè÷åñòâî
extern const Choice mcAveraging_Mode;                       ///< ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Ðåæèì

extern const Choice mcMinMax;                               ///<     ÄÈÑÏËÅÉ - Ìèí Ìàêñ
static bool IsActive_MinMax();                              ///< Àêòèâíà ëè íàñòðîéêà ÄÈÑÏËÅÉ-Ìèí Ìàêñ
static void OnChanged_MinMax(bool active);                  ///< Ðåàêöèÿ íà èçìåíåíèå ÄÈÑÏËÅÉ-Ìèì Ìàêñ

extern const Choice mcSmoothing;                            ///<     ÄÈÑÏËÅÉ - Ñãëàæèâàíèå

extern const Choice mcRefreshFPS;                           ///<     ÄÈÑÏËÅÉ - ×àñòîòà îáíîâë
void OnChanged_RefreshFPS(bool active);                     ///< Ðåàêöèÿ íà èçìåíåíèå ÄÈÑÏËÅÉ-×àñòîòà îáíîâë
extern const Page mspGrid;                                  ///<     ÄÈÑÏËÅÉ - ÑÅÒÊÀ

extern const Choice mcGrid_Type;                            ///<     ÄÈÑÏËÅÉ - ÑÅÒÊÀ - Òèï
extern const Governor mgGrid_Brightness;                    ///< ÄÈÑÏËÅÉ - ÑÅÒÊÀ - ßðêîñòü
void OnChanged_Grid_Brightness();                           ///< Ðåàêöèÿ íà èçìåíåíèå ÄÈÑÏËÅÉ-ÑÅÒÊÀ-ßðêîñòü
static void BeforeDraw_Grid_Brightness();                   ///< Âûçûâàåòñÿ ïåðåä èçìåíåíèåì ÄÈÑÏËÅÉ-ÑÅÒÊÀ-ßðêîñòü

extern const Choice mcTypeShift;                            ///<     ÄÈÑÏËÅÉ - Ñìåùåíèå

extern const Page mspSettings;                              ///<     ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ
extern const Page mspSettings_Colors;                       ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ
extern const Choice mcSettings_Colors_Scheme;               ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Öâåòîâàÿ ñõåìà
extern const GovernorColor mgcSettings_Colors_ChannelA;     ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 1
extern const GovernorColor mgcSettings_Colors_ChannelB;     ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 2
extern const GovernorColor mgcSettings_Colors_Grid;         ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Ñåòêà
extern const Governor mgSettings_Brightness;                ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ßðêîñòü
static void OnChanged_Settings_Brightness();                ///< Âûçûâàòåñÿ ïðè èçìåíåíèè ÄÈÑÏËÅÉ-ÍÀÑÒÐÎÉÊÈ-ßðêîñòü
extern const Governor mgSettings_Levels;                    ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Óðîâíè
extern const Governor mgSettings_TimeMessages;              ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Âðåìÿ
extern const Choice mcSettings_ShowStringNavigation;        ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñòðîêà ìåíþ
extern const Choice mcSettings_ShowAltMarkers;              ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Äîï. ìàðêåðû
static void OnChanged_Settings_ShowAltMarkers(bool);
extern const Choice mcSettings_AutoHide;                    ///< ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñêðûâàòü
static void OnChanged_Settings_AutoHide(bool autoHide);     ///< Âûçûâàåòñÿ ïðè èçìåíåíèè ÄÈÑÏËÅÉ-ÍÀÑÒÐÎÉÊÈ-Ñêðûâàòü


// ÄÈÑÏËÅÉ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;

static const arrayItems itemsDisplay =
{
    (void*)&mcMapping,              // ÄÈÑÏËÅÉ - Îòîáðàæåíèå
    (void*)&mspAccumulation,        // ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ
    (void*)&mspAveraging,           // ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ
    (void*)&mcMinMax,               // ÄÈÑÏËÅÉ - Ìèí Ìàêñ
    (void*)&mcSmoothing,            // ÄÈÑÏËÅÉ - Ñãëàæèâàíèå
    (void*)&mcRefreshFPS,           // ÄÈÑÏËÅÉ - ×àñòîòà îáíîâë
    (void*)&mspGrid,                // ÄÈÑÏËÅÉ - ÑÅÒÊÀ
    (void*)&mcTypeShift,            // ÄÈÑÏËÅÉ - Ñìåùåíèå
    (void*)&mspSettings             // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ
    //(void*)&mcDisplMemoryWindow,  // ÄÈÑÏËÅÉ - Îêíî ïàìÿòè
};

const Page pDisplay                 ///< ÄÈÑÏËÅÉ
(
    &mainPage, 0,
    "ÄÈÑÏËÅÉ", "DISPLAY",
    "Ñîäåðæèò íàñòðîéêè îòîáðàæåíèÿ äèñïëåÿ.",
    "Contains settings of display of the Display::",
    Page_Display, &itemsDisplay
);


// ÄÈÑÏËÅÉ - Îòîáðàæåíèå -----------------------------------------------------------------------------------------------------------------------------
static const Choice mcMapping =
{
    Item_Choice, &pDisplay, 0,
    {
        "Îòîáðàæåíèå", "View",
        "Çàäà¸ò ðåæèì îòîáðàæåíèÿ ñèãíàëà.",
        "Sets the display mode signal."
    },
    {
        {"Âåêòîð",  "Vector"},
        {"Òî÷êè",   "Points"}
    },
    (int8*)&MODE_DRAW_SIGNAL
};


// ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsAccumulation =
{
    (void*)&mcAccumulation_Number,  // ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Êîëè÷åñòâî
    (void*)&mcAccumulation_Mode,    // ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Ðåæèì
    (void*)&mcAccumulation_Clear    // ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Î÷èñòèòü    
};

static const Page mspAccumulation
(
    &pDisplay, IsActive_Accumulation,
    "ÍÀÊÎÏËÅÍÈÅ", "ACCUMULATION",
    "Íàñòðîéêè ðåæèìà îòîáðàæåíèÿ ïîñëåäíèõ ñèãíàëîâ íà ýêðàíå.",
    "Mode setting signals to display the last screen.",
    Page_DisplayAccumulation, &itemsAccumulation
);

static bool IsActive_Accumulation(void)
{
    return SET_TBASE > TBase_50ns;
}

// ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Êîëè÷åñòâî -----------------------------------------------------------------------------------------------------------------
static const Choice mcAccumulation_Number =
{
    Item_ChoiceReg, &mspAccumulation, 0,
    {
        "Êîëè÷åñòâî", "Number"
        ,
        "Çàäà¸ò ìàêñèìàëüíîå êîëè÷åñòâî ïîñëåäíèõ ñèãíàëîâ íà ýêðàíå. Åñëè â íàñòðîéêå \"Ðåæèì\" âûáðàíî \"Áåñêîíå÷íîñòü\", ýêðàí î÷èùàåòñÿ òîëüêî "
        "íàæàòèåì êíîïêè \"Î÷èñòèòü\"."
        "\"Áåñêîíå÷íîñòü\" - êàæäîå èçìåðåíèå îñòà¸òñÿ íà äèñïëåå äî òåõ ïîð, ïîêà íå áóäåò íàæàòà êíîïêà \"Î÷èñòèòü\"."
        ,
        "Sets the maximum quantity of the last signals on the screen. If in control \"Mode\" it is chosen \"Infinity\", the screen is cleared only "
        "by pressing of the button \"Clear\"."
        "\"Infinity\" - each measurement remains on the display until the button \"Clear\" is pressed."
    },
    {
        {DISABLE_RU,        DISABLE_EN},
        {"2",               "2"},
        {"4",               "4"},
        {"8",               "8"},
        {"16",              "16"},
        {"32",              "32"},
        {"64",              "64"},
        {"128",             "128"},
        {"Áåñêîíå÷íîñòü",   "Infinity"}
    },
    (int8*)&ENUM_ACCUM
};


// ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Ðåæèì ----------------------------------------------------------------------------------------------------------------------
static const Choice mcAccumulation_Mode =
{
    Item_Choice, &mspAccumulation, 0,
    {
        "Ðåæèì", "Mode"
        ,
        "1. \"Ñáðàñûâàòü\" - ïîñëå íàêîïëåíèÿ çàäàííîãî êîëè÷åñòâà èçìåðåíèÿ ïðîèñõîäèò î÷èñòêà äèñïëåÿ. Ýòîò ðåæèì óäîáåí, êîãäà ïàìÿòè íå õâàòàåò "
        "äëÿ ñîõðàíåíèÿ íóæíîãî êîëè÷åñòâà èçìåðåíèé.\n"
        "2. \"Íå ñáðàñûâàòü\" - íà äèñïëåé âñåãäà âûâîäèòñÿ çàäàííîå èëè ìåíüøåå (â ñëó÷àå íåõâàòêè ïàìÿòè) êîëè÷åñòâî èçìåðåíèé. Íåäîñòàòêîì ÿâëÿåòñÿ "
        "ìåíüøåå áûñòðîäåéñòâèå è íåâîçìîæíîñòü íàêîïëåíèÿ çàäàííîãî êîëè÷åñòâà èçìåðåíèé ïðè íåäîñòàòêå ïàìÿòè."
        ,
        "1. \"Dump\" - after accumulation of the set number of measurement there is a cleaning of the Display:: This mode is convenient when memory "
        "isn't enough for preservation of the necessary number of measurements.\n"
        "2. \"Not to dump\" - the number of measurements is always output to the display set or smaller (in case of shortage of memory). Shortcoming "
        "is smaller speed and impossibility of accumulation of the set number of measurements at a lack of memory."
    },
    {
        {"Íå ñáðàñûâàòü",   "Not to dump"},
        {"Ñáðàñûâàòü",      "Dump"}
    },
    (int8*)&MODE_ACCUM
};


// ÄÈÑÏËÅÉ - ÍÀÊÎÏËÅÍÈÅ - Î÷èñòèòü ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Button mcAccumulation_Clear
(
    &mspAccumulation, IsActive_Accumulation_Clear,
    "Î÷èñòèòü", "Clear",
    "Î÷èùàåò ýêðàí îò íàêîïëåííûõ ñèãíàëîâ.",
    "Clears the screen of the saved-up signals.",
    OnPress_Accumulation_Clear
);

static bool IsActive_Accumulation_Clear(void)
{
    return ENUM_ACCUM_IS_INFINITY;
}

void OnPress_Accumulation_Clear(void)
{
    Display::Redraw();
}


// ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsAveraging =
{
    (void*)&mcAveraging_Number, // ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Êîëè÷åñòâî
    (void*)&mcAveraging_Mode    // ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Ðåæèì    
};

static const Page mspAveraging
(
    &pDisplay, IsActive_Averaging,
    "ÓÑÐÅÄÍÅÍÈÅ", "AVERAGE",
    "Íàñòðîéêè ðåæèìà óñðåäíåíèÿ ïî ïîñëåäíèì èçìåðåíèÿì.",
    "Settings of the mode of averaging on the last measurements.",
    Page_DisplayAverage, &itemsAveraging
);

static bool IsActive_Averaging(void)
{
    return true;
}


// ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Êîëè÷åñòâî -----------------------------------------------------------------------------------------------------------------
static const Choice mcAveraging_Number =
{
    Item_ChoiceReg, &mspAveraging, 0,
    {
        "Êîëè÷åñòâî", "Number",
        "Çàäà¸ò êîëè÷åñòâî ïîñëåäíèõ èçìåðåíèé, ïî êîòîðûì ïðîèçâîäèòñÿ óñðåäíåíèå.",
        "Sets number of the last measurements on which averaging is made."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2",           "2"},
        {"4",           "4"},
        {"8",           "8"},
        {"16",          "16"},
        {"32",          "32"},
        {"64",          "64"},
        {"128",         "128"},
        {"256",         "256"},
        {"512",         "512"}
    },
    (int8*)&ENUM_AVE
};


// ÄÈÑÏËÅÉ - ÓÑÐÅÄÍÅÍÈÅ - Ðåæèì ----------------------------------------------------------------------------------------------------------------------
static const Choice mcAveraging_Mode =
{
    Item_Choice, &mspAveraging, 0,
    {
        "Ðåæèì", "Mode"
        ,
        "1. \"Òî÷íî\" - òî÷íûé ðåæèì óñðåäíåíèÿ, êîãäà â ðàñ÷¸òå ó÷àñòâóþò òîëüêî ïîñëåäíèå ñèãíàëû.\n"
        "2. \"Ïðèáëèçèòåëüíî\" - ïðèáëèçèòåëüíûé ðåæèì óñðåäíåíèÿ. Èìååò ñìûñë èñïîëüçîâàòü, êîãäà çàäàíî êîëè÷åñòâî èçìåðåíèé áîëüøåå, ÷åì ìîæåò "
        "ïîìåñòèòüñÿ â ïàìÿòè."
        ,
        "1. \"Accurately\" - the exact mode of averaging when only the last signals participate in calculation.\n"
        "2. \"Around\" - approximate mode of averaging. It makes sense to use when the number of measurements bigger is set, than can be located in "
        "memory."
    },
    {
        {"Òî÷íî",           "Accurately"},
        {"Ïðèáëèçèòåëüíî",  "Around"}
    },
    (int8*)&MODE_AVE
};


// ÄÈÑÏËÅÉ - Ìèí Ìàêñ --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcMinMax =
{
    Item_ChoiceReg, &pDisplay, IsActive_MinMax,
    {
        "Ìèí Ìàêñ", "Min Max"
        ,
        "Çàäà¸ò êîëè÷åñòâî ïîñëåäíèõ èçìåðåíèé, ïî êîòîðûì ñòðîÿòñÿ îãðàíè÷èòåëüíûå ëèíèè, îãèáàþùèå ìèíèìóìû è ìàêñèìóìû èçìåðåíèé."
        ,
        "Sets number of the last measurements on which the limiting lines which are bending around minima and maxima of measurements are under "
        "construction."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2",           "2"},
        {"4",           "4"},
        {"8",           "8"},
        {"16",          "16"},
        {"32",          "32"},
        {"64",          "64"},
        {"128",         "128"}
    },
    (int8*)&ENUM_MIN_MAX, OnChanged_MinMax
};

static bool IsActive_MinMax(void)
{
    return SET_TBASE > TBase_50ns;
}

static void OnChanged_MinMax(bool active)
{
    /*
    int maxMeasures = DS_NumberAvailableEntries();  
    int numMinMax = sDisplay_NumberMinMax();

    if (maxMeasures < numMinMax)
    {
        Display::ShowWarningWithNumber(ExcessValues, maxMeasures);
    }
    */
}



// ÄÈÑÏËÅÉ - Ñãëàæèâàíèå -----------------------------------------------------------------------------------------------------------------------------
static const Choice mcSmoothing =
{
    Item_ChoiceReg, &pDisplay, 0,
    {
        "Ñãëàæèâàíèå", "Smoothing",
        "Óñòàíàâëèâàåò êîëè÷åñòâî òî÷åê äëÿ ðàñ÷¸òà ñãëàæåííîãî ïî ñîñåäíèì òî÷êàì ñèãíàëà.",
        "Establishes quantity of points for calculation of the signal smoothed on the next points."
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {"2 òî÷êè",     "2 points"},
        {"3 òî÷êè",     "3 points"},
        {"4 òî÷êè",     "4 points"},
        {"5 òî÷åê",     "5 points"},
        {"6 òî÷åê",     "6 points"},
        {"7 òî÷åê",     "7 points"},
        {"8 òî÷åê",     "8 points"},
        {"9 òî÷åê",     "9 points"},
        {"10 òî÷åê",    "10 points"}
    },
    (int8*)&SMOOTHING
};


// ÄÈÑÏËÅÉ - ×àñòîòà îáíîâë --------------------------------------------------------------------------------------------------------------------------
static const Choice mcRefreshFPS =
{
    Item_Choice, &pDisplay, 0,
    {
        "×àñòîòà îáíîâë", "Refresh rate",
        "Çàäà¸ò ìàêñèìàëüíîå ÷èñëî âûâîäèìûõ â ñåêóíäó êàäðîâ.",
        "Sets the maximum number of the shots removed in a second."
    },
    {
        {"25",  "25"},
        {"10",  "10"},
        {"5",   "5"},
        {"2",   "2"},
        {"1",   "1"}
    },
    (int8*)&ENUM_SIGNALS_IN_SEC, OnChanged_RefreshFPS
};

void OnChanged_RefreshFPS(bool active)
{
    FPGA::SetNumSignalsInSec(sDisplay_NumSignalsInS());
}


// ÄÈÑÏËÅÉ - ÑÅÒÊÀ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsGrid =
{
    (void*)&mcGrid_Type,        // ÄÈÑÏËÅÉ - ÑÅÒÊÀ - Òèï
    (void*)&mgGrid_Brightness   // ÄÈÑÏËÅÉ - ÑÅÒÊÀ - ßðêîñòü
};

static const Page mspGrid
(
    &pDisplay, 0,
    "ÑÅÒÊÀ", "GRID",
    "Ñîäåðæèò íàñòðîéêè îòîáðàæåíèÿ êîîðäèíàòíîé ñåòêè.",
    "Contains settings of display of a coordinate Grid::",
    Page_DisplayGrid, &itemsGrid
);


// ÄÈÑÏËÅÉ - ÑÅÒÊÀ - Òèï -----------------------------------------------------------------------------------------------------------------------------
static const Choice mcGrid_Type =
{
    Item_Choice, &mspGrid, 0,
    {
        "Òèï", "Type",
        "Âûáîð òèïà ñåòêè.",
        "Choice like Grid::"
    },
    {
        {"Òèï 1",   "Type 1"},
        {"Òèï 2",   "Type 2"},
        {"Òèï 3",   "Type 3"},
        {"Òèï 4",   "Type 4"}
    },
    (int8*)&TYPE_GRID
};

// ÄÈÑÏËÅÉ - ÑÅÒÊÀ - ßðêîñòü -------------------------------------------------------------------------------------------------------------------------
static const Governor mgGrid_Brightness
(
    &mspGrid, 0,
    "ßðêîñòü", "Brightness",
    "Óñòàíàâëèâàåò ÿðêîñòü ñåòêè.",
    "Adjust the brightness of the Grid::",
    &BRIGHTNESS_GRID, 0, 100, OnChanged_Grid_Brightness, BeforeDraw_Grid_Brightness
);

ColorType colorTypeGrid = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_GRID};

void OnChanged_Grid_Brightness(void)
{
    Color_SetBrightness(&colorTypeGrid, BRIGHTNESS_GRID / 1e2f);
}

static void BeforeDraw_Grid_Brightness(void)
{
    Color_Init(&colorTypeGrid);
    BRIGHTNESS_GRID = (int16)(colorTypeGrid.brightness * 100.0f);
}


// ÄÈÑÏËÅÉ - Ñìåùåíèå --------------------------------------------------------------------------------------------------------------------------------
static const Choice mcTypeShift =
{
    Item_Choice, &pDisplay, 0,
    {
        "Ñìåùåíèå", "Offset"
        ,
        "Çàäà¸ò ðåæèì óäåðæàíèÿ ñìåùåíèÿ ïî âåðòèêàëè\n1. \"Íàïðÿæåíèå\" - ñîõðàíÿåòñÿ íàïðÿæåíèå ñìåùåíèÿ.\n2. \"Äåëåíèÿ\" - ñîõðàíÿåòñÿ ïîëîæåíèå "
        "ñìåùåíèÿ íà ýêðàíå."
        ,
        "Sets the mode of retaining the vertical displacement\n1. \"Voltage\" - saved dressing bias.\n2. \"Divisions\" - retained the position of "
        "the offset on the screen."
    },
    {
        {"Íàïðÿæåíèå", "Voltage"},
        {"Äåëåíèÿ", "Divisions"}
    },
    (int8*)&LINKING_RSHIFT
};


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsSettings =
{
    (void*)&mspSettings_Colors,                 // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ
    (void*)&mgSettings_Brightness,              // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ßðêîñòü
    (void*)&mgSettings_Levels,                  // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Óðîâíè
    (void*)&mgSettings_TimeMessages,            // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Âðåìÿ
    (void*)&mcSettings_ShowStringNavigation,    // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñòðîêà ìåíþ
    (void*)&mcSettings_ShowAltMarkers,          // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Äîï. ìàðêåðû
    (void*)&mcSettings_AutoHide                 // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñêðûâàòü
};

static const Page mspSettings
(
    &pDisplay, 0,
    "ÍÀÑÒÐÎÉÊÈ", "SETTINGS",
    "Äîïîëíèòåëüíûå íàñòðîéêè äèñïëåÿ",
    "Additional display settings",
    Page_ServiceDisplay, &itemsSettings
);


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsSettings_Colors =
{
    (void*)&mcSettings_Colors_Scheme,       // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Öâåòîâàÿ ñõåìà
    (void*)&mgcSettings_Colors_ChannelA,    // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 1
    (void*)&mgcSettings_Colors_ChannelB,    // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 2
    (void*)&mgcSettings_Colors_Grid,        // ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Ñåòêà
    //(void*)&mgcColorChannel0alt,          // TODO Ýòî äëÿ äîïîëíèòåëüíîãî öâåòà ïåðâîãî êàíàëà - äëÿ íàêîïëåíèÿ, íàïðèìåð.
    //(void*)&mgcColorChannel1alt,          // TODO Ýòî äëÿ äîï. öâåòà âòîðîãî êàíàëà - äëÿ íàêîïëåíèÿ, íàïðèìåð.    
};

static const Page mspSettings_Colors
(
    &mspSettings, 0,
    "ÖÂÅÒÀ", "COLORS",
    "Âûáîð öâåòîâ äèñïëåÿ",
    "The choice of colors display",
    Page_ServiceDisplayColors, &itemsSettings_Colors
);


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Öâåòîâàÿ ñõåìà ------------------------------------------------------------------------------------------------------
static const Choice mcSettings_Colors_Scheme =
{
    Item_Choice, &mspSettings_Colors, 0,
    {
        "Öâåòîâàÿ ñõåìà", "Color scheme",
        "Ðåæèì ðàáîòû êàëèáðàòîðà",
        "Mode of operation of the calibrator"
    },
    {   
        {"Ñõåìà 1", "Scheme 1"},
        {"Ñõåìà 2", "Scheme 2"}
    },
    (int8*)&COLOR_SCHEME
};

static ColorType colorT1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_A};

// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 1 -------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_ChannelA =
{
    Item_GovernorColor, &mspSettings_Colors, 0,
    {
        "Êàíàë 1", "Channel 1",
        "",
        ""
    },
    &colorT1
};

static ColorType colorT2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, COLOR_DATA_B};

// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Êàíàë 2 -------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_ChannelB =
{
    Item_GovernorColor, &mspSettings_Colors, 0,
    {
        "Êàíàë 2", "Channel 2",
        "",
        ""
    },
    &colorT2
};


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ÖÂÅÒÀ - Ñåòêà ---------------------------------------------------------------------------------------------------------------
static const GovernorColor mgcSettings_Colors_Grid =
{
    Item_GovernorColor, &mspSettings_Colors, 0,
    {
        "Ñåòêà", "Grid",
        "Óñòàíàâëèâàåò öâåò ñåòêè",
        "Sets the grid color"
    },
    &colorTypeGrid
};


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - ßðêîñòü ---------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Brightness
(
    &mspSettings, 0,
    "ßðêîñòü", "Brightness",
    "Óñòàíîâêà ÿðêîñòè ñâå÷åíèÿ äèñïëåÿ",
    "Setting the brightness of the display",
    &BRIGHTNESS, 0, 100, OnChanged_Settings_Brightness
);

static void OnChanged_Settings_Brightness(void)
{
    Painter::SetBrightnessDisplay(BRIGHTNESS);
}


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Óðîâíè ----------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_Levels
(
    &mspSettings, 0,
    "Óðîâíè", "Levels",
    "Çàäà¸ò âðåìÿ, â òå÷åíèå êîòîðîãî ïîñëå ïîâîðîòà ðó÷êè ñåùåíèÿ íàïðÿæåíèÿ íà ýêðàíå îñòà¸òñÿ âñïîìîãàòåëüíàÿ ìåòêà óðîâíÿ ñìåùåíèÿ",
    "Defines the time during which, after turning the handle visits to the voltage on the screen remains auxiliary label offset level",
    &TIME_SHOW_LEVELS, 0, 125
);


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Âðåìÿ -----------------------------------------------------------------------------------------------------------------------
static const Governor mgSettings_TimeMessages
(
    &mspSettings, 0,
    "Âðåìÿ", "Time",
    "Óñòàíîâêà âðåìåíè, â òå÷åíèå êîòîðîãî ñîîáùåíèÿ áóäóò íàõîäèòüñÿ íà ýêðàíå",
    "Set the time during which the message will be on the screen",
    &TIME_MESSAGES, 1, 99
);


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñòðîêà ìåíþ -----------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_ShowStringNavigation =
{
    Item_Choice, &mspSettings, 0,
    {
        "Ñòðîêà ìåíþ", "Path menu",
        "Ïðè âûáîðå \nÏîêàçûâàòü\n ñëåâà ââåðõó ýêðàíà âûâîäèòñÿ ïîëíûé ïóòü äî òåêóùåé ñòðàíèöû ìåíþ",
        "When choosing \nDisplay\n at the top left of the screen displays the full path to the current page menu"
    },
    {   
        {"Ñêðûâàòü",    "Hide"},
        {"Ïîêàçûâàòü",  "Show"}
    },
    (int8*)&SHOW_STRING_NAVIGATION
};


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Äîï. ìàðêåðû ----------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_ShowAltMarkers =
{
    Item_Choice, &mspSettings, 0,
    {
        "Äîï. ìàðêåðû", "Alt. markers"
        ,
        "Óñòàíàâëèâàåò ðåæèì îòîáðàæåíèÿ äîïîëíèòåëüíûõ ìàðêåðîâ óðîâíåé ñìåùåíèÿ è ñèíõðîíèçàöèè:\n"
        "\"Ñêðûâàòü\" - äîïîëíèòåëüíûå ìàðêåðû íå ïîêàçûâàþòñÿ,\n"
        "\"Ïîêàçûâàòü\" - äîïîëíèòåëüíûå ìàðêåðû ïîêàçûâàþòñÿ âñåãäà,\n"
        "\"Àâòî\" - äîïîëíèòåëüíûå ìàðêåðû ïîêàçûâàþòñÿ â òå÷åíèå 5 ñåê ïîñëå ïîâîðîòà ðó÷êè ñìåùåíèÿ êàíàëà ïî íàïðÿæåíèþ èëè óðîâíÿ ñèíõðîíèçàöèè"
        ,
        "Sets the display mode of additional markers levels of displacement and synchronization:\n"
        "\"Hide\" - additional markers are not shown,\n"
        "\"Show\" - additional markers are shown always,\n"
        "\"Auto\" - additional markers are displayed for 5 seconds after turning the handle channel offset voltage or trigger level"
    },
    {   
        {"Ñêðûâàòü",    "Hide"},
        {"Ïîêàçûâàòü",  "Show"},
        {"Àâòî",        "Auto"}
    },
    (int8*)&ALT_MARKERS, OnChanged_Settings_ShowAltMarkers
};

static void OnChanged_Settings_ShowAltMarkers(bool)
{
    Display::ChangedRShiftMarkers();
}


// ÄÈÑÏËÅÉ - ÍÀÑÒÐÎÉÊÈ - Ñêðûâàòü --------------------------------------------------------------------------------------------------------------------
static const Choice mcSettings_AutoHide =
{
    Item_Choice, &mspSettings, 0,
    {
        "Ñêðûâàòü", "Hide",
        "Óñòàíîâêà ïîñëå ïîñëåäíåãî íàæàòèÿ êíîïêè èëè ïîâîðîòà ðó÷êè, ïî èñòå÷åíèè êîòîðîãî ìåíþ àâòîìàòè÷åñêè óáèðàåòñÿ ñ ýêðàíà",
        "Installation after the last keystroke or turning the handle, after which the menu will automatically disappear"
    },
    {   
        {"Íèêîãäà",         "Never"},
        {"×åðåç 5 ñåê",     "Through 5 s"},
        {"×åðåç 10 ñåê",    "Through 10 s"},
        {"×åðåç 15 ñåê",    "Through 15 s"},
        {"×åðåç 30 ñåê",    "Through 30 s"},
        {"×åðåç 60 ñåê",    "Through 60 s"}
    },
    (int8*)&MENU_AUTO_HIDE, OnChanged_Settings_AutoHide
};

static void OnChanged_Settings_AutoHide(bool autoHide)
{
    Menu::SetAutoHide(autoHide);
}


// ÄÈÑÏËÅÉ - Îêíî ïàìÿòè /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
static const Choice mcDisplMemoryWindow =
{
    Item_Choice, &pDisplay, 0,
    {
        "Îêíî ïàìÿòè", "Window memory",
        "1. \"Ñòàíäàðòíîå\" - â âåðõíåé ÷àñòè ýêðàíà âûâîäèòñÿ ñîäåðæèìîå ïàìÿòè.\n2. \"Óïðîù¸ííîå\" - âûâîäèòñÿ ïîëîæåíèå âèäèìîãî îêíà â ïàìÿòè.",
        "1. \"Standard\" - in the top part of the screen memory contents are removed.\n2. \"Simplified\" - shows the position of the visible window in memory."
    },
    {
        {"Óïðîù¸ííîå",  "Simplified"},
        {"Ñòàíäàðòíîå", "Standard"}
    },
    (int8*)&set.display.showFullMemoryWindow
};
*/


/** @}  @}
 */
