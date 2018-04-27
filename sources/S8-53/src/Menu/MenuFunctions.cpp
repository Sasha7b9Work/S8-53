#include "defines.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Panel/Controls.h"
#include "MenuItemsLogic.h"
#include "Menu.h"
#include "Menu/Pages/PageCursors.h"
#include "Menu/Pages/PageMeasures.h"
#include "Menu/Pages/PageMemory.h"
#include "Menu/Pages/PageHelp.h"
#include "Menu/Pages/PageService.h"
#include "Menu/Pages/PageDebug.h"
#include "Settings/SettingsTypes.h"
#include "Utils/Measures.h"
#include "Tables.h"
#include "Display/Display.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Sound.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const Page mainPage;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TypeItem Menu::TypeMenuItem(void *address) 
{
    return address ? (*((TypeItem*)address)) : Item_None;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::CurrentItemIsOpened(NamePage namePage)
{
    bool retValue = _GET_BIT(MenuPosActItem(namePage), 7) == 1;
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int8 Menu::PosCurrentItem(const Page *page)
{
    return MenuPosActItem(page->name) & 0x7f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::SetCurrentItem(const void *item, bool active)
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
            for(int8 i = 0; i < NumItemsInPage(page); i++)
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
TypeItem Menu::TypeOpenedItem()
{
    return TypeMenuItem(OpenedItem());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void* Menu::OpenedItem()
{
    TypeItem type = Item_None;
    return RetLastOpened((Page*)&mainPage, &type);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void* Menu::Item(const Page *page, int numElement)
{
    const arrayItems &array = (*page->items);
    return array[numElement + (PageIsSB(page) ? 1 : 0)];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void* Menu::CurrentItem()
{
    TypeItem type = Item_None;
    void *lastOpened = RetLastOpened((Page*)&mainPage, &type);
    int8 pos = PosCurrentItem((const Page *)lastOpened);
    if(type == Item_Page && pos != 0x7f)
    {
        return Item((const Page *)lastOpened, pos);
    }
    return lastOpened;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Menu::HeightOpenedItem(void *item) 
{
    TypeItem type = TypeMenuItem(item);
    if(type == Item_Page)
    {
        int numItems = NumItemsInPage((const Page *)item) - NumCurrentSubPage((Page *)item) * MENU_ITEMS_ON_DISPLAY;
        LIMITATION(numItems, numItems, 0, MENU_ITEMS_ON_DISPLAY);
        return MP_TITLE_HEIGHT + MI_HEIGHT * numItems;
    } 
    else if(type == Item_Choice || type == Item_ChoiceReg)
    {
        return MOI_HEIGHT_TITLE + ((Choice *)item)->NumSubItems() * MOSI_HEIGHT - 1;
    }
    return MI_HEIGHT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Menu::NumCurrentSubPage(Page *page)
{
    return MenuCurrentSubPage(page->name);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char* Menu::TitleItem(void *item) 
{
    return TITLE((Page*)item);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Menu::PosItemOnTop(Page *page)
{
    return NumCurrentSubPage(page) * MENU_ITEMS_ON_DISPLAY;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::IsFunctionalButton(PanelButton button)
{
    return button >= B_F1 && button <= B_F5;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::ChangeSubPage(const Page *page, int delta)
{
    if (page)
    {
        if (delta > 0 && MenuCurrentSubPage(page->name) < NumSubPages(page) - 1)
        {
            Sound::RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) + 1);
        }
        else if (delta < 0 && MenuCurrentSubPage(page->name) > 0)
        {
            Sound::RegulatorSwitchRotate();
            SetMenuCurrentSubPage(page->name, MenuCurrentSubPage(page->name) - 1);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Menu::NumSubPages(const Page *page)
{
    return (NumItemsInPage(page) - 1) / MENU_ITEMS_ON_DISPLAY + 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void* Menu::RetLastOpened(Page *page, TypeItem *type)
{
    if(CurrentItemIsOpened(GetNamePage(page)))
    {
        int8 posActItem = PosCurrentItem(page);
        void *item = Item(page, posActItem);
        TypeItem typeLocal = TypeMenuItem(Item(page, posActItem));
        if(typeLocal == Item_Page)
        {
            return RetLastOpened((Page *)item, type);
        }
        else
        {
            return item;
        }
    }
    *type = Item_Page;
    return page;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::CloseOpenedItem()
{
    void *item = OpenedItem();
    if(TypeOpenedItem() == Item_Page)
    {
        if (PageIsSB((const Page *)item))                               // Для страницы малых кнопок
        {
            SmallButton *sb = SmallButonFromPage((Page *)item, 0);      // Выполняем функцию нажатия кнопки B_Menu
            if (sb->funcOnPress)                                        // Если она есть
            {
                sb->funcOnPress();
            }
        }
        if(NEED_CLOSE_PAGE_SB == 1)
        {
            NamePage namePage = Keeper(item)->name;
            SetMenuPosActItem(namePage, MenuPosActItem(namePage) & 0x7f);   // Сбрасываем бит 7 - "закрываем" активный пункт страницы namePage
        }
        NEED_CLOSE_PAGE_SB = 1;
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::OpenItem(const void *item, bool open)
{
    if(item)
    {
        Page *page = Keeper(item);
        SetMenuPosActItem(GetNamePage(page), open ? (PosCurrentItem(page) | 0x80) : (PosCurrentItem(page) & 0x7f));
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::ItemIsOpened(void *item)
{
    TypeItem type = TypeMenuItem(item);
    Page *page = Keeper(item);
    if(type == Item_Page)
    {
        return CurrentItemIsOpened(GetNamePage(Keeper(item)));
    }
    return (MenuPosActItem(page->name) & 0x80) != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Page* Menu::Keeper(const void *item)
{
    const Page* page = ((Page*)(item))->keeper;
    return (Page *)page;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
NamePage Menu::GetNamePage(const Page *page)
{
    if(TypeMenuItem((void*)page) != Item_Page)
    {
        return Page_NoPage;
    }
    return page->name;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
NamePage Menu::GetNameOpenedPage()
{
    return GetNamePage((const Page *)OpenedItem());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::OpenPageAndSetItCurrent(void *page)
{
    SetCurrentItem(page, true);
    OpenItem((Page *)page, !ItemIsOpened((Page *)page));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::ItemIsActive(void *item)
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Menu::NumItemsInPage(const Page * const page) 
{
    if (page->name == Page_MainPage)
    {
        return (SHOW_DEBUG_MENU == 0) ? 10 : 11;
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::ChangeOpenedItem(void *item, int delta)
{
    if (delta < 2 && delta > -2)
    {
        return false;
    }

    TypeItem type = TypeMenuItem(item);

    if (type == Item_Page)
    {
        ChangeSubPage((const Page *)item, delta);
    }
    else if (type == Item_IP)
    {
        IPaddress_ChangeValue((IPaddress *)item, delta);
    }
    else if (type == Item_MAC)
    {
        MACaddress_ChangeValue((MACaddress *)item, delta);
    }
    else if (type == Item_ChoiceReg || type == Item_Choice)
    {
        ((Choice *)item)->ChangeValue(delta);
    }
    else if (type == Item_Governor)
    {
        Governor_ChangeValue((Governor *)item, delta);
    }
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::ChangeItem(void *item, int delta)
{
    TypeItem type = TypeMenuItem(item);
    if (type == Item_Choice || type == Item_ChoiceReg)
    {
        ((Choice *)item)->StartChange(delta);
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
        GovernorColor_ChangeValue((GovernorColor *)item, delta);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Menu::ShortPressOnPageItem(Page *page, int numItem)
{
    if (TypeMenuItem(page) != Item_Page)
    {
        return;
    }
    NamePage namePage = page->name;
    if (namePage >= Page_SB_Curs)
    {
        SmallButton *sb = (SmallButton*)(*page->items)[numItem];
        if (sb && sb->funcOnPress)
        {
            sb->funcOnPress();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Page* Menu::PagePointerFromName(NamePage namePage)
{
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Menu::PageIsSB(const Page *page)
{
    return page->name >= Page_SB_Curs;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
SmallButton* Menu::SmallButonFromPage(Page *page, int numButton)
{
    return (SmallButton *)(*page->items)[numButton];
}
