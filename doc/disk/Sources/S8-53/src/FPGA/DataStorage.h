#pragma once


#include "../defines.h"
#include "../Settings/SettingsTypes.h"
#include "../Hardware/RTC.h"


void            DS_Clear(void);                                                                     // Удаление всех сохранённых измерений
void            DS_AddData(uint8 *data0, uint8 *data1, DataSettings dss);                           // Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
int             DS_NumElementsWithSameSettings(void);                                               // Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
int             DS_NumElementsWithCurrentSettings(void);                                            // Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
int             DS_NumElementsInStorage(void);                                                      // Возвращает количество сохранённых измерений
bool            DS_GetDataFromEnd(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1);    // Получить указатель на данные
uint8*          DS_GetData(Channel chan, int fromEnd);
uint8*          DS_GetAverageData(Channel chan);                                                    // Получить усреднённые данные по нескольким измерениям.
int             DS_AllDatas(void);                                                                  // Сколько всего измерений сохранено в памяти.
uint8*          DS_GetLimitation(Channel chan, int direction);                                      // Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
int             DS_NumberAvailableEntries(void);
