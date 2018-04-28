#include "FPGA.h"
#include "Storage.h"
#include "FPGA_Types.h"
#include "Menu/Pages/PageMemory.h"
#include "Display/Display.h"
#include "Hardware/FSMC.h"
#include "Hardware/Hardware.h"
#include "Hardware/ADC.h"
#include "Panel/Panel.h"
#include "Panel/Controls.h"
#include "Settings/Settings.h"
#include "Settings/SettingsChannel.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include "Utils/Generator.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Settings/Settings.h"
#include "Utils/Generator.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NULL_TSHIFT 1000000

static float freq = 0.0f;           // Частота, намеренная альтерой.
static float prevFreq = 0.0f;
static StateWorkFPGA stateWork = StateWorkFPGA_Stop;


volatile static int numberMeasuresForGates = 1000;

static int additionShift = 0;
#define n 200
static const int Kr[] = {n / 2, n / 5, n / 10, n / 20, n / 50};

static DataSettings ds;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8 dataRel0[FPGA_MAX_POINTS] = {0};   ///< Буфер используется для чтения данных первого канала.
static uint8 dataRel1[FPGA_MAX_POINTS] = {0};   ///< Буфер используется для чтения данных второго канала.

static Settings storingSettings;                ///< Здесь нужно уменьшить необходимый размер памяти - сохранять настройки только альтеры
static uint timeStart = 0;

/// Функция вызывается, когда можно считывать очередной сигнал.
static void OnTimerCanReadData();

static void ReadPoint();

static uint8 ReadFlag();

static void ReadFreq();

static void ReadPeriod();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FPGA::Init(void) 
{
    Storage::Clear();
    FPGA::LoadSettings();
    FPGA::SetNumSignalsInSec(sDisplay_NumSignalsInS());
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
    FPGA::SetNumberMeasuresForGates(NUM_MEAS_FOR_GATES);
} 

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetNumSignalsInSec(int numSigInSec) 
{
    Timer::Enable(kNumSignalsInSec, 1000.f / numSigInSec, OnTimerCanReadData);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void OnTimerCanReadData(void)
{
    FPGA_CAN_READ_DATA = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Start(void)
{
    if(SET_TBASE >= MIN_TBASE_P2P)
    {
        Display::ResetP2Ppoints(false);
        Timer::Enable(kP2P, 1, ReadPoint);
    }
    else
    {
        Timer::Disable(kP2P);
        Display::ResetP2Ppoints(true);
    }
    FSMC::Write(WR_START, 1);
    FillDataPointer(&ds);
    timeStart = gTimerMS;
    stateWork = StateWorkFPGA_Wait;
    FPGA_CRITICAL_SITUATION = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SwitchingTrig(void)
{
    FSMC::Write(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x00U : 0x01U);
    FSMC::Write(WR_TRIG_F, TRIG_POLARITY_IS_FRONT ? 0x01U : 0x00U);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::ProcessingData(void)
{
    bool retValue = false;

    int num = (sTime_RandomizeModeEnabled() && (!START_MODE_IS_SINGLE) && SAMPLE_TYPE_IS_EQUAL) ? Kr[SET_TBASE] : 1;
    
   for (int i = 0; i < num; i++)
   {
        uint8 flag = ReadFlag();
        if (FPGA_CRITICAL_SITUATION)
        {
            if (gTimerMS - timeStart > 500)
            {
                SwitchingTrig();
                TRIG_AUTO_FIND = 1;
                FPGA_CRITICAL_SITUATION = 0;
            }
            else if (_GET_BIT(flag, BIT_TRIG))
            {
                FPGA_CRITICAL_SITUATION = 0;
            }
        }
        else if (_GET_BIT(flag, BIT_DATA_READY))
        {
            if (set.debug.showRegisters.flag)
            {
                char buffer[9];
                LOG_WRITE("флаг готовности %s", Bin2String(flag, buffer));
            }
            Panel::EnableLEDTrig(true);
            FPGA::Stop(true);
            DataRead(_GET_BIT(flag, BIT_SIGN_SHIFT_POINT), (num == 1) || (i == num - 1));
            retValue = true;
            if (!START_MODE_IS_SINGLE)
            {
                Start();
                stateWork = StateWorkFPGA_Work;
            }
            else
            {
                FPGA::Stop(false);
            }
        }
        else
        {
            if (flag & (1 << 2))
            {
                if (START_MODE_IS_AUTO)
                {
                    FPGA_CRITICAL_SITUATION = 1;
                }
                timeStart = gTimerMS;
            }
        }
        Panel::EnableLEDTrig(_GET_BIT(flag, BIT_TRIG) ? true : false);
    }

    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Update(void)
{
    ReadFlag();

    if (gStateFPGA.needCalibration)              // Если вошли в режим калибровки -
    {
        FPGA::ProcedureCalibration();            // выполняем её.
        gStateFPGA.needCalibration = false;
    }
    if (FPGA_TEMPORARY_PAUSE)
    {
        return;
    }

	if(AUTO_FIND_IN_PROGRESS)
    {
		AutoFind();
		return;
	}

    //if(((FPGA_CAN_READ_DATA == 0) && !sTime_RandomizeModeEnabled()) || (stateWork == StateWorkFPGA_Stop))
    if((FPGA_CAN_READ_DATA == 0) || (stateWork == StateWorkFPGA_Stop))
    {
        return;
    }

    if(SET_SELFRECORDER)
    {
        ReadPoint();
    }

    ProcessingData();

    FPGA_CAN_READ_DATA = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
StateWorkFPGA FPGA::CurrentStateWork(void)
{
    return stateWork;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::OnPressStartStop(void)
{
    if(stateWork == StateWorkFPGA_Stop) 
    {
        FPGA::Start();
    } 
    else
    {
        FPGA::Stop(false);
    } 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::Stop(bool pause) 
{
    Timer::Disable(kP2P);
    FSMC::Write(WR_STOP, 1);
    stateWork = pause ? StateWorkFPGA_Pause : StateWorkFPGA_Stop;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::IsRunning(void)
{
    return stateWork != StateWorkFPGA_Stop;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                      \
    if(SET_INVERSE(ch))                                                                   \
    {                                                                                               \
        data = (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8(data, MIN_VALUE, MAX_VALUE));    \
    }                                                                                               \
    *addr = data;

/*
static uint8 InverseIfNecessary(uint8 data, Channel chan)
{
    if (set.chan[chan].inverse)  
    {
        return (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8(data, MIN_VALUE, MAX_VALUE));
    }
    return data;
}
*/

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::ReadRandomizeMode(void)
{
    int Tsm = CalculateShift();
    if (Tsm == NULL_TSHIFT)
    {
        return;
    };

    int step = Kr[SET_TBASE];
    int index = Tsm - step - additionShift;

    if (index < 0)
    {
        index += step;        // WARN
    }

    uint8 *pData0 = &dataRel0[index];
    const uint8 * const pData0Last = &dataRel0[FPGA_MAX_POINTS - 1];
    uint8 *pData1 = &dataRel1[index];
    const uint8 * const pData1Last = &dataRel1[FPGA_MAX_POINTS - 1];

    const uint8 * const first0 = &dataRel0[0];
    const uint8 * const last0 = pData0Last;
    const uint8 * const first1 = &dataRel1[0];
    const uint8 * const last1 = pData1Last;

    int numAve = NUM_AVE_FOR_RAND;

    if (sDisplay_NumAverage() > numAve)
    {
        numAve = sDisplay_NumAverage();
    }

    int addShiftMem = step / 2;

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        FPGA::ClearData();
    }

    while (pData0 < &dataRel0[FPGA_MAX_POINTS])
    {
        uint8 data10 = *RD_ADC_B2;
        //uint8 data11 = *RD_ADC_B1;
        uint8 data00 = *RD_ADC_A2;
        //uint8 data01 = *RD_ADC_A1;

        /*
        if (*pData0 == 0 || numAve == 1 || startMode == StartMode_Single)
        {
        */
            if (pData0 >= first0 && pData0 <= last0)
            {
                WRITE_AND_OR_INVERSE(pData0, data00, A);
            }

            uint8 *addr = pData0 + addShiftMem;
            if (addr >= first0 && addr <= last0)
            {
//                WRITE_AND_OR_INVERSE(addr, data01, A);
            }

            if (pData1 >= first1 && pData1 <= last1)
            {
                WRITE_AND_OR_INVERSE(pData1, data10, B);
            }
            addr = pData1 + addShiftMem;
            if (addr >= first1 && addr <= last1)
            {
//                WRITE_AND_OR_INVERSE(addr, data11, B);
            }
        /*
        }
        else
        {
            if (pData0 >= first0 && pData0 <= last0)
            {
                *pData0 = (float)(numAve - 1) / (float)(numAve)* (*pData0) + InverseIfNecessary(data00, A) * 1.0f / (float)numAve;
            }

            uint8 *addr = pData0 + addShiftMem;
            if (addr >= first0 && addr <= last0)
            {
                *addr = (float)(numAve - 1) / (float)(numAve)* (*(pData0 + addShiftMem)) + InverseIfNecessary(data01, A) * 1.0f / (float)numAve;
            }

            if (pData1 >= first1 && pData1 <= last1)
            {
                *pData1 = (float)(numAve - 1) / (float)(numAve)* (*pData1) + InverseIfNecessary(data10, B) * 1.0f / (float)numAve;
            }

            addr = pData1 + addShiftMem;

            if (addr >= first1 && addr <= last1)
            {
                *addr = (float)(numAve - 1) / (float)(numAve)* (*(pData1 + addShiftMem)) + InverseIfNecessary(data11, B) * 1.0f / (float)numAve;
            }
        }
        */

        pData0 += step;
        pData1 += step;
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        Processing::InterpolationSinX_X(dataRel0, SET_TBASE);
        Processing::InterpolationSinX_X(dataRel1, SET_TBASE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::ReadRealMode(bool necessaryShift)
{
    uint8 *p0 = &dataRel0[0];
    uint8 *p1 = &dataRel1[0];
    uint8 *endP = &dataRel0[FPGA_MAX_POINTS];

    if (ds.peakDet != PeackDet_Disable)
    {
        uint8 *p0min = p0;
        uint8 *p0max = p0min + 512;
        uint8 *p1min = p1;
        uint8 *p1max = p1min + 512;
        while (p0max < endP && FPGA_IN_PROCESS_READ)
        {
            uint8 data = *RD_ADC_B2;
            *p1max++ = data;
            data = *RD_ADC_B1;
            *p1min++ = data;
            data = *RD_ADC_A2;
            *p0min++ = data;
            data = *RD_ADC_A1;
            *p0max++ = data;
        }
    }
    else
    {
        while (p0 < endP && FPGA_IN_PROCESS_READ)
        {
            *p1++ = *RD_ADC_B2;
            *p1++ = *RD_ADC_B1;
            uint8 data = *RD_ADC_A2;
            *p0++ = data;
            data = *RD_ADC_A1;
            *p0++ = data;
        }

        int shift = 0;
        if (SET_TBASE == TBase_100ns || SET_TBASE == TBase_200ns)
        {
            shift = CalculateShift();
        }
        else if (necessaryShift)
        {
            //shift = set.debug.altShift;       WARN Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, можно включить при необходимости
            shift = -1;
        }
        if (shift != 0)
        {
            if (shift < 0)
            {
                shift = -shift;
                for (int i = FPGA_MAX_POINTS - shift - 1; i >= 0; i--)
                {
                    dataRel0[i + shift] = dataRel0[i];
                    dataRel1[i + shift] = dataRel1[i];
                }
            }
            else
            {
                for (int i = shift; i < FPGA_MAX_POINTS; i++)
                {
                    dataRel0[i - shift] = dataRel0[i];
                    dataRel1[i - shift] = dataRel1[i];
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::DataRead(bool necessaryShift, bool saveToStorage) 
{
    Panel::EnableLEDTrig(false);
    FPGA_IN_PROCESS_READ = 1;
    if((TBase)ds.tBase < TBase_100ns)
    {
        ReadRandomizeMode();
    } 
    else 
    {
        ReadRealMode(necessaryShift);
    }

    static uint prevTime = 0;

    if (saveToStorage || (gTimerMS - prevTime > 500))
    {
        prevTime = gTimerMS;
        if (!sTime_RandomizeModeEnabled())
        {
            InverseDataIsNecessary(A, dataRel0);
            InverseDataIsNecessary(B, dataRel1);
        }

        Storage::AddData(dataRel0, dataRel1, ds);

        if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
        {
            FPGA::FindAndSetTrigLevel();
            TRIG_AUTO_FIND = 0;
        }
    }
    FPGA_IN_PROCESS_READ = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetAdditionShift(int shift)
{
    additionShift = shift;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{   
    if(FPGA_FIRST_AFTER_WRITE)
    {
        FPGA_FIRST_AFTER_WRITE = 0;
        return false;
    }
    
    if(rand < 500 || rand > 3500)
    {
        LOG_WRITE("ОШИБКА!!! считано %d", rand);
        return false;
    }
    
    static const int numValues = 1000;

    static float minGate = 0.0f;
    static float maxGate = 0.0f;
    static int numElements = 0;
    static uint16 min = 0xffff;
    static uint16 max = 0;

    numElements++;
    if(rand < min)
    {
        min = rand;
    }
    if(rand > max)
    {
        max = rand;
    }

    if(minGate == 0)
    {
        *eMin = min;
        *eMax = max;
        if(numElements < numValues)
        {
            //LOG_WRITE("min %u, max %u, rand %d", *eMin, *eMax, rand);
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if(numElements == numValues)
    {
        minGate = 0.9f * minGate + 0.1f * min;
        maxGate = 0.9f * maxGate + 0.1f * max;
        //LOG_WRITE("%.0f ... %.0f, min = %u, max = %u", minGate, maxGate, min, max);
        numElements = 0;
        min = 0xffff;
        max = 0;
    }
    *eMin = minGate;
    *eMax = maxGate;

    //LOG_WRITE("min %u, max %u, rand %d", *eMin, *eMax, rand);
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int FPGA::CalculateShift(void)            // \todo Не забыть восстановить функцию
{
    uint16 rand = ADConverter::GetValue();
    //LOG_WRITE("rand = %d", (int)rand);
    uint16 min = 0;
    uint16 max = 0;

    if (SET_TBASE == TBase_200ns)
    {
        return rand < 3000 ? 0 : -1;    /// set.debug.altShift; \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор 
                                        /// закомментирован, можно раскомментировать при необходимости
    }

    if (!CalculateGate(rand, &min, &max))
    {
        return NULL_TSHIFT;
    }
    
    //LOG_WRITE("ворота %d - %d", min, max);

    //min += 100;
    //max -= 100;

    if (sTime_RandomizeModeEnabled())
    {
        float tin = (float)(rand - min) / (max - min) * 10e-9;
        int retValue = tin / 10e-9 * Kr[SET_TBASE];
        return retValue;
    }

    if (SET_TBASE == TBase_100ns && rand < (min + max) / 2)
    {
        return 0;
    }

    return -1;  // set.debug.altShift;      \todo Остановились на жёстком задании дополнительного смещения. На PageDebug выбор закомментирован, 
                                            //можно раскомментировать при необходимости
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::WriteToHardware(uint8 *address, uint8 value, bool restart)
{
    FPGA_FIRST_AFTER_WRITE = 1;
    if(restart)
    {
        if(FPGA_IN_PROCESS_READ)
        {
            FPGA::Stop(true);
            FPGA_IN_PROCESS_READ = 0;
            FSMC::Write(address, value);
            FPGA::Start();
        }
        else
        {
            if(stateWork != StateWorkFPGA_Stop)
            {
                FPGA::Stop(true);
                FSMC::Write(address, value);
                FPGA::Start();
            }
            else
            {
                FSMC::Write(address, value);
            }
        }
    }
    else
    {
        FSMC::Write(address, value);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ReadPoint(void)
{
    if(_GET_BIT(ReadFlag(), BIT_POINT_READY))
    {
        uint8 dataB1 = *RD_ADC_B1;
        uint8 dataB2 = *RD_ADC_B2;
        uint8 dataA1 = *RD_ADC_A1;
        uint8 dataA2 = *RD_ADC_A2;
        Display::AddPoints(dataA2, dataA1, dataB2, dataB1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SaveState(void)
{
    gStateFPGA.stateWorkBeforeCalibration = stateWork;
    storingSettings = set;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::RestoreState(void)
{
    int16 rShiftAdd[2][RangeSize][2];
    for (int chan = 0; chan < 2; chan++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                rShiftAdd[chan][range][mode] = RSHIFT_ADD(chan, range, mode);
            }
        }
    }
    set = storingSettings;
    for (int chan = 0; chan < 2; chan++)
    {
        for (int mode = 0; mode < 2; mode++)
        {
            for (int range = 0; range < RangeSize; range++)
            {
                 RSHIFT_ADD(chan, range, mode) = rShiftAdd[chan][range][mode];
            }
        }
    }
    FPGA::LoadSettings();
    if(gStateFPGA.stateWorkBeforeCalibration != StateWorkFPGA_Stop)
    {
        gStateFPGA.stateWorkBeforeCalibration = StateWorkFPGA_Stop;
        FPGA::OnPressStartStop();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static bool readPeriod = false;     ///< Установленный в true флаг означает, что частоту нужно считать по счётчику периода

//----------------------------------------------------------------------------------------------------------------------------------------------------
static BitSet32 ReadRegFreq(void)
{
    BitSet32 fr;
    fr.byte[0] = FSMC::Read(RD_ADDR_FREQ_LOW);
    fr.byte[1] = FSMC::Read(RD_ADDR_FREQ_MID);
    fr.byte[2] = FSMC::Read(RD_ADDR_FREQ_HI);
    fr.byte[3] = 0;
    return fr;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static BitSet32 ReadRegPeriod(void)
{
    BitSet32 period;
    period.byte[0] = FSMC::Read(RD_ADDR_PERIOD_LOW_LOW);
    period.byte[1] = FSMC::Read(RD_ADDR_PERIOD_LOW);
    period.byte[2] = FSMC::Read(RD_ADDR_PERIOD_MID);
    period.byte[3] = FSMC::Read(RD_ADDR_PERIOD_HI);
    return period;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float FreqCounterToValue(BitSet32 *fr)
{
    return fr->word * 10.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float PeriodCounterToValue(BitSet32 *period)
{
    if (period->word == 0)
    {
        return 0.0f;
    }
    return 10e6f / (float)period->word;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void ReadFreq(void)                         // Чтение счётчика частоты производится после того, как бит 4 флага RD_FL установится в едицину
{                                           // После чтения автоматически запускается новый цикл счёта
    BitSet32 freqFPGA = ReadRegFreq();

    if(freqFPGA.word < 1000) 
    {
        readPeriod = true;
    } 
    else 
    {
        float fr = FreqCounterToValue(&freqFPGA);
        if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
        {
            freq = ERROR_VALUE_FLOAT;
        }
        else
        {
            freq = fr;
        }
        prevFreq = fr;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ReadPeriod(void)
{
    BitSet32 periodFPGA = ReadRegPeriod();
    float fr = PeriodCounterToValue(&periodFPGA);
    if (fr < prevFreq * 0.9f || fr > prevFreq * 1.1f)
    {
        freq = ERROR_VALUE_FLOAT;
    }
    else
    {
        freq = fr;
    }
    prevFreq = fr;
    readPeriod = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static uint8 ReadFlag(void)
{
    uint8 flag = FSMC::Read(RD_FL);
    if(!readPeriod) 
    {
        if(_GET_BIT(flag, BIT_FREQ_READY)) 
        {
            ReadFreq();
        }
    }
    if(readPeriod && _GET_BIT(flag, BIT_PERIOD_READY)) 
    {
        ReadPeriod();
    }

    return flag;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float CalculateFreqFromCounterFreq(void)
{
    while (_GET_BIT(FSMC::Read(RD_FL), BIT_FREQ_READY) == 0) {};
    ReadRegFreq();
    while (_GET_BIT(FSMC::Read(RD_FL), BIT_FREQ_READY) == 0) {};
    BitSet32 fr = ReadRegFreq();
    if (fr.word >= 5)
    {
        return FreqCounterToValue(&fr);
    }
    return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static float CalculateFreqFromCounterPeriod(void)
{
    uint time = gTimerMS;
    while (gTimerMS - time < 1000 && _GET_BIT(FSMC::Read(RD_FL), BIT_PERIOD_READY) == 0) {};
    ReadRegPeriod();
    time = gTimerMS;
    while (gTimerMS - time < 1000 && _GET_BIT(FSMC::Read(RD_FL), BIT_PERIOD_READY) == 0) {};
    BitSet32 period = ReadRegPeriod();
    if (period.word > 0 && (gTimerMS - time < 1000))
    {
        return PeriodCounterToValue(&period);
    }
    return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float FPGA::GetFreq(void) 
{
    return freq;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::ClearData(void)
{
    memset(dataRel0, 0, FPGA_MAX_POINTS);
    memset(dataRel1, 0, FPGA_MAX_POINTS);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::AllPointsRandomizer(void)
{
    if(SET_TBASE < TBase_100ns) 
    {
        for(int i = 0; i < 281; i++) 
        {
            if(dataRel0[i] == 0) 
            {
                return false;   
            }
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::InverseDataIsNecessary(Channel chan, uint8 *data)
{
    if(SET_INVERSE(chan))
    {
        for (int i = 0; i < FPGA_MAX_POINTS; i++)
        {
            data[i] = (uint8)((int)(2 * AVE_VALUE) - LimitationUInt8(data[i], MIN_VALUE, MAX_VALUE));
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::SetNumberMeasuresForGates(int number)
{
    numberMeasuresForGates = number;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::StartAutoFind(void)
{
	AUTO_FIND_IN_PROGRESS = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 FPGA::CalculateMinWithout0(uint8 buffer[100])
{
    /// \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    /// Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 min = 255;
    
    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] > 0 && buffer[i] < min)
        {
            min = buffer[i];
        }
    }
    return min;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8 FPGA::CalculateMaxWithout255(uint8 buffer[100])
{
    /// \todo На одном экземпляре был страшенныый глюк, когда без сигнала выбивались значения 0 и 255 в рандомных местах
    /// Вот такой кастыиль на скорую ногу, чтобы нули выкинуть.
    uint8 max = 0;
    for(int i = 1; i < 100; i++)
    {
        if(buffer[i] < 255 && buffer[i] > max)
        {
            max = buffer[i];
        }
    }
    return max;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
TBase CalculateTBase(float freq_)
{
    if     (freq_ >= 100e6)  { return TBase_2ns;   }
    else if(freq_ >= 40e6)   { return TBase_5ns;   }
    else if(freq_ >= 20e6)   { return TBase_10ns;  }
    else if(freq_ >= 10e6)   { return TBase_20ns;  }
    else if(freq_ >= 3e6)    { return TBase_50ns;  }
    else if(freq_ >= 2e6)    { return TBase_100ns; }
    else if(freq_ >= 900e3)  { return TBase_200ns; }
    else if(freq_ >= 400e3)  { return TBase_500ns; }
    else if(freq_ >= 200e3)  { return TBase_1us;   }
    else if(freq_ >= 90e3)   { return TBase_2us;   }
    else if(freq_ >= 30e3)   { return TBase_5us;   }
    else if(freq_ >= 20e3)   { return TBase_10us;  }
    else if(freq_ >= 10e3)   { return TBase_20us;  }
    else if(freq_ >= 4e3)    { return TBase_50us;  }
    else if(freq_ >= 2e3)    { return TBase_100us; }
    else if(freq_ >= 1e3)    { return TBase_200us; }
    else if(freq_ >= 350.0f) { return TBase_500us; }
    else if(freq_ >= 200.0f) { return TBase_1ms;   }
    else if(freq_ >= 100.0f) { return TBase_2ms;   }
    else if(freq_ >= 40.0f)  { return TBase_5ms;   }
    else if(freq_ >= 20.0f)  { return TBase_10ms;  }
    else if(freq_ >= 10.0f)  { return TBase_20ms;  }
    else if(freq_ >= 4.0f)   { return TBase_50ms;  }
    else if(freq_ >= 2.0f)   { return TBase_100ms; }
    return TBase_200ms;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::AutoFind(void)
{
    //LOG_WRITE(" ");
    //Timer::StartLogging();

    //LOG_WRITE("Канал 1");
    if (!FindWave(A))
    {
        //LOG_WRITE("Канал 2");
        if(!FindWave(B))
        {
            Display::ShowWarningBad(SignalNotFound);
        }
    }

    Init();
    Start();
    
    AUTO_FIND_IN_PROGRESS = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool FPGA::FindWave(Channel chan)
{
    Settings settings = set;    // Сохраняем предыдущие настройки

    Stop(false);
    SET_ENABLED(chan) = true;
    FPGA::SetTrigSource((TrigSource)chan);
    FPGA::SetTrigLev((TrigSource)chan, TrigLevZero);
    FPGA::SetRShift(chan, RShiftZero);
    FPGA::SetModeCouple(chan, ModeCouple_AC);
    Range range = AccurateFindRange(chan);
    //LOG_WRITE("Range %s", RangeName(range));
    if(range != RangeSize)
    {
        SET_RANGE(chan) = range;
        TBase tBase = AccurateFindTBase(chan);
        if (tBase != TBaseSize)
        {
            SET_TBASE = tBase;
            TRIG_SOURCE = (TrigSource)chan;
            return true;
        }
    }

    set = settings;
    FPGA::LoadSettings();
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Range FPGA::AccurateFindRange(Channel chan)
{
    /*
    Алгоритм поиска.
    Устанавливаем развёртку 50 мс.
    Устанавливаем закрытый вход.
    Включаем пиковый детектор.
    1. Устанавливаем растяжку 20В/дел.
    Не дожидаясь СИ, начинаем считывать точки в поточечном режиме и считываем их 50 штук - 2 клетки с небольшим. За это время
        на экране уложится чуть более одного периода частоты 10 Гц.
    Проверяем считанную информацию на выход за пределы экрана. Если вышла - берём предыдущую растяжку (например, 2В вместо 1В) и возвращаем
        её в качестве результата.
    Если не вышла - берём следующую растяжку и повторяем шаги с 1.
    */

    uint8 buffer[100];  // Сюда будем считывать точки

    SetTBase(TBase_50ms);
    FPGA::SetModeCouple(chan, ModeCouple_AC);
    PeackDetMode peackDetMode = PEAKDET;
    FPGA::SetPeackDetMode(PeackDet_Enable);
    for (int range = RangeSize - 1; range >= 0; range--)
    {
        //Timer::LogPointMS("1");
        FPGA::Stop(false);
        FPGA::SetRange(chan, (Range)range);
        Timer::PauseOnTime(10);
        FPGA::Start();

        for (int i = 0; i < 50; i++)
        {
            while (_GET_BIT(FSMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
            FSMC::Read(RD_ADC_B2);
            FSMC::Read(RD_ADC_B1);
            FSMC::Read(RD_ADC_A2);
            FSMC::Read(RD_ADC_A1);
        }

        if (chan == A)
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(FSMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
                FSMC::Read(RD_ADC_B2);
                FSMC::Read(RD_ADC_B1);
                buffer[i] = FSMC::Read(RD_ADC_A2);
                buffer[i + 1] = FSMC::Read(RD_ADC_A1);
            }
        }
        else
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(FSMC::Read(RD_FL), BIT_POINT_READY) == 0) {};
                buffer[i] = FSMC::Read(RD_ADC_B2);
                buffer[i + 1] = FSMC::Read(RD_ADC_B1);
                FSMC::Read(RD_ADC_A2);
                FSMC::Read(RD_ADC_A1);
            }
        }

        /*
        if(chan == A)
        {
            LOG_WRITE("min = %d, max = %d", CalculateMinWithout0(buffer), CalculateMaxWithout255(buffer));
        }
        */

        if (CalculateMinWithout0(buffer) < MIN_VALUE || CalculateMaxWithout255(buffer) > MAX_VALUE)
        {
            if (range < Range_20V)
            {
                range++;
            }
            FPGA::SetPeackDetMode(peackDetMode);
            return (Range)range;
        }

        uint8 min = AVE_VALUE - 30;
        uint8 max = AVE_VALUE + 30;

        if(range == Range_2mV && CalculateMinWithout0(buffer) > min && CalculateMaxWithout255(buffer) < max)
        {
            FPGA::SetPeackDetMode(peackDetMode);
            return RangeSize;
        }
    }
    FPGA::SetPeackDetMode(peackDetMode);
    return RangeSize;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
TBase FPGA::AccurateFindTBase(Channel chan)
{
    for (int i = 0; i < 5; i++)
    {
        TBase tBase = FindTBase(chan);
        TBase secondTBase = FindTBase(chan);

        if (tBase == secondTBase && tBase != TBaseSize)
        {
            return tBase;
        }
    }
    return TBaseSize;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
TBase FPGA::FindTBase(Channel chan)
{
    SetTrigInput(TrigInput_Full);
    Timer::PauseOnTime(10);
    FPGA::Stop(false);
    float fr = CalculateFreqFromCounterFreq();

    FPGA::SetTrigInput(fr < 1e6f ? TrigInput_LPF : TrigInput_Full);

    fr = CalculateFreqFromCounterFreq();

    TBase tBase = TBaseSize;

    if (fr >= 50.0f)
    {
        tBase = CalculateTBase(fr);
        FPGA::SetTBase(tBase);
        FPGA::Start();
        FPGA::SetTrigInput(fr < 500e3 ? TrigInput_LPF : TrigInput_HPF);
        return tBase;
    }
    else
    {
        FPGA::SetTrigInput(TrigInput_LPF);
        freq = CalculateFreqFromCounterPeriod();
        if (fr > 0.0f)
        {
            tBase = CalculateTBase(fr);
            FPGA::SetTBase(tBase);
            Timer::PauseOnTime(10);
            FPGA::Start();
            return tBase;
        }
    }

    return TBaseSize;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void StopTemporaryPause(void)
{
    FPGA_TEMPORARY_PAUSE = 0;
    Timer::Disable(kTemporaryPauseFPGA);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::TemporaryPause(void)
{
    FPGA_TEMPORARY_PAUSE = 1;
    Timer::Enable(kTemporaryPauseFPGA, 500, StopTemporaryPause);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::FillDataPointer(DataSettings *dp)
{
    dp->enableCh0 = sChannel_Enabled(A) ? 1U : 0U;
    dp->enableCh1 = sChannel_Enabled(B) ? 1U : 0U;
    dp->inverseCh0 = SET_INVERSE_A ? 1U : 0U;
    dp->inverseCh1 = SET_INVERSE_B ? 1U : 0U;
    dp->range[0] = SET_RANGE_A;
    dp->range[1] = SET_RANGE_B;
    dp->rShiftCh0 = (uint)SET_RSHIFT_A;
    dp->rShiftCh1 = (uint)SET_RSHIFT_B;
    dp->tBase = SET_TBASE;
    dp->tShift = TSHIFT;
    dp->modeCouple0 = SET_COUPLE_A;
    dp->modeCouple1 = SET_COUPLE_B;
    dp->length1channel = (uint)sMemory_GetNumPoints(false);
    dp->trigLevCh0 = (uint)TRIG_LEVEL_A;
    dp->trigLevCh1 = (uint)TRIG_LEVEL_B;
    dp->peakDet = (uint)PEAKDET;
    dp->multiplier0 = SET_DIVIDER_A;
    dp->multiplier1 = SET_DIVIDER_B;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::FindAndSetTrigLevel(void)
{
    TrigSource trigSource = TRIG_SOURCE;
    if (Storage::AllDatas() == 0 || TRIG_SOURCE_IS_EXT)
    {
        return;
    }

    Channel chanTrig = (Channel)trigSource;
    uint8 *data0 = 0;
    uint8 *data1 = 0;
    DataSettings *ds_ = 0;

    Storage::GetDataFromEnd(0, &ds_, &data0, &data1);

    const uint8 *data = (chanTrig == A) ? data0 : data1;

    int lastPoint = (int)ds_->length1channel - 1;

    uint8 min = Math_GetMinFromArray(data, 0, lastPoint);
    uint8 max = Math_GetMaxFromArray(data, 0, lastPoint);

    uint8 aveValue = (uint8)(((int)min + (int)max) / 2);

    static const float scale = (float)(TrigLevMax - TrigLevZero) / (float)(MAX_VALUE - AVE_VALUE) / 2.4f;

    int16 trigLev = TrigLevZero + scale * ((int)aveValue - AVE_VALUE) - (SET_RSHIFT(chanTrig) - RShiftZero);

    FPGA::SetTrigLev(trigSource, trigLev);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
#define pinCLC      GPIO_PIN_2
#define pinData     GPIO_PIN_3
#define CHIP_SELECT_IN_LOW  HAL_GPIO_WritePin(GPIOG, pinSelect, GPIO_PIN_RESET);
#define CHIP_SELECT_IN_HI   HAL_GPIO_WritePin(GPIOG, pinSelect, GPIO_PIN_SET);
#define CLC_HI              HAL_GPIO_WritePin(GPIOG, pinCLC, GPIO_PIN_SET);
#define CLC_LOW             HAL_GPIO_WritePin(GPIOG, pinCLC, GPIO_PIN_RESET);
#define DATA_SET(x)         HAL_GPIO_WritePin(GPIOG, pinData, x);

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::WriteToAnalog(TypeWriteAnalog type, uint data)
{
#define pinSelect   GPIO_PIN_5

    char buffer[19];
    char *str = Bin2String16((uint16)data, buffer);
    if (type == TypeWriteAnalog_Range0 && IS_SHOW_REG_RANGE_A)
    {
        LOG_WRITE("range 0 = %s", str);
    }
    else if (type == TypeWriteAnalog_Range1 && IS_SHOW_REG_RANGE_B)
    {
        LOG_WRITE("range 1 = %s", str);
    }
    else if (type == TypeWriteAnalog_TrigParam && IS_SHOW_REG_TRIGPARAM)
    {
        LOG_WRITE("парам. синхр. = %s", str);
    }
    else if (type == TypeWriteAnalog_ChanParam0 && IS_SHOW_REG_PARAM_A)
    {
        LOG_WRITE("парам. кан. 1 = %s", str);
    }
    else if (type == TypeWriteAnalog_ChanParam1 && IS_SHOW_REG_PARAM_B)
    {
        LOG_WRITE("парам. кан. 2 = %s", str);
    }
    else if (type == TypeWriteAnalog_All && (IS_SHOW_REG_TRIGPARAM || IS_SHOW_REG_RANGE_A || IS_SHOW_REG_RANGE_B || IS_SHOW_REG_PARAM_A || IS_SHOW_REG_PARAM_B))
    {
        LOG_WRITE("полная запись в аналоговую часть = %s", str);
    }

    CHIP_SELECT_IN_LOW
        for (int i = 23; i >= 0; i--)
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
                CLC_LOW
        }
    CHIP_SELECT_IN_HI;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::WriteToDAC(TypeWriteDAC type, uint16 data)
{
#undef pinSelect
#define pinSelect   GPIO_PIN_7
    char buffer[19];
    if (type == TypeWriteDAC_RShiftA && IS_SHOW_REG_RSHIFT_A)
    {
        LOG_WRITE("rShift 0 = %s", Bin2String16(data, buffer));
    }
    else if (type == TypeWriteDAC_RShiftB && IS_SHOW_REG_RSHIFT_B)
    {
        LOG_WRITE("rShfit 1 = %s", Bin2String16(data, buffer));
    }
    else if (type == TypeWriteDAC_TrigLev && IS_SHOW_REG_TRIGLEV)
    {
        LOG_WRITE("trigLev = %s", Bin2String16(data, buffer));
    }

    CHIP_SELECT_IN_LOW
        for (int i = 15; i >= 0; i--)
        {
            DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            CLC_HI
                CLC_LOW
        }
    CHIP_SELECT_IN_HI;
}
