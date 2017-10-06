#pragma once
#include "Panel/Controls.h"
#include "MenuItems.h"


/** @defgroup Menu
 *  @{
 */


class Menu
{
public:
    /// Возвращает адрес элемента меню, соответствующего данной button.
    void* ItemUnderButton(PanelButton button);
    /// Возвращает true, если элемент меню item затенён (находится не на самом верхнем слое. Как правило, это означает, что раскрыт раскрывающийся элемент меню вроде Choice или Governor.
    bool IsShade(void* item);
    bool IsPressed(void* item);
    void Draw();
    /// Функция должна вызываться в главном цикле.
    void UpdateInput();
    /// Функция обработки короткого нажатия кнопки (менее 0.5 сек.).
    void ShortPressureButton(PanelButton button);
    /// Функция обработки длинного нажатия кнопки (более 0.5 сек.).
    void LongPressureButton(PanelButton button);
    /// Функция вызывается, когда кнопка переходит из отжатого в нажатое положение.
    void PressButton(PanelButton button);
    /// Функция вызывается, когда кнопка переходит из нажатого в отжатое положение.
    void ReleaseButton(PanelButton button);
    /// Функция обработки поворота ручки УСТАНОВКА вправо.
    void RotateRegSetRight();
    /// Функция обработки поворота ручки УСТАНОВКА влево.
    void RotateRegSetLeft();
    /// Установить время автоматического сокрытия меню в соответствии с установками.
    void SetAutoHide(bool active);
    /// Возвращает адрес пункта меню, находящегося под нажатой в данный момент кнопкой.
    void* ItemUnderKey();
    /// Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
    char* StringNavigation(char buffer[100]);

    void OpenItemTime();
    /// Вовзращает true, если элемент меню item является ативным, т.е. может быть нажат.
    bool ItemIsActive(void *item);
    /// Возвращает количество элементов в странице по адресу page.
    int NumItemsInPage(const Page * const page);
    /// Возвращает номер текущей подстранцы элемента по адресу page.
    int NumCurrentSubPage(Page *page);
    /// Dозвращает число подстраниц в странице по адресу page.
    int NumSubPages(const Page *page);
    /// Возвращает тип элемента меню по адресу address.
    TypeItem TypeMenuItem(void *address);
    /// Возвращает тип открытого элемента меню.
    TypeItem TypeOpenedItem();
    /// Возвращает адрес открытого элемента меню.
    void* OpenedItem();
    /// Возвращает адрес текущего элемента меню (текущим, как правило, является элемент, кнопка которого была нажата последней.
    void* CurrentItem();
    /// Возвращает адрес элемента меню заданной страницы.
    void* Item
                (const Page *page,              ///< страница, элемент которой нужно узнать
                int numElement                  ///< порядковый номер элемента, который нужно узнать
                );
    /// Возвращает true, если текущий элемент страницы с именем namePage открыт.
    bool CurrentItemIsOpened(NamePage namePage);
    /// Возвращает позицию текущего элемента странцы page.
    int8 PosCurrentItem(const Page *page);
    /// Сделать/разделать текущим пункт страницы.
    void SetCurrentItem
                        (const void *item,      ///< адрес элемента, активность которого нужно установить/снять
                        bool active             ///< true - сделать элемент текущим, false - расделать элемент текущим. В этом случае текущим становится заголовок страницы
                        );
    /// Возвращает адрес элемента, которому принадлежит элемент по адресу item.
    Page* Keeper(const void *item);
    /// Возвращает имя страницы page.
    NamePage GetNamePage(const Page *page);

    NamePage GetNameOpenedPage();

    void OpenPageAndSetItCurrent(NamePage namePage);

    bool ChangeOpenedItem(void *item, int delta);
    /// Уменьшает или увеличивает значение Governor, GovernorColor или Choice по адресу item в зависимости от знака delta
    void ChangeItem(void *item, int delta);
    /// Возвращает высоту в пикселях открытого элемента Choice или NamePage по адресу item.
    int HeightOpenedItem(void *item);
    /// Возвращает название элемента по адресу item, как оно выглядит на дисплее прибора.
    const char* TitleItem(void *item);
    /// Возвращает позицию первого элемента страницы по адресу page на экране. Если текущая подстраница 0, это будет 0, если текущая подстраница 1, это будет 5 и т.д.
    int PosItemOnTop(Page *page);
    /// Возвращает true, если button - функциональная клавиша [1..5].
    bool IsFunctionalButton(PanelButton button);
    /// Закрыть открытый элемент меню.
    void CloseOpenedItem();

    void OpenItem(const void *item, bool open);          // Открыть/закрыть элемент меню по адрему item.

    bool ItemIsOpened(void *item);                       // Возвращает true, если элемент меню по адрему item открыт.

    void ChangeSubPage(const Page *page, int delta);

    //void PrintStatePage(Page *page);

    void ShortPressOnPageItem(Page *page, int numItem);   // Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page;

    Page* PagePointerFromName(NamePage namePage);

    bool PageIsSB(const Page *page);

    SmallButton* SmallButonFromPage(Page *page, int numButton);

    void ChangeStateFlashDrive();

private:
    /// Обработка короткого нажатия кнопки.
    void ProcessingShortPressureButton();
    /// Обработка длинного нажатия кнопки.
    void ProcessingLongPressureButton();
    /// Обработка опускания кнопки вниз.
    void ProcessingPressButton();
    /// Обработка поднятия кнопки вверх.
    void ProcessingReleaseButton();
    /// Обработка поворота ручки УСТАНОВКА.
    void ProcessingRegulatorSet();
    /// Включить/выключить светодиод ручки УСТАНОВКА, если необходимо.
    void SwitchSetLED();
    /// Обработка короткого нажатия на элемент NamePage с адресом page.
    void ShortPress_Page(void *page);
    /// Обработка короткого нажатия на элемент Choice с адресом choice.
    void ShortPress_Choice(void *choice);

    void ShortPress_Time(void *time);
    /// Обработка короткого нажатия на элемент Button с адресом button.
    void ShortPress_Button(void *button);
    /// Обработка короткого нажатия на элемент Governor с адресом governor.
    void ShortPress_Governor(void *governor);
    /// Обработка короткого нажатия на элемент GovernorColor с адресом governorColor.
    void ShortPress_GovernorColor(void *governorColor);

    void ShortPress_IP(void *item);

    void ShortPress_MAC(void *item);

    void ShortPress_ChoiceReg(void *choice);

    void ShortPress_SmallButton(void *smallButton);
    /// Обработка длинного нажатия на элемент меню item.
    void FuncOnLongPressItem(void *item);

    void FuncOnLongPressItemTime(void *item);
    /// Обработка длинного нажатия на элемент Button с адресом button.
    void FuncOnLongPressItemButton(void *button);
    /// Возвращает функцию обработки короткого нажатия на элемент меню item.
    void ExecuteFuncForShortPressOnItem(void *item);
    /// Возвращает функцию обработки длинного нажатия на элемент меню item.
    void ExecuteFuncForLongPressureOnItem(void *item);

    bool NeedForFireSetLED();

    void DrawOpenedPage(Page *page, int layer, int yTop);

    void DrawTitlePage(Page *page, int layer, int yTop);

    void DrawItemsPage(Page *page, int layer, int yTop);

    void DrawPagesUGO(Page *page, int right, int bottom);

    int CalculateX(int layer);

    void ResetItemsUnderButton();

    int ItemOpenedPosY(void *item);

    void* RetLastOpened(Page *_page, TypeItem *_type);
};

extern Menu menu;

/** @}
 */
