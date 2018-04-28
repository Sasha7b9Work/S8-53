#include "defines.h"
#include "Settings/SettingsTypes.h"
#include "PageMemory.h"
#include "Definition.h"
#include "Menu/FileManager.h"
#include "FPGA/FPGA.h"
#include "FPGA/Storage.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/EPROM.h"
#include "Hardware/Sound.h"
#include "Log.h"
#include "Tables.h"
#include <string.h>
#include <stdio.h>


/** @addtogroup Menu
 *  @{
 *  @addtogroup PageMemory
 *  @{
 */


extern const Page pMemory;

extern Page mainPage;

//extern USBH_USR_AppStateDataStruct_TypeDef USBH_USR_AppState;

static void DrawSetMask();  // Эта функция рисует, когда выбран режим задания маски.
static void DrawSetName();  // Эта функция рисует, когда нужно задать имя файла для сохранения


void ChangeC_Memory_NumPoints(bool active)
{
    if(sMemory_GetNumPoints(false) == 281)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else
    {
        if(SET_TPOS_IS_LEFT)
        {
            SHIFT_IN_MEMORY = 0;
        }
        else if(SET_TPOS_IS_CENTER)
        {
            SHIFT_IN_MEMORY = sMemory_GetNumPoints(false) / 2 - Grid::Width() / 2;
        }
        else if(SET_TPOS_IS_RIGHT)
        {
            SHIFT_IN_MEMORY = sMemory_GetNumPoints(false) - Grid::Width() - 2;
        }
    }
    FPGA::SetTShift(TSHIFT);
}

// Активна ли ПАМЯТЬ - ВНЕШН ЗУ - Маска
bool IsActiveMemoryExtSetMask()
{                       
    return FILE_NAMING_MODE_IS_MASK;
}


void DrawSB_MemLastSelect(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 3, y + 2, set.memory.strMemoryLast.isActiveModeSelect ? '\x2a' : '\x28');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_MemLast_Prev(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x20');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_MemLast_Next(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    Painter::SetFont(TypeFont_8);
}

void PressSB_MemLastSelect()
{
    set.memory.strMemoryLast.isActiveModeSelect = !set.memory.strMemoryLast.isActiveModeSelect;
}

void PressSB_MemLast_Next()
{
    CircleIncreaseInt16(&CURRENT_NUM_LATEST_SIGNAL, 0, Storage::AllDatas() - 1);
}

void PressSB_MemLast_Prev()
{
    CircleDecreaseInt16(&CURRENT_NUM_LATEST_SIGNAL, 0, Storage::AllDatas() - 1);
}

static void RotateSB_MemLast(int angle)
{
    if (Storage::AllDatas() > 1)
    {
        Sound::RegulatorSwitchRotate();
    }
    if (Math_Sign(angle) > 0)
    {
        PressSB_MemLast_Next();
    }
    else
    {
        PressSB_MemLast_Prev();
    }
}

static void FuncDrawingAdditionSPageMemoryLast()
{
    char buffer[20];
    
    int width = 40;
    int height = 10;
    Painter::FillRegionC(Grid::Right() - width, GRID_TOP, width, height, COLOR_BACK);
    Painter::DrawRectangleC(Grid::Right() - width, GRID_TOP, width, height, COLOR_FILL);
    Painter::DrawText(Grid::Right() - width + 2, GRID_TOP + 1, Int2String(CURRENT_NUM_LATEST_SIGNAL + 1, false, 3, buffer));
    Painter::DrawText(Grid::Right() - width + 17, GRID_TOP + 1, "/");
    Painter::DrawText(Grid::Right() - width + 23, GRID_TOP + 1, Int2String(Storage::AllDatas(), false, 3, buffer));
}

void DrawSB_MemLast_IntEnter(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x40');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_MemLast_SaveToFlash(int x, int y)
{
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

static void DrawSB_MemExtSetNameSave(int x, int y)
{
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

//const PageSB pageSBmemExtSetName;

static void PressSB_MemLast_SaveToFlash()
{
    EXIT_FROM_SET_NAME_TO = RETURN_TO_LAST_MEM;
    Memory_SaveSignalToFlashDrive();
}


static void PressSB_SetName_Exit()
{
    Display::RemoveAddDrawFunction();
    if (EXIT_FROM_SET_NAME_TO_DIS_MENU)
    {
        Menu::ShortPressOnPageItem(Menu::PagePointerFromName(Page_SB_MemExtSetName), 0);
    }
    else if (EXIT_FROM_SET_NAME_TO_LAST)
    {
        Menu::OpenPageAndSetItCurrent(PageMemory::Latest::GetPointer());
    }
    else if (EXIT_FROM_SET_NAME_TO_INT)
    {
        Menu::OpenPageAndSetItCurrent(PageMemory::Internal::GetPointer());
    }
    EXIT_FROM_SET_NAME_TO = RETURN_TO_DISABLE_MENU;
}


static void PressSB_MemExtSetNameSave()
{
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        PressSB_SetName_Exit();
        NEED_SAVE_TO_DRIVE = 1;
    }
}

extern const Page mspMemLast;

const SmallButton sbMemLastPrev
(
    &mspMemLast, 0,
    "Предыдущий", "Previous",
    "Перейти к предыдущему сигналу",
    "Go to the previous signal",
    PressSB_MemLast_Prev,
    DrawSB_MemLast_Prev
);

const SmallButton sbMemLastNext
(
    &mspMemLast, 0,
    "Следующий", "Next",
    "Перейти к следующему сигналу",
    "Go to the next signal",
    PressSB_MemLast_Next,
    DrawSB_MemLast_Next
);

void PressSB_MemLast_IntEnter()
{
    Menu::OpenPageAndSetItCurrent(PageMemory::Internal::GetPointer());
    MODE_WORK = ModeWork_MemInt;
    EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
    EXIT_FROM_INT_TO_LAST = 1;
}

const SmallButton sbMemLastIntEnter
(
    &mspMemLast, 0,
    "Внутр ЗУ", "Internal storage",
    "Нажмите эту кнопку, чтобы сохранить сигнал во внутреннем запоминающем устройстве",
    "Press this button to keep a signal in an internal memory",
    PressSB_MemLast_IntEnter,
    DrawSB_MemLast_IntEnter
);

const SmallButton sbMemLastSaveToFlash
(
    &mspMemLast, 0,
    "Сохранить", "Save",
    "Кнопка становится доступна при присоединённом внешнем ЗУ. Позволяет сохранить сигнал на внешем ЗУ",
    "Click this button to save the signal on the external FLASH",
    PressSB_MemLast_SaveToFlash,
    DrawSB_MemLast_SaveToFlash
);

extern const Page mpSetName;

static const SmallButton sbSetNameSave
(
    &mpSetName, 0,
    "Сохранить", "Save",
    "Сохранение на флеш под заданным именем",
    "Saving to flashdrive with the specified name",
    PressSB_MemExtSetNameSave,
    DrawSB_MemExtSetNameSave
);


void DrawSB_SetMask_Backspace(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter::SetFont(TypeFont_8);
}

void DrawSB_SetName_Backspace(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter::SetFont(TypeFont_8);
}

void PressSB_SetMask_Backspace()
{
    int size = strlen(FILE_NAME_MASK);
    if (size > 0)
    {
        if (size > 1 && FILE_NAME_MASK[size - 2] == 0x07)
        {
            FILE_NAME_MASK[size - 2] = '\0';
        }
        else
        {
            FILE_NAME_MASK[size - 1] = '\0';
        }
    }

}

void PressSB_SetName_Backspace()
{
    int size = strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}

void DrawSB_SetMask_Delete(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(TypeFont_8);
}

void PressSB_SetMask_Delete()
{
    FILE_NAME_MASK[0] = '\0';
}

void DrawSB_SetName_Delete(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(TypeFont_8);
}

void PressSB_SetName_Delete()
{
    FILE_NAME[0] = '\0';
}

void DrawSB_SetMask_Insert(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Painter::SetFont(TypeFont_8);
}

void PressSB_SetMask_Insert()
{
    int index = INDEX_SYMBOL;
    int size = strlen(FILE_NAME_MASK);
    if (size == MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        return;
    }
    if (index < 0x41)
    {
        FILE_NAME_MASK[size] = symbolsAlphaBet[index][0];
        FILE_NAME_MASK[size + 1] = '\0';
    }
    else
    {
        index -= 0x40;
        if (index == 0x07)  // Для %nN - отдельный случай
        {
            if (size < MAX_SYMBOLS_IN_FILE_NAME - 2 && size > 0)
            {
                if (FILE_NAME_MASK[size - 1] >= 0x30 && FILE_NAME_MASK[size - 1] <= 0x39) // Если ранее введено число
                {
                    FILE_NAME_MASK[size] = FILE_NAME_MASK[size - 1] - 0x30;
                    FILE_NAME_MASK[size - 1] = 0x07;
                    FILE_NAME_MASK[size + 1] = '\0'; 
                }
            }
        }
        else
        {
            FILE_NAME_MASK[size] = (int8)index;
            FILE_NAME_MASK[size + 1] = '\0';
        }
    }
}

void DrawSB_SetName_Insert(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x26');
    Painter::SetFont(TypeFont_8);
}

void PressSB_SetName_Insert()
{
    int size = strlen(FILE_NAME);
    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = symbolsAlphaBet[INDEX_SYMBOL][0];
        FILE_NAME[size + 1] = '\0';
    }
}

void OnMemExtSetMaskNameRegSet(int angle, int maxIndex)
{
    int8(*func[3])(int8 *, int8, int8) =
    {
        CircleDecreaseInt8,
        CircleDecreaseInt8,
        CircleIncreaseInt8
    };

    Painter::ResetFlash();
    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = maxIndex - 1;
    }
    func[Math_Sign(angle) + 1](&INDEX_SYMBOL, 0, maxIndex - 1);
    Sound::RegulatorSwitchRotate();

}

static void OnMemExtSetMaskRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4);
}

extern const Page mspSetMask;

const SmallButton sbSetMaskBackspace
(
    &mspSetMask, 0,
    "Backspace", "Backspace",
    "Удаляет последний введённый символ",
    "Deletes the last entered symbol",
    PressSB_SetMask_Backspace,
    DrawSB_SetMask_Backspace
);

const SmallButton sbSetMaskDelete
(
    &mspSetMask, 0,
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered symbols",
    PressSB_SetMask_Delete,
    DrawSB_SetMask_Delete
);

const SmallButton sbSetMaskInsert
(
    &mspSetMask, 0,
    "Вставить", "Insert",
    "Вставляет выбранный символ",
    "Inserts the chosen symbol",
    PressSB_SetMask_Insert,
    DrawSB_SetMask_Insert
);



const SmallButton sbSetNameBackspace
(
    &mpSetName, 0,
    "Backspace", "Backspace",
    "Удаляет последний символ",
    "Delete the last character",
    PressSB_SetName_Backspace,
    DrawSB_SetName_Backspace
);

const SmallButton sbSetNameDelete
(
    &mpSetName, 0,
    "Удалить", "Delete",
    "Удаляет все введённые символы",
    "Deletes all entered characters",
    PressSB_SetName_Delete,
    DrawSB_SetName_Delete
);

const SmallButton sbSetNameInsert
(
    &mpSetName, 0,
    "Вставить", "Insert",
    "Вводит очередной символ",
    "Print the next character",
    PressSB_SetName_Insert,
    DrawSB_SetName_Insert
);

void DrawSB_MemExtNewFolder(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 1, y, '\x46');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_FM_LevelDown(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Painter::SetFont(TypeFont_8);
}

void DrawSB_FM_LevelUp(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_FM_Tab(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x6e');
    Painter::SetFont(TypeFont_8);
}

extern const Page mspFileManager;

const SmallButton sbFileManagerTab
(
    &mspFileManager, 0,
    "Tab", "Tab",
    "Переход между каталогами и файлами",
    "The transition between the directories and files",
    FM::PressTab,
    DrawSB_FM_Tab
);

const SmallButton sbFileManagerLevelDown
(
    &mspFileManager, 0,
    "Войти", "Enter",
    "Переход в выбранный каталог",
    "Transition to the chosen catalog",
    FM::PressLevelDown,
    DrawSB_FM_LevelDown
);

const SmallButton sbFileManagerLevelUp
(
    &mspFileManager, 0,
    "Выйти", "Leave",
    "Переход в родительский каталог",
    "Transition to the parental catalog",
    FM::PressLevelUp,
    DrawSB_FM_LevelUp
);

void DrawSB_MemInt_SaveToIntMemory(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(TypeFont_8);
}


static void SaveSignalToIntMemory(void)
{
    if (EXIT_FROM_INT_TO_LAST)          // Если перешли во ВНУТР ЗУ из ПОСЛЕДНИЕ
    {
        if  (gDSmemLast != 0)
        {                               // то сохраняем сигнал из последних
            EPROM::SaveData(CURRENT_NUM_INT_SIGNAL, gDSmemLast, gData0memLast, gData1memLast);
            EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
            Display::ShowWarningGood(SignalIsSaved);
        }
    }
    else                                // Иначе сохраняем текущий сигнал
    {
        if (gDSet != 0)
        {
            EPROM::SaveData(CURRENT_NUM_INT_SIGNAL, gDSet, gData0, gData1);
            EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSet, &gData0memInt, &gData1memInt);
            Display::ShowWarningGood(SignalIsSaved);
        }
    }
}


void PressSB_MemInt_SaveToIntMemory()
{
    SaveSignalToIntMemory();
}

void DrawSB_MemInt_SaveToFlashDrive(int x, int y)
{
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        Painter::SetFont(TypeFont_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x42');
        Painter::SetFont(TypeFont_8);
    }
}

static void DrawMemoryWave(int num, bool exist)
{
    char buffer[20];
    
    int x = Grid::Left() + 2 + num * 12;
    int y = Grid::FullBottom() - 10;
    int width = 12;
    Painter::FillRegionC(x, y, width, 10, num == CURRENT_NUM_INT_SIGNAL ? COLOR_FLASH_10 : COLOR_BACK);
    Painter::DrawRectangleC(x, y, width, 10, COLOR_FILL);
    Painter::SetColor(num == CURRENT_NUM_INT_SIGNAL ? COLOR_FLASH_01 : COLOR_FILL);
    if (exist)
    {
        Painter::DrawText(x + 2, y + 1, Int2String(num + 1, false, 2, buffer));
    }
    else
    {
        Painter::DrawText(x + 3, y + 1, "\x88");
    }
}

static void FuncAdditionDrawingSPageMemoryInt()
{
    // Теперь нарисуем состояние памяти

    bool exist[MAX_NUM_SAVED_WAVES] = {false};

    EPROM::GetDataInfo(exist);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        DrawMemoryWave(i, exist[i]);
    }
}

void PressSB_MemInt_SaveToFlashDrive()
{
    EXIT_FROM_SET_NAME_TO = RETURN_TO_INT_MEM;
    Memory_SaveSignalToFlashDrive();
}

static void FuncOnRegSetMemInt(int delta)
{
    Sound::RegulatorSwitchRotate();
    if (delta < 0)
    {
        CircleDecreaseInt8(&CURRENT_NUM_INT_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (delta > 0)
    {
        CircleIncreaseInt8(&CURRENT_NUM_INT_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
    Painter::ResetFlash();
}

static void DrawSB_MemInt_ShowSignalAllways_Yes(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ShowSignalAllways_No(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ShowSignalAlways(int x, int y)
{
    if (INT_SHOW_ALWAYS == 0)
    {
        DrawSB_MemInt_ShowSignalAllways_No(x, y);
    }
    else
    {
        DrawSB_MemInt_ShowSignalAllways_Yes(x, y);
    }
}

static void PressSB_MemInt_ShowSignalAlways()
{
    INT_SHOW_ALWAYS = (INT_SHOW_ALWAYS == 0) ? 1U : 0U;
}

extern const Page mspMemInt;

static const arrayHints hintsMemIntShowSignalAlways =
{
    {DrawSB_MemInt_ShowSignalAllways_Yes, "показывать выбранный сигнал из внутренней памяти поверх текущего",
                                          "to show the chosen signal from internal memory over the current"},
    {DrawSB_MemInt_ShowSignalAllways_No,  "сигнал из внутренней памяти виден только в режиме работы с внутренним запоминающим устройством",
                                          "the signal from internal memory is visible only in an operating mode with an internal memory"}
};

static const SmallButton sbMemIntShowSignalAlways
(
    &mspMemInt, 0,
    "Показывать всегда", "To show always",
    "Позволяет всегда показывать выбранный сохранённый сигнал поверх текущего",
    "Allows to show always the chosen kept signal over the current",
    PressSB_MemInt_ShowSignalAlways,
    DrawSB_MemInt_ShowSignalAlways,
    &hintsMemIntShowSignalAlways
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawSB_MemInt_ModeShow_Direct(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_DIRECT);
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ModeShow_Saved(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_MEM_INT_SHOW_SAVED);
    Painter::SetFont(TypeFont_8);
}

static void DrawSB_MemInt_ModeShow_Both(int x, int y)
{
    ++y;

    Painter::DrawRectangle(x + 2, y + 5, 4, 7);

    Painter::DrawRectangle(x + 8, y + 8, 4, 4);
    Painter::DrawVLine(x + 8, y + 8, y + 5);
    Painter::DrawHLine(y + 5, x + 8, x + 12);

    Painter::DrawVLine(x + 14, y + 6, y + 12);
    Painter::DrawVLine(x + 17, y + 5, y + 12);
    Painter::DrawHLine(y + 9, x + 14, x + 17);
    Painter::DrawHLine(y + 5, x + 15, x + 17);
}

static void DrawSB_MemInt_ModeShow(int x, int y)
{
    ModeShowIntMem mode = MODE_SHOW_MEMINT;

    if (mode == ModeShowIntMem_Direct)
    {
        DrawSB_MemInt_ModeShow_Direct(x, y);
    }
    else if (mode == ModeShowIntMem_Saved)
    {
        DrawSB_MemInt_ModeShow_Saved(x, y);
    }
    else if (mode == ModeShowIntMem_Both)
    {
        DrawSB_MemInt_ModeShow_Both(x, y);
    }
}

static void PressSB_MemInt_ModeShow()
{
    CircleIncreaseInt8((int8*)&MODE_SHOW_MEMINT, 0, 2);
}

static const arrayHints hintsMemIntModeShow =
{
    { DrawSB_MemInt_ModeShow_Direct, "на дисплее текущий сигнал",     "on the display current signal" },
    { DrawSB_MemInt_ModeShow_Saved,  "на дисплее сохранённый сигнал", "on the display the kept signal" },
    { DrawSB_MemInt_ModeShow_Both,   "на дисплее оба сигнала",        "on the display the both signals" }
};

static const SmallButton sbMemIntModeShow
(
    &mspMemInt, 0,
    "Вид сигнала", "Type of a signal",
    "Показывать записанный или текущий сигнал в режиме ВНУТР ЗУ",
    "Show recorded or current signal in mode Internal Memory",
    PressSB_MemInt_ModeShow,
    DrawSB_MemInt_ModeShow,
    &hintsMemIntModeShow
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void PressSB_MemInt_Delete()
{
    EPROM::DeleteData(CURRENT_NUM_INT_SIGNAL);
}

static void DrawSB_MemInt_Delete(int x, int y)
{
    Painter::SetFont(TypeFont_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(TypeFont_8);
}

static const SmallButton sbMemIntDelete
(
    &mspMemInt, 0,
    "Удалить сигнал", "Delete signal",
    "Удалить сигнал",
    "Delete signal",
    PressSB_MemInt_Delete,
    DrawSB_MemInt_Delete
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
const SmallButton sbMemIntSave
(
    &mspMemInt, 0,
    "Сохранить", "Save",
    "Сохранить сигнал во внутреннем запоминующем устройстве",
    "To keep a signal in an internal memory",
    PressSB_MemInt_SaveToIntMemory,
    DrawSB_MemInt_SaveToIntMemory
);

const SmallButton sbMemIntSaveToFlash
(
    &mspMemInt, 0,
    "Сохранить", "Save",
    "Сохраняет сигнал на флешку",
    "Save signal to flash drive",
    PressSB_MemInt_SaveToFlashDrive,
    DrawSB_MemInt_SaveToFlashDrive
);

void PressSB_MemInt_Exit()
{
    EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
    if (EXIT_FROM_INT_TO_LAST)
    {
        Menu::OpenPageAndSetItCurrent(PageMemory::Latest::GetPointer());
        MODE_WORK = ModeWork_Latest;
        EXIT_FROM_INT_TO_LAST = 0;
        NEED_CLOSE_PAGE_SB = 0;
    }
    else
    {
        MODE_WORK = ModeWork_Direct;
        Menu::ShortPressOnPageItem(Menu::PagePointerFromName(Page_SB_MemInt), 0);
    }
}

extern const Page mpSetName;

static const SmallButton sbExitSetName     // Кнопк для выхода из режима задания имени сохраняемому сигналу. Одновременно кнопка отказа от сохранения
(
    &mpSetName, 0,
    EXIT_RU, EXIT_EN,
    "Отказ от сохранения",
    "Failure to save",
    PressSB_SetName_Exit,
    DrawSB_Exit
);

extern const Page mspMemInt;

static const SmallButton sbExitMemInt    // Кнопка для выхода из режима малых кнопок.
(
    &mspMemInt,
    COMMON_BEGIN_SB_EXIT,
    PressSB_MemInt_Exit,
    DrawSB_Exit
);


// Нажатие ПАМЯТЬ - ВНЕШН ЗУ - Маска
void OnPressMemoryExtMask(void)
{
    Menu::OpenPageAndSetItCurrent(PageMemory::SetMask::GetPointer());
    Display::SetAddDrawFunction(DrawSetMask);
}

// ПАМЯТЬ
extern const Page pMemory;

static bool FuncActiveMemoryNumPoinst(void)
{
    return PEAKDET_IS_DISABLE;
}

// ПАМЯТЬ - Точки
const Choice mcMemoryNumPoints =
{
    Item_Choice, &pMemory, FuncActiveMemoryNumPoinst,
    {
        "Точки", "Points"
        ,
        "Выбор количества отсчётов для сохраняемых сигналов. "
        "При увеличении количества отсчётов уменьшается количество сохранённых в памяти сигналов."
        ,
        "Choice of number of counting for the saved signals. "
        "At increase in number of counting the quantity of the signals kept in memory decreases."
    },
    {   
        {"281",     "281"},
        {"512",     "512"},
        {"1024",    "1024"}
    },
    (int8*)&ENUM_POINTS, ChangeC_Memory_NumPoints
};

// ПАМЯТЬ - ВНЕШН ЗУ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mspMemoryExt;

// ПАМЯТЬ - ВНЕШН ЗУ - Имя файла
const Choice mcMemoryExtName =
{
    Item_Choice, &mspMemoryExt, 0,
    {
        "Имя файла", "File name"
        ,
        "Задаёт режим наименования файлов при сохранении на внешний накопитель:\n"
        "\"По маске\" - файлы именуются автоматически по заранее введённой маске (след. пункт меню),\n"
        "\"Вручную\" - каждый раз имя файла нужно задавать вручную"
        ,
        "Sets the mode name when saving files to an external drive:\n"
        "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
        "\"Manual\" - each time the file name must be specified manually"
    },
    {   
        {"По маске",    "Mask"},
        {"Вручную",     "Manually"}
    },
    (int8*)&FILE_NAMING_MODE
};
    
// ПАМЯТЬ - ВНЕШН ЗУ - Автоподключение
const Choice mcMemoryExtAutoConnectFlash =
{
    Item_Choice, &mspMemoryExt, 0,
    {
        "Автоподкл.", "AutoConnect",
        "Eсли \"Вкл\", при подключении внешнего накопителя происходит автоматический переход на страницу ПАМЯТЬ - Внешн ЗУ",
        "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.Storage"
    },
    {   
        {DISABLE_RU,    DISABLE_EN},
        {ENABLE_RU,     ENABLE_EN}
    },
    (int8*)&FLASH_AUTOCONNECT
};

// ПАМЯТЬ - ВНЕШН ЗУ - Реж кн ПАМЯТЬ
const Choice mcMemoryExtModeBtnMemory =
{
    Item_Choice, &mspMemoryExt, 0,
    {
        "Реж кн ПАМЯТЬ", "Mode btn MEMORY",
        "",
        ""
    },
    {
        {"Меню",        "Menu"},
        {"Сохранение",  "Save"}
    },
    (int8*)&MODE_BTN_MEMORY
};

// ПАМЯТЬ - ВНЕШН ЗУ - Сохранять как
const Choice mcMemoryExtModeSave =
{
    Item_Choice, &mspMemoryExt, 0,
    {
        "Сохранять как", "Save as"
        ,
        "Если выбран вариант \"Изображение\", сигнал будет сохранён в текущем каталоге в графическом файле с расширением BMP\n"
        "Если выбран вариант \"Текст\", сигнал будет сохранён в текущем каталоге в текстовом виде в файле с раширением TXT"
        ,
        "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
        "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    },
    {
        {"Изображение", "Image"},
        {"Текст",       "Text"}
    },
    (int8*)&MODE_SAVE_SIGNAL
};
    
void DrawSetName()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 60;
    int width = Grid::Width() - 80;
    int height = 80;

    Painter::DrawRectangleC(x0, y0, width, height, COLOR_FILL);
    Painter::FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, COLOR_BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while (symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // Теперь рисуем цифры и пробел
    position = 0;
    while (symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while (symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    int x = Painter::DrawTextC(x0 + deltaX, y0 + 65, FILE_NAME, COLOR_FILL);
    Painter::FillRegionC(x, y0 + 65, 5, 8, COLOR_FLASH_10);
}

static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    Painter::SetColor(COLOR_FILL);
    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = Painter::DrawChar(x, y, *ch);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = Painter::DrawChar(x, y, '%');
                x = Painter::DrawChar(x, y, (char)(0x30 | *(ch + 1)));
                x = Painter::DrawChar(x, y, 'N');
                ch++;
            }
            else
            {
                x = Painter::DrawText(x, y, symbolsAlphaBet[*ch + 0x40]);
            }
        }
        ch++;
    }
    Painter::FillRegionC(x, y, 5, 8, COLOR_FLASH_10);
}

void DrawSetMask()
{
    int x0 = Grid::Left() + 40;
    int y0 = GRID_TOP + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangleC(x0, y0, width, height, COLOR_FILL);
    Painter::FillRegionC(x0 + 1, y0 + 1, width - 2, height - 2, COLOR_BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // Рисуем большие буквы английского алфавита
    while(symbolsAlphaBet[index][0] != ' ')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }
    
    // Теперь рисуем цифры и пробел
    position = 0;
    while(symbolsAlphaBet[index][0] != 'a')
    {
        DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // Теперь рисуем малые буквы алфавита
    position = 0;
    while(symbolsAlphaBet[index][0] != '%')
    {
        DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    // Теперь рисуем спецсимволы
    position = 0;
    while (index < (sizeof(symbolsAlphaBet) / 4))
    {
        DrawStr(index, x0 + deltaX + 26 + position * 20, y0 + deltaY0 + deltaY * 3);
        index++;
        position++;
    }

    DrawFileMask(x0 + deltaX, y0 + 65);

    static const char* strings[] =
    {
        "%y - год, %m - месяц, %d - день",
        "%H - часы, %M - минуты, %S - секунды",
        "%nN - порядковый номер, где",
        "n - минимальное количество знаков для N"
    };

    deltaY--;
    Painter::SetColor(COLOR_FILL);
    for(int i = 0; i < sizeof(strings) / 4; i++)
    {
        Painter::DrawText(x0 + deltaX, y0 + 100 + deltaY * i, strings[i]);
    }
}

void Memory_SaveSignalToFlashDrive()
{
    if (FLASH_DRIVE_IS_CONNECTED)
    {
        if (FILE_NAMING_MODE_IS_HAND)
        {
            Menu::OpenPageAndSetItCurrent(PageMemory::SetName::GetPointer());
            Display::SetAddDrawFunction(DrawSetName);
        }
        else
        {
            NEED_SAVE_TO_DRIVE = 1;
        }
    }
    else
    {
        EXIT_FROM_SET_NAME_TO = RETURN_TO_MAIN_MENU;
    }
}

static void PressSB_MemLast_Exit()
{
    MODE_WORK = ModeWork_Direct;
    if (RUN_FPGA_AFTER_SMALL_BUTTONS)
    {
        FPGA::Start();
        RUN_FPGA_AFTER_SMALL_BUTTONS = 0;
    }
    Display::RemoveAddDrawFunction();
}

// Нажатие ПАМЯТЬ - Последние.
void OnPressMemoryLatest()
{
    CURRENT_NUM_LATEST_SIGNAL = 0;
    RUN_FPGA_AFTER_SMALL_BUTTONS = FPGA::IsRunning() ? 1U : 0U;
    FPGA::Stop(false);
    MODE_WORK = ModeWork_Latest;
}

static const SmallButton sbExitMemLast
(
    &mspMemLast,
    COMMON_BEGIN_SB_EXIT,
    PressSB_MemLast_Exit,
    DrawSB_Exit
);

// Память - Последние /////////////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsMemLast =
{
    (void*)&sbExitMemLast,
    (void*)0,
    (void*)&sbMemLastNext,
    (void*)&sbMemLastPrev,
    (void*)&sbMemLastIntEnter,
    (void*)&sbMemLastSaveToFlash
};

static const Page mspMemLast
(
    &pMemory, 0,
    "ПОСЛЕДНИЕ", "LATEST",
    "Переход в режим работы с последними полученными сигналами",
    "Transition to an operating mode with the last received signals",
    Page_SB_MemLatest, &itemsMemLast, OnPressMemoryLatest, FuncDrawingAdditionSPageMemoryLast, RotateSB_MemLast
);

static void PressSB_SetMask_Exit()
{
    Display::RemoveAddDrawFunction();
}

static const SmallButton sbExitSetMask
(
    &mspSetMask,
    COMMON_BEGIN_SB_EXIT,
    PressSB_SetMask_Exit,
    DrawSB_Exit
);

// Память - ВНЕШН ЗУ - Маска ////////////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsSetMask =
{
    (void*)&sbExitSetMask,
    (void*)&sbSetMaskDelete,
    (void*)0,
    (void*)0,
    (void*)&sbSetMaskBackspace,
    (void*)&sbSetMaskInsert
};

static const Page mspSetMask
(
    &mspMemoryExt, IsActiveMemoryExtSetMask,
    "МАСКА", "MASK",
    "Режим ввода маски для автоматического именования файлов",
    "Input mode mask for automatic file naming",
    Page_SB_MemExtSetMask, &itemsSetMask, EmptyFuncVV, OnPressMemoryExtMask, OnMemExtSetMaskRegSet
);

// ПАМЯТЬ - ВНЕШН ЗУ - Каталог ///////////////////////////////////////////////////////////////////////////////
void OnPressMemoryExtFileManager()
{
    if(FLASH_DRIVE_IS_CONNECTED)
    {
        Menu::OpenPageAndSetItCurrent(PageMemory::FileManager::GetPointer());
        Display::SetDrawMode(DrawMode_Hand, FM::Draw);
        NEED_REDRAW_FILEMANAGER = 1;
    }
}

bool FuncOfActiveExtMemFolder()
{
    return FLASH_DRIVE_IS_CONNECTED == 1;
}

static void PressSB_FM_Exit()
{
    Display::SetDrawMode(DrawMode_Auto, 0);
    Display::RemoveAddDrawFunction();
}

const SmallButton sbExitFileManager
(
    &mspFileManager, 0,
    EXIT_RU, EXIT_EN,
    EXIT_ILLUSTRATION_RU,
    EXIT_ILLUSTRATION_EN,
    PressSB_FM_Exit,
    DrawSB_Exit
);

static const arrayItems itemsFileManager =
{
    (void*)&sbExitFileManager,
    (void*)&sbFileManagerTab,
    (void*)0,
    (void*)0,
    (void*)&sbFileManagerLevelUp,
    (void*)&sbFileManagerLevelDown
};

const void *pMspFileManager = (const void *)&mspFileManager;

static const Page mspFileManager
(
    &mspMemoryExt, FuncOfActiveExtMemFolder,
    "КАТАЛОГ", "DIRECTORY",
    "Открывает доступ к файловой системе подключенного накопителя",
    "Provides access to the file system of the connected drive",
    Page_SB_FileManager, &itemsFileManager, OnPressMemoryExtFileManager, EmptyFuncVV, FM::RotateRegSet
);

// ПАМЯТЬ - ВНЕШН ЗУ /////////////////////////////////////////////////////////////////
static const arrayItems itemsMemoryExt =
{
    (void*)&mspFileManager,
    (void*)&mcMemoryExtName,
    (void*)&mspSetMask,
    (void*)&mcMemoryExtModeSave,
    (void*)&mcMemoryExtModeBtnMemory,
    (void*)&mcMemoryExtAutoConnectFlash
};

static const Page mspMemoryExt
(
    &pMemory, 0,
    "ВНЕШН ЗУ", "EXT STORAGE",
    "Работа с внешним запоминающим устройством.",
    "Work with external storage device.",
    Page_MemoryExt, &itemsMemoryExt
);

// ПАМЯТЬ - Внутр ЗУ ///////////////////////////////////////////////////////////////////////////////////////

// Нажатие ПАМЯТЬ - Внутр ЗУ
void OnPressMemoryInt()
{
    Menu::OpenPageAndSetItCurrent(PageMemory::Internal::GetPointer());
    MODE_WORK = ModeWork_MemInt;
    EPROM::GetData(CURRENT_NUM_INT_SIGNAL, &gDSmemInt, &gData0memInt, &gData1memInt);
}

static const arrayItems itemsMemInt =
{
    (void*)&sbExitMemInt,
    (void*)&sbMemIntShowSignalAlways,
    (void*)&sbMemIntModeShow,
    (void*)&sbMemIntDelete,
    (void*)&sbMemIntSave,
    (void*)&sbMemIntSaveToFlash
};

static const Page mspMemInt
(
    &pMemory, 0,
    "ВНУТР ЗУ", "INT STORAGE",
    "Переход в режим работы с внутренней памятью",
    "Transition to an operating mode with internal memory",
    Page_SB_MemInt, &itemsMemInt, OnPressMemoryInt, FuncAdditionDrawingSPageMemoryInt, FuncOnRegSetMemInt
);

// Страница вызывается при выбранном ручном режиме задания имени файла перед сохранением на флешку ///////////////
static void OnMemExtSetNameRegSet(int angle)
{
    OnMemExtSetMaskNameRegSet(angle, sizeof(symbolsAlphaBet) / 4 - 7);
}

static const arrayItems itemsSetName =
{
    (void*)&sbExitSetName,
    (void*)&sbSetNameDelete,
    (void*)0,
    (void*)&sbSetNameBackspace,
    (void*)&sbSetNameInsert,
    (void*)&sbSetNameSave
};

static const Page mpSetName
(
    0, 0,
    "", "",
    "", 
    "",
    Page_SB_MemExtSetName, &itemsSetName, EmptyFuncVV, EmptyFuncVV, OnMemExtSetNameRegSet
);

// ПАМЯТЬ /////////////////////////////////////////////////////////////////////////////
static const arrayItems itemsMemory =
{
    (void*)&mcMemoryNumPoints,
    (void*)&mspMemLast,
    (void*)&mspMemInt,
    (void*)&mspMemoryExt
};

static const Page pMemory              ///< ПАМЯТЬ
(
    &mainPage, 0,
    "ПАМЯТЬ", "MEMORY",
    "Работа с внешней и внутренней памятью.",
    "Working with external and internal memory.",
    Page_Memory, &itemsMemory
);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::GetPointer()
{
    return (void *)&pMemory;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::Latest::GetPointer()
{
    return (void *)&mspMemLast;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::Internal::GetPointer()
{
    return (void *)&mspMemInt;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::SetMask::GetPointer()
{
    return (void *)&mspSetMask;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::SetName::GetPointer()
{
    return (void *)&mpSetName;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void *PageMemory::FileManager::GetPointer()
{
    return (void *)&mspFileManager;
}

/** @}  @}
 */
