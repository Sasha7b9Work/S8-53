#pragma once
#include "MenuItems.h"


void GovernorColor_Draw(GovernorColor *govColor, int x, int y, bool opened);
void ItemGovernor_Draw(Governor *governor, int x, int y, bool opened);
void ItemIPaddress_Draw(IPaddress *ip, int x, int y, bool opened);
void ItemMACaddress_Draw(MACaddress *mac, int x, int y, bool opened);
void ItemFormula_Draw(Formula *formula, int x, int y, bool opened);
void ItemChoice_Draw(Choice *choice, int x, int y, bool opened);
void ItemButton_Draw(Button *button, int x, int y);
void ItemSmallButton_Draw(SmallButton *smallButton, int x, int y);
void ItemPage_Draw(Page *page, int x, int y);
void ItemTime_Draw(Time *time, int x, int y, bool opened);
