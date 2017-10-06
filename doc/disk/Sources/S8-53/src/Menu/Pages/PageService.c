#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/FLASH.h"
#include "Settings/Settings.h"
#include "Menu/Menu.h"
#include "Menu/MenuDrawing.h"
#include "Menu/MenuFunctions.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Display/Colors.h"


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageService
 *  @{
 */


static const Button bResetSettings;            ///< СЕРВИС - Сброс настроек
static const Button bAutoSearch;               ///< СЕРВИС - Поиск сигнала
static const Page pCalibrator;                 ///< СЕРВИС - КАЛИБРАТОР
static const Page pMath;                       ///< СЕРВИС - МАТЕМАТИКА
static const Page pEthernet;                   ///< СЕРВИС - ETHERNET
static const Choice cSound;                    ///< СЕРВИС - Звук
static const Choice cLang;                     ///< СЕРВИС - Язык
static const Time tTime;                       ///< СЕРВИС - Время
static const Choice cModeLongPressButtonTrig; ///< СЕРВИС - Реж длит СИНХР
static const Page pInformation;                ///< СЕРВИС - ИНФОРМАЦИЯ


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern Page mainPage;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnTimerDraw()
{
    Display_Update();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncDraw()
{
    Painter_BeginScene(COLOR_BACK);

    Painter_DrawTextInRectWithTransfersC(30, 110, 300, 200, "Подтвердите сброс настроек нажатием кнопки ПУСК/СТОП.\n"
                                        "Нажмите любую другую кнопку, если сброс не нужен.", COLOR_FILL);

    Painter_EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPressResetSettings()
{
    Panel_Disable();
    Display_SetDrawMode(DrawMode_Hand, FuncDraw);
    Timer_Enable(kTimerDrawHandFunction, 100, OnTimerDraw);

    if (Panel_WaitPressingButton() == B_Start)
    {
        Settings_Load(true);
    }

    Timer_Disable(kTimerDrawHandFunction);
    Display_SetDrawMode(DrawMode_Auto, 0);
    Panel_Enable();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnChangedModeCalibrator(bool active)
{
    FPGA_SetCalibratorMode(CALIBRATOR);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnPressPrevSettings()
{

}


// СЕРВИС //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mpService;

// СЕРВИС - Найти сигнал
static void StartAutoSearch(void)
{
    FPGA_StartAutoFind();
};

// СЕРВИС - Откат настроек
const Button mbServicePreviousSettings =
{
    Item_Button, &mpService, 0,
    {
        "Откат настроек", "Return settings",
        "Возвращает настройки осциллографа, которые были в момент нажатия \"Поиск сигнала\"",
        "Returns the oscilloscope settings that were in when you press \"Searching for signal\""
    },
    OnPressPrevSettings
};

// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pCalibrator;

// СЕРВИС - КАЛИБРАТОР - Калибровать

void OnPressCalibration()
{
    gStateFPGA.needCalibration = true;
}

const Button mbServiceCalibration =
{
    Item_Button, &pCalibrator, 0,
    {
        "Калибровать", "Calibrate",
        "Запуск процедуры калибровки",
        "Running the calibration procedure"
    },
    OnPressCalibration
};

// СЕРВИС - КАЛИБРАТОР - Режим
const Choice mcServiceModeCalibrator =
{
    Item_Choice, &pCalibrator, 0,
    {
        "Калибратор",  "Calibrator",
        "Режим работы калибратора",
        "Mode of operation of the calibrator"
    },
    {
        {"Перем",   "DC"},
        {"Пост",    "AC"},
        {"0В",      "OV"}
    },
    (int8*)&CALIBRATOR, OnChangedModeCalibrator
};

// СЕРВИС - ДИСПЛЕЙ - ЦВЕТА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspSettings_Colors;


// СЕРВИС - ДИСПЛЕЙ - СООБЩЕНИЯ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspServDisplMess;
        
// СЕРВИС - ЗВУК //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Page mspServiceSound;

#include "PageServiceMath.h"

void ReInitAllColorGovernor()
{
    for(int i = 0; i < MAX_NUM_ITEMS_IN_PAGE; i++)
    {
        void *item = Item(&mspSettings_Colors, i);
        if(item && (TypeMenuItem(item) == Item_GovernorColor))
        {
            GovernorColor *governor = (GovernorColor*)item;
            governor->colorType->alreadyUsed = false;
            Color_Init(governor->colorType);
        }
    }
}

const Page pEthernet;

static void FuncOfChangedEthernetSettings(bool active)
{
    Display_ShowWarningGood(NeedRebootDevice2);
    Display_ShowWarningGood(NeedRebootDevice1);
}

static const Choice mcServEthEnable =
{
    Item_Choice, &pEthernet, 0,
    {
        "Ethernet", "Ethernet"
        ,
        "Чтобы задействовать ethernet, выберите \"Включено\" и выключите прибор.\n"
        "Чтобы отключить ethernet, выберите \"Отключено\" и выключите прибор."
        ,
        "To involve ethernet, choose \"Included\" and switch off the device.\n"
        "To disconnect ethernet, choose \"Disconnected\" and switch off the device."
    },
    {
        {"Включено",    "Included"},
        {"Отключено",   "Disconnected"}
    },
    (int8*)&ETH_ENABLE, FuncOfChangedEthernetSettings
};

static const IPaddress ipAddress =
{
    Item_IP, &pEthernet, 0,
    {
        "IP адрес", "IP-address",
        "Установка IP адреса",
        "Set of IP-address"
    },
    &IP_ADDR0, &IP_ADDR1, &IP_ADDR2, &IP_ADDR3,
    FuncOfChangedEthernetSettings,
    &PORT
};

static const IPaddress ipNetMask =
{
    Item_IP, &pEthernet, 0,
    {
        "Маска подсети", "Network mask",
        "Установка маски подсети",
        "Set of network mask"
    },
    &NETMASK_ADDR0, &NETMASK_ADDR1, &NETMASK_ADDR2, &NETMASK_ADDR3,
    FuncOfChangedEthernetSettings
};

static const IPaddress ipGateway =
{
    Item_IP, &pEthernet, 0,
    {
        "Шлюз", "Gateway",
        "Установка адреса основного шлюза",
        "Set of gateway address"
    },
    &GW_ADDR0, &GW_ADDR1, &GW_ADDR2, &GW_ADDR3,
    FuncOfChangedEthernetSettings
};

static const MACaddress macMAC =
{
    Item_MAC, &pEthernet, 0,
    {
        "Физ адрес", "MAC-address",
        "Установка физического адреса",
        "Set of MAC-address"
    },
    &MAC_ADDR0, &MAC_ADDR1, &MAC_ADDR2, &MAC_ADDR3, &MAC_ADDR4, &MAC_ADDR5,
    FuncOfChangedEthernetSettings
};

const Page pInformation;

static void PressSB_Information_Exit()
{
    Display_SetDrawMode(DrawMode_Auto, 0);
    Display_RemoveAddDrawFunction();
}

const SmallButton sbExitInformation =
{
    Item_SmallButton, &pInformation,
    COMMON_BEGIN_SB_EXIT,
    PressSB_Information_Exit,
    DrawSB_Exit
};

static void Information_Draw()
{
    Painter_BeginScene(COLOR_BACK);
    int x = 70;
    int y = 40;
    int width = SCREEN_WIDTH - 2 * x;
    int height = 120;
    Painter_FillRegionC(x, y, width, height, COLOR_GRID);
    Painter_DrawRectangleC(x, y, width, height, COLOR_FILL);
    Painter_DrawStringInCenterRect(x, y, width, 30, LANG_RU ? "ИНФОРМАЦИЯ" : "INFORMATION");
    Painter_DrawText(x + 20, y + 30, LANG_RU ? "Модель С8-53/1" : "Model S8-53/1");
    char buffer[100];
    OTP_GetSerialNumber(buffer);
    if (buffer[0])
    {
        Painter_DrawFormText(x + 20, y + 42, COLOR_FILL, (LANG_RU ? "Серийный номер: %s" : "Serial number: %s"), buffer);
    }
    Painter_DrawStringInCenterRect(x, y + 50, width, 30, LANG_RU ? "Программное обеспечение:" : "Software:");
    sprintf(buffer, LANG_RU ? "версия %s" : "version %s", NUM_VER);
    Painter_DrawText(x + 20, y + 79, buffer);
    Painter_DrawText(x + 20, y + 95, "CRC32 A1C8760F");

    int dY = -10;
    Painter_DrawStringInCenterRect(0, 190 + dY, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter_DrawStringInCenterRect(0, 205 + dY, 320, 20, "Отдел маркетинга: тел./факс. 8-017-262-57-50");
    Painter_DrawStringInCenterRect(0, 220 + dY, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-262-57-51");

    Menu_Draw();
    Painter_EndScene();
}

static void OnPressInformation()
{
    OpenPageAndSetItCurrent(Page_SB_Information);
    Display_SetDrawMode(DrawMode_Hand, Information_Draw);
}

// СЕРВИС ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page mpService =                   ///< СЕРВИС
{
    Item_Page, &mainPage, 0,
    {
        "СЕРВИС", "SERVICE",
        "Дополнительные настройки, калибровка, поиск сигнала, математические функции",
        "Additional settings, calibration, signal search, mathematical functions"
    },
    Page_Service,
    {
        (void*)&bResetSettings,                // СЕРВИС - Сброс настроек
        (void*)&bAutoSearch,                   // СЕРВИС - Поиск сигнала
        (void*)&pCalibrator,                   // СЕРВИС - КАЛИБРАТОР
        (void*)&pMath,                         // СЕРВИС - МАТЕМАТИКА
        (void*)&pEthernet,                     // СЕРВИС - ETHERNET
        (void*)&cSound,                        // СЕРВИС - Звук
        (void*)&cLang,                         // СЕРВИС - Язык
        (void*)&tTime,                         // СЕРВИС - Время
        (void*)&cModeLongPressButtonTrig,     // СЕРВИС - Реж длит СИНХР
        (void*)&pInformation                   // СЕРВИС - ИНФОРМАЦИЯ
    }
};

// СЕРВИС - Сброс настроек ---------------------------------------------------------------------------------------------------------------------------
static const Button bResetSettings =
{
    Item_Button, &mpService, 0,
    {
        "Сброс настроек", "Reset settings",
        "Сброс настроек на настройки по умолчанию",
        "Reset to default settings"
    },
    OnPressResetSettings
};


// СЕРВИС - Поиск сигнала ----------------------------------------------------------------------------------------------------------------------------
static const Button bAutoSearch =
{
    Item_Button, &mpService, 0,
    {
        "Поиск сигнала", "Find signal",
        "Устанавливает оптимальные установки осциллографа для сигнала в канале 1",
        "Sets optimal settings for the oscilloscope signal on channel 1"
    },
    StartAutoSearch
};

// СЕРВИС - КАЛИБРАТОР ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pCalibrator =
{
    Item_Page, &mpService, 0,
    {
        "КАЛИБРАТОР", "CALIBRATOR",
        "Управлением калибратором и калибровка осциллографа",
        "Control of the calibrator and calibration of an oscillograph"
    },
    Page_ServiceCalibrator,
    {
        (void*)&mcServiceModeCalibrator,
        (void*)&mbServiceCalibration
    }
};

// СЕРВИС - МАТЕМАТИКА ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pMath =
{
    Item_Page, &mpService, 0,
    {
        "МАТЕМАТИКА", "MATH",
        "Математические функции и БПФ",
        "Mathematical functions and FFT"
    },
    Page_Math,
    {
        (void*)&mspServiceMathFunction,
        (void*)&mspFFT
    }
};

// СЕРВИС - ETHERNET /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const Page pEthernet =
{
    Item_Page, &mpService, 0,
    {
        "ETHERNET", "ETHERNET",
        "Настройки ethernet",
        "Settings of ethernet"
    },
    Page_ServiceEthernet,
    {
        (void*)&mcServEthEnable,
        (void*)&ipAddress,
        (void*)&ipNetMask,
        (void*)&ipGateway,
        (void*)&macMAC
    }
};

// СЕРВИС - Звук -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cSound =
{
    Item_Choice, &mpService, 0,
    {
        "Звук", "Sound",
        "Включение/выключение звука",
        "Inclusion/switching off of a sound"
    },
    {
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&SOUND_ENABLED
};


// СЕРВИС - Язык -------------------------------------------------------------------------------------------------------------------------------------
static const Choice cLang =
{
    Item_Choice, &mpService, 0,
    {
        "Язык", "Language",
        "Позволяет выбрать язык меню",
        "Allows you to select the menu language"
    },
    {
        {"Русский",     "Russian"},
        {"Английский",  "English"}
    },
    (int8*)&LANG
};


// СЕРВИС - Время ------------------------------------------------------------------------------------------------------------------------------------
static int8 dServicetime = 0;
static int8 hours = 0, minutes = 0, secondes = 0, year = 0, month = 0, day = 0;
static const Time tTime =
{
    Item_Time, &mpService, 0,
    {
        "Время", "Time"
        ,
        "Установка текущего времени.\nПорядок работы:\n"
        "Нажать на элемент меню \"Время\". Откроется меню установки текущего времени. Короткими нажатиями кнопки на цифровой клавиатуре, соответсвующей "
        "элементу управления \"Время\", выделить часы, минуты, секунды, год, месяц, или число. Выделенный элемент обозначается мигающей областью. "
        "Вращением ручки УСТАНОВКА установить необходимое значение. Затем выделить пункт \"Сохранить\", нажать и удреживать более 0.5 сек кнопку на панели "
        "управления. Меню установки текущего временя закроется с сохранением нового текущего времени. Нажатие длительное удержание кнопки на любом другом элементе "
        "приведёт к закрытию меню установки текущего времени без сохранения нового текущего времени"
        ,
        "Setting the current time. \nPoryadok work:\n"
        "Click on the menu item \"Time\".The menu set the current time.By briefly pressing the button on the numeric keypad of conformity "
        "Control \"Time\", highlight the hours, minutes, seconds, year, month, or a number.The selected item is indicated by a flashing area. "
        "Turn the setting knob to set the desired value. Then highlight \"Save\", press and udrezhivat more than 0.5 seconds, the button on the panel "
        "Control. Menu Setting the current time will be closed to the conservation of the new current time. Pressing a button on the prolonged retention of any other element "
        "will lead to the closure of the current time setting menu without saving the new current time"
    },
    &dServicetime, &hours, &minutes, &secondes, &month, &day, &year
};

// СЕРВИС - Реж длит СИНХР ---------------------------------------------------------------------------------------------------------------------------
static const Choice cModeLongPressButtonTrig =
{
    Item_Choice, &mpService, 0,
    {
        "Реж длит СИНХР", "Mode long СИНХР"
        ,
        "Устанавливает действия для длительного нажатия кнопки СИНХР:\n\"Сброс уровня\" - установка уровня синхронизации в ноль,\n\"Автоуровень\" "
        "- автоматическое определение и установка уровня синхронизации"
        ,
        "Sets the action for long press CLOCK:\n\"Reset trig lvl\" - to set the trigger level to zero, \n\"Auto level\""
        "- Automatically detect and install the trigger level"
    },
    {
        {"Сброс уровня",    "Reset trig level"},
        {"Автоуровень",     "Autolevel"}
    },
    (int8*)&MODE_LONG_PRESS_TRIG
};

// СЕРВИС - ИНФОРМАЦИЯ -------------------------------------------------------------------------------------------------------------------------------
static const Page pInformation =
{
    Item_Page, &mpService, 0,
    {
        "ИНФОРМАЦИЯ", "INFORMATION",
        "Выводит на экран идентификационные данные осциллографа",
        "Displays identification data of the oscilloscope"
    },
    Page_SB_Information,
    {
        (void*)&sbExitInformation,
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)0,
        (void*)0
    },
    OnPressInformation, EmptyFuncVV, EmptyFuncVI
};


/** @}  @}
 */
