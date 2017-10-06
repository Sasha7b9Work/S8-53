#pragma once
#include "defines.h"
#include "FPGA_Types.h"
#include "Panel/Controls.h"
#include "DataStorage.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"


void	    FPGA_Init(void);
void	    FPGA_SetNumSignalsInSec(int numSigInSec);   // Установить количество считываемых сигналов в секунду.
void        FPGA_Update(void);
void        FPGA_WriteToHardware				        // Запись в регистр ПЛИС нового значения.
			    			(uint8 *address,            // адрес регистра.
				    		uint8 value,                // записываемое значение.
					    	bool restart                // true означает, что после записи нужно запусить режим измерений, если до этого прибор не находился в режиме паузы.
						    );
StateWorkFPGA FPGA_CurrentStateWork(void);
void        FPGA_OnPressStartStop(void);                // Запускает цикл сбора информации.
void        FPGA_Start(void);							// Запуск процесса сбора информации.
void        FPGA_Stop(bool pause);                      // Прерывает процесс сбора информации.
bool        FPGA_IsRunning(void);                       // Возвращает true, если прибор находится не в процессе сбора информации.
void        FPGA_SaveState(void);                       // Сохраняет текущие настройки. Потом их можно восстановить функцией FPGA_RestoreState().
void        FPGA_RestoreState(void);                    // Восстанавливает настройки, ранее сохранённые функцией FPGA_SaveState().
float       FPGA_GetFreq(void);                         // Получить значение частоты.
void        FPGA_ClearData(void);                       // Удаляет данные. Нужно для режима рандомизаотра, где информация каждого цикла не является самостоятельной.
void        FPGA_SetAdditionShift(int shift);           // Установить дополнительное смещение. Нужно для правильной расстановки точек в режиме рандомизатора.
bool        FPGA_AllPointsRandomizer(void);             // Возвращает true,если все точки получены в режиме рандомизатора.
void        FPGA_SetNumberMeasuresForGates(int number); // Установить количество измерений, по которым будут рассчитываться ворота в режиме рандомизатора.
void        FPGA_SwitchingTrig(void);                   // Принудительно запустить синхронизацию.
void        FPGA_StartAutoFind(void);					// Запустить процесс поиска сигнала.
void        FPGA_TemporaryPause(void);                  // Установить временную паузу после изменения ручек - чтобы смещённый сигнал зафиксировать на некоторое время
void        FPGA_FillDataPointer(DataSettings *dp);
void        FPGA_FindAndSetTrigLevel(void);             // Найти и установить уровень синхронизации по последнему считанному сигналу
    
void        FPGA_LoadSettings(void);                                  // Загрузить настройки в аппаратную часть из глобальной структуры SSettings.
void        FPGA_SetModeCouple(Channel chan, ModeCouple modeCoupe);   // Установить режим канала по входу.
void        FPGA_EnableChannelFiltr(Channel chan, bool enable);       // Включить/выключить фильтр на входе канала.
void        FPGA_SetRange(Channel chan, Range range);                 // Установить масштаб по напряжению.
bool        FPGA_RangeIncrease(Channel chan);                         // Увеличить масштаб по напряжению.
bool        FPGA_RangeDecrease(Channel chan);                         // Уменьшить масштаб по напряжению.
void        FPGA_SetTBase(TBase tBase);                               // Установить масштаб по времени.
void        FPGA_TBaseDecrease(void);                                 // Уменьшить масштаб по времени.
void        FPGA_TBaseIncrease(void);                                 // Увеличить масштаб по времени.
void        FPGA_SetRShift(Channel chan, int16 rShift);               // Установить относительное смещение по напряжению.
void        FPGA_SetTShift(int tShift);                               // Установить относительное смещение по времени.
void        FPGA_SetDeltaTShift(int16 shift);                         // Установить добавочное смещение по времени для режима рандомизатора. В каждой развёртке это смещение должно быть разное.
void        FPGA_SetPeackDetMode(PeackDetMode peackDetMode);          // Включить/выключить режим пикового детектора.
void        FPGA_SetCalibratorMode(CalibratorMode calibratorMode);    // Включить/выключить калибратор.
void        FPGA_LoadKoeffCalibration(Channel chan);                  // Загрузить в аппарат коэффициенты калибровки каналов.

void        FPGA_SetTrigLev(TrigSource chan, int16 trigLev);          // Установить относительный уровень синхронизации.
void        FPGA_SetTrigSource(TrigSource trigSource);                // Установить источник синхронизации.
void        FPGA_SetTrigPolarity(TrigPolarity polarity);              // Установить полярность синхронизации.
void        FPGA_SetTrigInput(TrigInput trigInput);                   // Установить режим входа синхронизации.

const char* FPGA_GetTShiftString(int16 tShiftRel, char buffer[20]);                    // Возвращает установленное смещение по времени в текстовом виде, пригодном для вывода на экран.
