#include "Menu.h"
#include "MenuItems.h"
#include "Hardware/Sound.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
/// »спользуетс€ дл€ анимации изменени€ значени€ Choice
typedef struct
{
    Choice* choice;                 ///< јдрес Choice, который находитс€ в данный момент в движении. ≈сли 0 - все статичны.
    uint        timeStartMS;        ///< ¬рем€ начала анимации choice.
    uint        inMoveIncrease : 1;
    uint        inMoveDecrease : 1;
} TimeStructChoice;

static TimeStructChoice tsChoice;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Control::Control(const ControlStruct *str) : type(str->type), keeper(str->keeper), funcOfActive(str->funcOfActive)
{
    titleHint[0] = str->titleHint[0];
    titleHint[1] = str->titleHint[1];
    titleHint[2] = str->titleHint[2];
    titleHint[3] = str->titleHint[3];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Control::Control(TypeItem type_, const Page* keeper_, pFuncBV funcOfActive_, const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN) :
    type(type_), keeper(keeper_), funcOfActive(funcOfActive_)
{
    titleHint[0] = titleRU;
    titleHint[1] = titleEN;
    titleHint[2] = hintRU;
    titleHint[3] = hintEN;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Page::Page(const Page *keeper_, pFuncBV funcOfActive_, const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN, 
           NamePage name_, const arrayItems *items_, pFuncVV funcOnPress_, pFuncVV funcOnDraw_, pFuncVI funcRegSetSB_) :
    Control(Item_Page, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    name(name_), items(items_), funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), funcRegSetSB(funcRegSetSB_)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Button::Button(const Page *keeper_, pFuncBV funcOfActive_,
       const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN, pFuncVV funcOnPress_) :
    Control(Item_Button, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_)
{
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
SmallButton::SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
            const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN,
            pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_) :
    Control(Item_SmallButton, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    funcOnPress(funcOnPress_), funcOnDraw(funcOnDraw_), hintUGO(hintUGO_)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Governor::Governor(const Page *keeper_, pFuncBV funcOfActive_,
         const char *titleRU, const char *titleEN, const char *hintRU, const char *hintEN,
         int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_, pFuncVV funcBeforeDraw_) :
    Control(Item_Governor, keeper_, funcOfActive_, titleRU, titleEN, hintRU, hintEN),
    cell(cell_), minValue(minValue_), maxValue(maxValue_), funcOfChanged(funcOfChanged_), funcBeforeDraw(funcBeforeDraw_)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Control::IsPressed()
{
    return this == Menu::ItemUnderKey();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
IPaddress::IPaddress(const IPaddressStruct *str) :
    Control(&str->str), ip0(str->ip0), ip1(str->ip1), ip2(str->ip2), ip3(str->ip3), funcOfChanged(str->funcOfChanged), port(str->port)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Choice::NameSubItem(int i)
{
    return names[i][LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Choice::NameCurrentSubItem()
{
    return (cell == 0) ? "" : names[*cell][LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Choice::NameNextSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell + 1;
    if (index == NumSubItems())
    {
        index = 0;
    }
    return names[index][LANG];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
const char *Choice::NamePrevSubItem()
{
    if (cell == 0)
    {
        return "";
    }
    int index = *cell - 1;
    if (index < 0)
    {
        index = NumSubItems() - 1;
    }
    return names[index][set.common.lang];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int Choice::NumSubItems()
{
    int i = 0;
    for (; i < MAX_NUM_SUBITEMS_IN_CHOICE; i++)
    {
        if (names[i][set.common.lang] == 0)
        {
            return i;
        }
    }
    return i;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::StartChange(int delta)
{
    if (tsChoice.choice != 0)
    {
        return;
    }
    Sound::GovernorChangedValue();
    if (SHOW_HELP_HINTS)
    {
        SetItemForHint(this);
    }
    else if (!Menu::ItemIsActive(this))
    {
        FuncOnChanged(false);
    }
    else
    {
        tsChoice.choice = this;
        tsChoice.timeStartMS = gTimerMS;
        if (delta > 0)
        {
            tsChoice.inMoveIncrease = 1;
        }
        else if (delta < 0)
        {
            tsChoice.inMoveDecrease = 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::FuncOnChanged(bool active)
{
    if (funcOnChanged)
    {
        funcOnChanged(active);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::FuncForDraw(int x, int y)
{
    if (funcForDraw)
    {
        funcForDraw(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float Choice::Step()
{
    static const float speed = 0.1f;
    static const int numLines = 12;
    if (tsChoice.choice == this)
    {
        float delta = speed * (gTimerMS - tsChoice.timeStartMS);
        if (delta == 0.0f)
        {
            delta = 0.001f; // “аймер в несколько первых кадров может показать, что прошло 0 мс, но мы возвращаем большее число, потому что ноль будет говорить о том, что движени€ нет
        }
        if (tsChoice.inMoveIncrease == 1)
        {
            if (delta <= numLines)
            {
                return delta;
            }
            CircleIncreaseInt8(cell, 0, NumSubItems() - 1);
        }
        else if (tsChoice.inMoveDecrease == 1)
        {
            delta = -delta;

            if (delta >= -numLines)
            {
                return delta;
            }
            CircleDecreaseInt8(cell, 0, NumSubItems() - 1);
        }
        tsChoice.choice = 0;
        FuncOnChanged(Menu::ItemIsActive(this));
        Display::Redraw();
        tsChoice.inMoveDecrease = tsChoice.inMoveIncrease = 0;
        return 0.0f;
    }
    return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Choice::ChangeValue(int delta)
{
    if (delta < 0)
    {
        int8 value = (*cell == NumSubItems() - 1) ? 0 : (*cell + 1);
        *cell = value;
    }
    else
    {
        int8 value = (*cell == 0) ? (NumSubItems() - 1) : (*cell - 1);
        *cell = value;
    }
    FuncOnChanged(Menu::ItemIsActive(this));
    Sound::GovernorChangedValue();
    Display::Redraw();
}
