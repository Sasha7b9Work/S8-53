#pragma once
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/SettingsTypes.h"


class Storage
{
public:
    /// Удаление всех сохранённых измерений
    static void Clear();
    /// Добавить считанные данные. При этом настройками считаются текущие настройки прибора.
    static void AddData(uint8 *data0, uint8 *data1, DataSettings dss);
    /// Возвращает число непрерывных измерений, начиная с последнего, с такими же настройками, как у последнего.
    static int NumElementsWithSameSettings();
    /// Возвращает число непрерывных измерений, начиная с последнего, с текущими настройками прибора
    static int NumElementsWithCurrentSettings();
    /// Возвращает количество сохранённых измерений
    static int NumElementsInStorage();
    /// Получить указатель на данные
    static bool GetDataFromEnd(int fromEnd, DataSettings **ds, uint8 **data0, uint8 **data1);

    static uint8* GetData(Channel chan, int fromEnd);
    /// Получить усреднённые данные по нескольким измерениям.
    static uint8* GetAverageData(Channel chan);
    /// Сколько всего измерений сохранено в памяти.
    static int AllDatas();
    /// Получить ограничивающую линию сигнала 0 - снизу, 1 - сверху.
    static uint8* GetLimitation(Channel chan, int direction);                                      

    static int NumberAvailableEntries();
private:

    static void CalculateSums(void);
    /// Возвращает количество свободной памяти в байтах
    static int MemoryFree();
    /// Вычисляет, сколько памяти трубуется, чтобы сохранить измерения с настройками dp
    static int SizeElem(DataSettings *dp);
    /// Удалить первое (самое старое) измерение
    static void RemoveFirstElement();
    /// Сохранить данные
    static void PushData(DataSettings *dp, uint8 *data0, uint8 *data1);
    /// Возвращает указатель на измерение, следующее за elem
    static DataSettings* NextElem(DataSettings *elem);
    /// Возвращает указатель на данные, отстоящие на indexFromEnd oт последнего сохранённого
    static DataSettings* FromEnd(int indexFromEnd);
    /// Возвращает true, если настройки измерений с индексами elemFromEnd0 и elemFromEnd1 совпадают, и false в ином случае.
    static bool SettingsIsIdentical(int elemFromEnd0, int elemFromEnd1);
    /// Возващает true, если настройки в обоих структурах одинаковы
    static bool SettingsIsEquals(DataSettings *dp0, DataSettings *dp1);
    /// Очистка значений мин, макс и сумм
    static void ClearLimitsAndSums();

    static void CalculateLimits(uint8 *data0, uint8 *data1, DataSettings *dss);

    static DataSettings* GetSettingsDataFromEnd(int fromEnd);
    /// Копирует данные канала chan из, определяемые ds, в одну из двух строк массива dataImportRel. Возвращаемое значение false означает, что данный канал выключен.
    static bool CopyData(DataSettings *ds, Channel chan, uint8 datatImportRel[NumChannels][FPGA_MAX_POINTS]);

    static void PrintElement(DataSettings *dp);
    
    static void CalculateAroundAverage(uint8 *data0, uint8 *data1, DataSettings *dss);
    /// Количество отведённой для измерений памяти.
    static const int SIZE_POOL = (30 * 1024);
    /// Здесь хранятся данные.
    static uint8 pool[SIZE_POOL];
    /// Адрес начала памяти для хранения
    static uint8* beginPool;
    /// Адрес последнего байта памяти для хранения
    static uint8* endPool;
    /// Здесь хранятся суммы измерений обоих каналов
    static uint  sum[NumChannels][FPGA_MAX_POINTS];
    /// Максимальные значения каналов
    static uint8 limitUp[NumChannels][FPGA_MAX_POINTS];
    /// Минимальные значения каналов
    static uint8 limitDown[NumChannels][FPGA_MAX_POINTS];
    /// Указатель на первые сохранённые данные
    static DataSettings* firstElem;
    /// Указатель на последние сохранённые данные
    static DataSettings* lastElem;
    /// Всего данных сохранено
    static int allData;
    /// В этих массивах хранятся усреднённые значения, подсчитанные по приблизительному алгоритму.
    static float aveData0[FPGA_MAX_POINTS];

    static float aveData1[FPGA_MAX_POINTS];
    /// Если true, то новые суммы рассчитаны, и нужно повторить расчёт среднего
    static bool newSumCalculated[NumChannels];

};
