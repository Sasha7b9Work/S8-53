#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"
#include "Hardware/RTC.h"


class DataStorage
{
public:
    /// Удаление всех сохранённых измерений
    void Clear();
    /// Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    void AddData(uint8 *data0, uint8 *data1, DataSettings dss);
    /// Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    int NumElementsWithSameSettings();
    /// Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    int NumElementsWithCurrentSettings();
    /// Возвращает количество сохранённых измерений
    int NumElementsInStorage();
    /// Получить указатель на данные
    bool GetDataFromEnd(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1);

    uint8* GetData(Channel chan, int fromEnd);
    /// Получить усреднённые данные по нескольким измерениям.
    uint8* GetAverageData(Channel chan);
    /// Сколько всего измерений сохранено в памяти.
    int AllDatas();
    /// Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    uint8* GetLimitation(Channel chan, int direction);                                      

    int NumberAvailableEntries();
private:

    void CalculateSums(void);
    /// Возвращает количество свободной памяти в байтах
    int MemoryFree();
    /// Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    int SizeElem(DataSettings *dp);
    /// Удалить первое (самое старое) измерение
    void RemoveFirstElement();
    /// Сохранить данные
    void PushData(DataSettings *dp, uint8 *data0, uint8 *data1);
    /// Возвращает указатель на измерение, следующее за elem
    DataSettings* NextElem(DataSettings *elem);
    /// Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
    DataSettings* FromEnd(int indexFromEnd);
    /// Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);
    /// Возващает true, если настройки в обоих структурах одинаковы
    bool SettingsIsEquals(DataSettings *dp0, DataSettings *dp1);
    /// Очистка значений мин, макс и сумм
    void ClearLimitsAndSums();

    void CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss);

    DataSettings* GetSettingsDataFromEnd(int fromEnd);
    /// Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое значение false означает, что данный канал выключен.
    bool CopyData(DataSettings *ds, Channel chan, uint8 datatImportRel[NumChannels][FPGA_MAX_POINTS]);

    void PrintElement(DataSettings *dp);
};

extern DataStorage dataStorage;
