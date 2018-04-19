#pragma once
#include "Settings/SettingsTypes.h"


/** @addtogroup Menu
 *  @{
 *  @defgroup PageCursors
 *  @{
 */


void  Cursors_Update();    // Вызываем эту функцию для каждого измерения, чтобы обновить положие курсоров, если они должны обновляться автоматически.


class PageCursors
{
public:
    
    class PageSet
    {
    public:
        static void *pointer;
    };

    static void *GetPointer();
};


/** @}  @}
 */
