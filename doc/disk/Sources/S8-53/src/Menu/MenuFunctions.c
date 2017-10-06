#include "defines.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Panel/Controls.h"
#include "MenuFunctions.h"
#include "MenuItemsLogic.h"
#include "Menu.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Measures.h"
#include "Tables.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Sound.h"


extern const Page mainPage;


static void* RetLastOpened(Page *_page, TypeItem *_type);


TypeItem TypeMenuItem(void *address) 
{
    return address ? (*((TypeItem*)address)) : Item_None;
}

bool    CurrentItemIsOpened(NamePage namePage)
{
    bool retValue = _GET_BIT(MenuPosActItem(namePage), 7) == 1;
    return retValue;
}

int8    PosCurrentItem(const Page *page)
{
    return MenuPosActItem(page->name) & 0x7f;
}

void    SetCurrentItem(const void *item, bool active)
{
    if(item != 0)
    {
        Page *page = (Keeper(item));
        if(!active)
        {
            SetMenuPosActItem(page->name, 0x7f);
        }
        else
        {
            for(int i = 0; i < NumItemsInPage(page); i++)
            {
                if(Item(page, i) == item)
                {
                    SetMenuPosActItem(page->name, i);
                    return;
                }
            }
        }
    }
}

TypeItem    TypeOpenedItem()
{
    return TypeMenuItem(OpenedItem());
    /*
    TypeItem type = Item_None;
    RetLastOpened((Page*)&mainPage, &type);
    return type;
    */
}

void*    OpenedItem()
{
    TypeItem type = Item_None;
    return RetLastOpened((Page*)&mainPage, &type);
}

void* Item(const Page *page, int numElement)
{
    return page->items[numElement + (PageIsSB(page) ? 1 : 0)];
}

void*   CurrentItem()
{
    TypeItem type = Item_None;
    void *lastOpened = RetLastOpened((Page*)&mainPage, &type);
    int8 pos = PosCurrentItem(lastOpened);
    if(type == Item_Page && pos != 0x7f)
    {
        return Item(lastOpened, pos);
    }
    return lastOpened;
}

int HeightOpenedItem(void *item) 
{
    TypeItem type = TypeMenuItem(item);
    if(type == Item_Page)
    {
        int numItems = NumItemsInPage(item) - NumCurrentSubPage(item) * MENU_ITEMS_ON_DISPLAY;
        LIMITATION(numItems, numItems, 0, MENU_ITEMS_ON_DISPLAY);
        return MP_TITLE_HEIGHT + MI_HEIGHT * numItems;
    } 
    else if(type == Item_Choice || type == Item_ChoiceReg)
    {
        return MOI_HEIGHT_TITLE + NumSubItems(item) * MOSI_HEIGHT - 1;
    }
    return MI_HEIGHT;
}

int    NumCurrentSubPage(Page *page)
{
    return MenuCurrentSubPage(page->name);
}

const char* TitleItem(void *item) 
{
    return TITLE((Page*)item);
}

int PosItemOnTop(Page *page)
{
    return NumCurrentSubPage(page) * MENU_ITEMS_ON_DISPLAY;
}

bool IsFunctionalButton(PanelButton button)
{
    return button >= B_F1 && button <= B_F5;
}

void ChangeSubPage(const Page *page, int delta)
{
    if (page)
    {
        if (delta > 0 && MenuCurrentSubPage(page->name) < NumSubPages(page) - 1)
        {
            Sound_RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) + 1);
        }
        else if (delta < 0 && MenuCurrentSubPage(page->name) > 0)
        {
            Sound_RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) - 1);
        }
    }
}

int NumSubPages(const Page *page) {
    return (NumItemsInPage(page) - 1) / MENU_ITEMS_ON_DISPLAY + 1;
}

void* RetLastOpened(Page *page, TypeItem *type)
{
    if(CurrentItemIsOpened(GetNamePage(page)))
    {
        int8 posActItem = PosCurrentItem(page);
        void *item = Item(page, posActItem);
        TypeItem typeLocal = TypeMenuItem(Item(page, posActItem));
        if(typeLocal == Item_Page)
        {
            return RetLastOpened(item, type);
        }
        else
        {
            return item;
        }
    }
    *type = Item_Page;
    return page;
}

void CloseOpenedItem()
{
    void *item = OpenedItem();
    if(TypeOpenedItem() == Item_Page)
    {
        if (PageIsSB(item))
        {
            SmallButton *sb = SmallButonFromPage(item, 0);
            if (sb->funcOnPress)
            {
                sb->funcOnPress();
            }
        }
        NamePage name = Keeper(item)->name;
        SetMenuPosActItem(name, MenuPosActItem(name) & 0x7f);
        if(item == &mainPage)
        {
            ShowMenu(false);
        }
    }
    else
    {
        OpenItem(item, false);
    } 
}

void OpenItem(const void *item, bool open)
{
    if(item)
    {
        Page *page = Keeper(item);
        SetMenuPosActItem(GetNamePage(page), open ? (PosCurrentItem(page) | 0x80) : (PosCurrentItem(page) & 0x7f));
    }
}

bool ItemIsOpened(void *item)
{
    TypeItem type = TypeMenuItem(item);
    Page *page = Keeper(item);
    if(type == Item_Page)
    {
        return CurrentItemIsOpened(GetNamePage(Keeper(item)));
    }
    return (MenuPosActItem(page->name) & 0x80) != 0;
}

Page* Keeper(const void *item)
{
    const Page* page = ((Page*)(item))->keeper;
    return (void*)page;
}

NamePage GetNamePage(const Page *page)
{
    if(TypeMenuItem((void*)page) != Item_Page)
    {
        return Page_NoPage;
    }
    return page->name;
}

NamePage GetNameOpenedPage()
{
    return GetNamePage(OpenedItem());
}

void OpenPageAndSetItCurrent(NamePage namePage)
{

}

bool ItemIsActive(void *item)
{
    TypeItem type = TypeMenuItem(item);

    /** @todo Здесь оптимизировать через битовую маску */

    if (type == Item_Choice || type == Item_Page || type == Item_Button || type == Item_Governor || type == Item_SmallButton)
    {
        pFuncBV func = ((Page*)(item))->funcOfActive;

        return func ? func() : true;
    }

    return true;
}

int NumSubItems(Choice *choice)
{
    int i = 0;
    for(; i < MAX_NUM_SUBITEMS_IN_CHOICE; i++) 
    {
        if(choice->names[i][set.common.lang] == 0) 
        {
            return i;
        }
    }
    return i;
}

int NumItemsInPage(const Page * const page) 
{
    if (page->name == Page_MainPage)
    {
        return (gBF.showDebugMenu == 0) ? 10 : 11;
    }
    else if (PageIsSB(page))
    {
        return 5;
    }
    else
    {
        for (int i = 0; i < MAX_NUM_ITEMS_IN_PAGE; i++)
        {
            if (Item(page, i) == 0)
            {
                return i;
            }
        }
    }
    return 0;
}

const char* NameCurrentSubItem(Choice *choice) 
{
    return (choice->cell == 0) ? "" : choice->names[*choice->cell][set.common.lang];
}

const char* NameSubItem(Choice *choice, int i) 
{
    return choice->names[i][set.common.lang];
}

const char* NameNextSubItem(Choice *choice) 
{
    if(choice->cell == 0) 
    {
        return "";
    }
    int index = *choice->cell + 1;
    if (index == NumSubItems(choice))
    {
        index = 0;
    }
    return choice->names[index][set.common.lang];
}

const char* NamePrevSubItem(Choice *choice)
{
    if (choice->cell == 0)
    {
        return "";
    }
    int index = *choice->cell - 1;
    if (index < 0)
    {
        index = NumSubItems(choice) - 1;
    }
    return choice->names[index][set.common.lang];
}

bool ChangeOpenedItem(void *item, int delta)
{
    if (delta < 2 && delta > -2)
    {
        return false;
    }

    TypeItem type = TypeMenuItem(item);

    if (type == Item_Page)
    {
        ChangeSubPage(item, delta);
    }
    else if (type == Item_IP)
    {
        IPaddress_ChangeValue(item, delta);
    }
    else if (type == Item_MAC)
    {
        MACaddress_ChangeValue(item, delta);
    }
    else if (type == Item_ChoiceReg || type == Item_Choice)
    {
        Choice_ChangeValue(item, delta);
    }
    else if (type == Item_Governor)
    {
        Governor_ChangeValue(item, delta);
    }
    
    return true;
}

void ChangeItem(void *item, int delta)
{
    TypeItem type = TypeMenuItem(item);
    if (type == Item_Choice || type == Item_ChoiceReg)
    {
        Choice_StartChange(item, delta);
    }
    else if (type == Item_Governor)
    {
        Governor *governor = (Governor*)item;
        if (OpenedItem() != governor)
        {
            Governor_StartChange(governor, delta);
        }
        else
        {
            Governor_ChangeValue(governor, delta);
        }
    }
    else if (type == Item_GovernorColor)
    {
        GovernorColor_ChangeValue(item, delta);
    }
}

void ShortPressOnPageItem(Page *page, int numItem)
{
    if (TypeMenuItem(page) != Item_Page)
    {
        return;
    }
    NamePage namePage = page->name;
    if (namePage >= Page_SB_Curs)
    {
        SmallButton *sb = (SmallButton*)page->items[numItem];
        if (sb && sb->funcOnPress)
        {
            sb->funcOnPress();
        }
    }
}

Page* PagePointerFromName(NamePage namePage)
{
    return 0;
}

bool PageIsSB(const Page *page)
{
    return page->name >= Page_SB_Curs;
}

SmallButton* SmallButonFromPage(Page *page, int numButton)
{
    return page->items[numButton];
}
