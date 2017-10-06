#include "defines.h"
#include "HelpContentPages.h"
#include "HelpContent.h"
#include "Display/Painter.h"
#include "Display/Grid.h"
#include "Display/font/Font.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"


int currentParagraph = 0;   // ≈сли TypePage(currentPage) == TypePage_Content, то указывает не текущий раздел оглавлени€
const PageHelp *currentPage = &helpMain;


static const int WIDTH = 295;


static void DrawPageContent()
{
    int y = 50;
    painter.DrawStringInCenterRect(0, y, WIDTH, 10, TITLE(currentPage));

    int numPage = 0;

    y += 40;

    while(currentPage->pages[numPage])
    {
        Page *page = (Page*)currentPage->pages[numPage];
        const char *title = TITLE(page);
        if(currentParagraph == numPage)
        {
            painter.DrawStringInCenterRectOnBackgroundC(0, y, WIDTH, 10, title, COLOR_BACK, 2, COLOR_FILL);
        }
        else
        {
            painter.DrawStringInCenterRectC(0, y, WIDTH, 10, title, COLOR_FILL);
        }
        y += 16;
        numPage++;
    }
}

static void DrawPageDescription()
{
    painter.DrawStringInCenterRect(0, 3, WIDTH, 10, TITLE(currentPage));
    painter.DrawTextInRectWithTransfers(2, 15, WIDTH - 5, 240, HINT(currentPage));
}


void HelpContent_Draw()
{
    uint startTime = gTimerMS;
    painter.FillRegionC(grid.Right(), 0, 319 - grid.Right(), 20, COLOR_BACK);
    painter.FillRegion(grid.Right(), 219, 319 - grid.Right(), 21);
    painter.FillRegion(1, 1, WIDTH, 237);
    painter.DrawRectangleC(0, 0, WIDTH + 2, 239, COLOR_FILL);

    /*
    uint16 *addr1 = (uint16*)(0x08000000 + (rand() % 65535));
    uint8 *addr2 = (uint8*)(0x08000000 + (rand() % 65535));

    painter.SetColor(COLOR_FILL);
    for (int i = 0; i < 10000; i++)
    {
        painter.SetPoint((*addr1) % WIDTH, Math_LimitationInt(*addr2, 0, 239));
        addr1++;
        addr2++;
    }
    */

    if(currentPage->type == TypePage_Content)
    {
        DrawPageContent();
    }
    else if(currentPage->type == TypePage_Description)
    {
        DrawPageDescription();
    }
    painter.DrawFormatText(2, 230, COLOR_FILL, "%d", gTimerMS - startTime);
}

static int NumParagraphs(const PageHelp *page)
{
    int retValue = 0;
    while(page->pages[retValue])
    {
        retValue++;
    }
    return retValue;
}

void HelpContent_NextParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        CircleIncreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

void HelpContent_PrevParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        CircleDecreaseInt(&currentParagraph, 0, NumParagraphs(currentPage) - 1);
    }
}

void HelpContent_EnterParagraph()
{
    if(currentPage->type == TypePage_Content)
    {
        currentPage = (const PageHelp *)currentPage->pages[currentParagraph];
    }
    currentParagraph = 0;
}

void HelpContent_LeaveParagraph()
{
    currentParagraph = 0;
    if(currentPage->parent)
    {
        currentPage = (const PageHelp *)currentPage->parent;
    }
}

bool HelpContent_LeaveParagraphIsActive()
{
    return currentPage->parent != 0;
}

bool HelpContent_EnterParagraphIsActive()
{
    return currentPage->type == TypePage_Content;
}
