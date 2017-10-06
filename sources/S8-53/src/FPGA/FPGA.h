#pragma once
#include "defines.h"
#include "FPGA_Types.h"
#include "Panel/Controls.h"
#include "DataStorage.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"


class FPGA
{
public:

    void Init();
    /// Установить количество считываемых сигналов в секунду.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();
    /// Запись в регистр ПЛИС нового значения.
    void WriteToHardware				        
                            (uint8 *address,    ///< адрес регистра.
                                uint8 value,    ///< записываемое значение.
                                bool restart    ///< true означает, что после записи нужно запусить режим измерений, если до этого прибор не находился в режиме паузы.
                            );

    StateWorkFPGA FPGA_CurrentStateWork();
    /// Запускает цикл сбора информации.
    void OnPressStartStop();
    /// Запуск процесса сбора информации.
    void Start();
    /// Прерывает процесс сбора информации.
    void Stop(bool pause);
    /// Возвращает true, если прибор находится не в процессе сбора информации.
    bool IsRunning();
    /// Сохраняет текущие настройки. Потом их можно восстановить функцией FPGA_RestoreState().
    void SaveState();
    /// Восстанавливает настройки, ранее сохранённые функцией FPGA_SaveState().
    void RestoreState();
    /// Получить значение частоты.
    float GetFreq();
    /// Удаляет данные. Нужно для режима рандомизаотра, где информация каждого цикла не является самостоятельной.
    void ClearData();
    /// Установить дополнительное смещение. Нужно для правильной расстановки точек в режиме рандомизатора.
    void SetAdditionShift(int shift);
    /// Возвращает true,если все точки получены в режиме рандомизатора.
    bool AllPointsRandomizer();
    /// Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора.
    void SetNumberMeasuresForGates(int number);
    /// Принудительно запустить синхронизацию.
    void SwitchingTrig();
    /// Запустить процесс поиска сигнала.
    void StartAutoFind();
    /// Установить временную паузу после изменения ручек - чтобы смещённый сигнал зафиксировать на некоторое время
    void TemporaryPause();

    void FillDataPointer(DataSettings *dp);
    /// Найти и установить уровень синхронизации по последнему считанному сигналу
    void FindAndSetTrigLevel();
    /// Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
    void LoadSettings();
    /// Установить режим канала по входу.
    void SetModeCouple(Channel chan, ModeCouple modeCoupe);
    /// Включить/выключить фильтр на входе канала.
    void EnableChannelFiltr(Channel chan, bool enable);
    /// Установить масштаб по напряжению.
    void SetRange(Channel chan, Range range);
    /// Увеличить масштаб по напряжению.
    bool RangeIncrease(Channel chan);
    /// Уменьшить масштаб по напряжению.
    bool RangeDecrease(Channel chan);
    /// Установить масштаб по времени.
    void SetTBase(TBase tBase);
    /// Уменьшить масштаб по времени.
    void TBaseDecrease();
    /// Увеличить масштаб по времени.
    void TBaseIncrease();
    /// Установить относительное смещение по напряжению.
    void SetRShift(Channel chan, int16 rShift);
    /// Установить относительное смещение по времени.
    void SetTShift(int tShift);
    /// Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное.
    void SetDeltaTShift(int16 shift);
    /// Включить/выключить режим пикового детектора.
    void SetPeackDetMode(PeackDetMode peackDetMode);
    /// Включить/выключить калибратор.
    void SetCalibratorMode(CalibratorMode calibratorMode);
    /// Загрузить в аппарат коэффициенты калибровки каналов.
    void LoadKoeffCalibration(Channel chan);
    /// Установить относительный уровень синхронизации.
    void SetTrigLev(TrigSource chan, int16 trigLev);
    /// Установить источник синхронизации.
    void SetTrigSource(TrigSource trigSource);
    /// Установить полярность синхронизации.
    void SetTrigPolarity(TrigPolarity polarity);
    /// Установить режим входа синхронизации.
    void SetTrigInput(TrigInput trigInput);
    /// Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.
    const char* GetTShiftString(int16 tShiftRel, char buffer[20]);
    /// Запуск функции калибровки.
    void ProcedureCalibration();

    StateWorkFPGA CurrentStateWork();

private:
    /// Загрузка коэффицента развёртки в аппаратную часть.
    void LoadTBase();
    /// Загрузка смещения по времени в аппаратную часть.
    void LoadTShift();
    /// Загрузка масштаба по напряжению в аппаратную часть.
    void LoadRange(Channel chan);
    /// Загрузка смещения по напряжению в аппаратную часть.
    void LoadRShift(Channel chan);
    /// Загрузка уровня синхронизации в аппаратную часть.
    void LoadTrigLev();                                  
    /// Загузка полярности синхронизации в аппаратную часть.
    void LoadTrigPolarity();
    /// Загрузить все параметры напряжения каналов и синхронизации в аппаратную часть.
    void SetAttribChannelsAndTrig(TypeWriteAnalog type);
    /// Загрузить регистр WR_UPR (пиковый детектор и калибратор).
    void LoadRegUPR();

    void WriteToAnalog(TypeWriteAnalog type, uint data);

    void WriteToDAC(TypeWriteDAC type, uint16 data);
    /// Прочитать данные.
    void DataRead(
                        bool necessaryShift,    ///< Признак того, что сигнал нужно смещать.
                        bool saveToStorage      ///< Нужно в режиме рандомизатора для указания, что пора сохранять измерение
                        );

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    int CalculateShift();
    /// Инвертирует данные.
    void InverseDataIsNecessary(Channel chan, uint8 *data);

    void AutoFind();

    uint8 CalculateMin(uint8 buffer[100]);

    uint8 CalculateMax(uint8 buffer[100]);

    bool ProcessingData();

    void ReadRandomizeMode();

    void ReadRealMode(bool necessaryShift);

    Range FindRange(Channel chan);

    Range AccurateFindRange(Channel chan);

    bool FindParams(Channel chan, TBase *tBase);

    bool AccurateFindParams(Channel chan);

    bool FindWave(Channel chan);
};


extern FPGA fpga;
