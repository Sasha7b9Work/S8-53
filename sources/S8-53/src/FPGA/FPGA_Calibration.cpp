#include "FPGA.h" 
#include "FPGA_Types.h"
#include "Display/Display.h"
#include "Display/DisplayPrimitives.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Panel/Panel.h"
#include "Hardware/FSMC.h"
#include "Settings/SettingsTypes.h"
#include "Settings/Settings.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include <stdio.h>
#include <limits.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int16    CalculateAdditionRShift(Channel chan, Range range);	///< Измерить добавочное смещение канала по напряжению.
static float    CalculateKoeffCalibration(Channel chan);			///< Измерить коэффициент калибровки канала по напряжению.
static void     AlignmentADC();
static void     FuncAttScreen();								///< Функция обновления экрана в режиме калибровки.
static float    CalculateDeltaADC(Channel chan, float *avgADC1, float *avgADC2, float *delta);
static void     DrawParametersChannel(Channel chan, int x, int y, bool inProgress);

static float deltaADC[2] = {0.0f, 0.0f};
static float deltaADCPercents[2] = {0.0f, 0.0f};
static float avrADC1[2] = {0.0f, 0.0f};
static float avrADC2[2] = {0.0f, 0.0f};

static float deltaADCold[2] = {0.0f, 0.0f};
static float deltaADCPercentsOld[2] = {0.0f, 0.0f};
static float avrADC1old[2] = {0.0f, 0.0f};
static float avrADC2old[2] = {0.0f, 0.0f};

static int8 shiftADC0 = 0;
static int8 shiftADC1 = 0;

static float koeffCal0 = -1.0f;
static float koeffCal1 = -1.0f;

static ProgressBar bar0;                            // Прогресс-бар для калибровки первого канала.
static ProgressBar bar1;                            // Прогресс-бар для калибровки второго канала.

static uint startTimeChan0 = 0;                     // Время начала калибровки первого канала.
static uint startTimeChan1 = 0;                     // Время начала калибровки второго канала.

static float koeffCalibrationOld[2];

extern void LoadStretchADC(Channel chan);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnTimerDraw(void)
{
    Display::Update();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::ProcedureCalibration(void)
{
    bool chanAenable = SET_ENABLED_A;
    bool chanBenable = SET_ENABLED_B;

    SET_ENABLED_A = SET_ENABLED_B = true;

    Display::SetDrawMode(DrawMode_Hand, FuncAttScreen);
    Timer::Enable(kTimerDrawHandFunction, 100, OnTimerDraw);

    koeffCalibrationOld[A] = STRETCH_ADC_A;
    koeffCalibrationOld[B] = STRETCH_ADC_B;

    bar0.fullTime = bar0.passedTime = bar1.fullTime = bar1.passedTime = 0;

    FPGA::SaveState();                               // Сохраняем текущее состояние.
    Panel::Disable();                                // Отлкючаем панель управления.

    while(1)
    {
        gStateFPGA.stateCalibration = StateCalibration_ADCinProgress;                  // Запускаем процедуру балансировки АЦП.

        FPGA::SetTBase(TBase_500us);
        FPGA::SetTShift(0);
        STRETCH_ADC_A = 1.0f;
        STRETCH_ADC_B = 1.0f;
        FPGA::LoadKoeffCalibration(A);
        FPGA::LoadKoeffCalibration(B);
        FPGA::SetRange(A, Range_500mV);
        FPGA::SetRange(B, Range_500mV);
        FPGA::SetRShift(A, RShiftZero);
        FPGA::SetRShift(B, RShiftZero);
        FPGA::SetModeCouple(A, ModeCouple_GND);
        FPGA::SetModeCouple(B, ModeCouple_GND);
        FSMC::Write(WR_ADD_RSHIFT_DAC1, 0);
        FSMC::Write(WR_ADD_RSHIFT_DAC2, 0);

        deltaADCPercentsOld[0] = CalculateDeltaADC(A, &avrADC1old[A], &avrADC2old[A], &deltaADCold[A]);
        deltaADCPercentsOld[1] = CalculateDeltaADC(B, &avrADC1old[B], &avrADC2old[B], &deltaADCold[B]);

        AlignmentADC();

        deltaADCPercents[A] = CalculateDeltaADC(A, &avrADC1[A], &avrADC2[A], &deltaADC[A]);
        deltaADCPercents[B] = CalculateDeltaADC(B, &avrADC1[B], &avrADC2[B], &deltaADC[B]);

        gStateFPGA.stateCalibration = StateCalibration_RShift0start;                 

        koeffCal0 = koeffCal1 = ERROR_VALUE_FLOAT;

        if(Panel::WaitPressingButton() == B_Start)             // Ожидаем подтверждения или отмены процедуры калибровки первого канала.
        {
			gStateFPGA.stateCalibration = StateCalibration_RShift0inProgress;

			koeffCal0 = CalculateKoeffCalibration(A);
			if(koeffCal0 == ERROR_VALUE_FLOAT)
            {
				gStateFPGA.stateCalibration = StateCalibration_ErrorCalibration0;
				Panel::WaitPressingButton();
                DEBUG_STRETCH_ADC_TYPE = StretchADC_Hand;
                LoadStretchADC(A);
            }
            else
            {
                STRETCH_ADC_A = koeffCal0;
                FPGA::LoadKoeffCalibration(A);
            }
			
            for (int range = 0; range < RangeSize; range++)
            {
                for (int mode = 0; mode < 2; mode++)
                {
                    if (!(mode == 0 && (range == Range_2mV || range == Range_5mV || range == Range_10mV)))
                    {
                        FPGA::SetModeCouple(A, (ModeCouple)mode);
                        RSHIFT_ADD(A, range, mode) = 0;
                        RSHIFT_ADD(A, range, mode) = CalculateAdditionRShift(A, (Range)range);
                    }
                }
            }
		}

        gStateFPGA.stateCalibration = StateCalibration_RShift1start;

        HAL_Delay(500);

		if(Panel::WaitPressingButton() == B_Start)                 // Ожидаем подтверждения или отмены процедуры калибровки второго канала.
        {
			gStateFPGA.stateCalibration = StateCalibration_RShift1inProgress;

            koeffCal1 = CalculateKoeffCalibration(B);
			if(koeffCal1 == ERROR_VALUE_FLOAT)
            {
				gStateFPGA.stateCalibration = StateCalibration_ErrorCalibration1;
				Panel::WaitPressingButton();
                DEBUG_STRETCH_ADC_TYPE = StretchADC_Hand;
                LoadStretchADC(B);
			}
            else
            {
                STRETCH_ADC_B = koeffCal1;
                FPGA::LoadKoeffCalibration(B);
            }

            for (int range = 0; range < RangeSize; range++)
            {
                for (int mode = 0; mode < 2; mode++)
                {
                    if (!(mode == 0 && (range == Range_2mV || range == Range_5mV || range == Range_10mV)))
                    {
                        FPGA::SetModeCouple(B, (ModeCouple)mode);
                        RSHIFT_ADD(B, range, mode) = 0;
                        RSHIFT_ADD(B, range, mode) = CalculateAdditionRShift(B, (Range)range);
                    }
                }
            }
		}

        break;
    }

    FPGA::RestoreState();

    SET_BALANCE_ADC_A = shiftADC0;
    SET_BALANCE_ADC_B = shiftADC1;
    FSMC::Write(WR_ADD_RSHIFT_DAC1, (uint8)SET_BALANCE_ADC_A);
    FSMC::Write(WR_ADD_RSHIFT_DAC2, (uint8)SET_BALANCE_ADC_B);

    FPGA::SetRShift(A, SET_RSHIFT_A);
    FPGA::SetRShift(B, SET_RSHIFT_B);

    STRETCH_ADC_A = (koeffCal0 == ERROR_VALUE_FLOAT) ? koeffCalibrationOld[0] : koeffCal0;

    FPGA::LoadKoeffCalibration(A);

    STRETCH_ADC_B = (koeffCal1 == ERROR_VALUE_FLOAT) ? koeffCalibrationOld[1] : koeffCal1;
    FPGA::LoadKoeffCalibration(B);

    gStateFPGA.stateCalibration = StateCalibration_None;
    Panel::WaitPressingButton();
    Panel::Enable();
    Timer::Disable(kTimerDrawHandFunction);
    Display::SetDrawMode(DrawMode_Auto, 0);
    gStateFPGA.stateCalibration = StateCalibration_None;

    SET_ENABLED_A = chanAenable;
    SET_ENABLED_B = chanBenable;

    FPGA::OnPressStartStop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FuncAttScreen(void)
{
    Painter::BeginScene(COLOR_BLACK);

    static bool first = true;
    static uint startTime = 0;
    if(first)
    {
        first = false;
        startTime = gTimerMS;
    }
    int16 y = 10;
    Display::Clear();
    
    Painter::SetColor(COLOR_FILL);
    
#define dX 20
#define dY -15
    
    switch(gStateFPGA.stateCalibration)
    {
        case StateCalibration_None:
        {
                Painter::DrawTextInRect(40 + dX, y + 25 + dY, SCREEN_WIDTH - 100, 200, "Калибровка завершена. Нажмите любую кнопку, чтобы выйти из режима калибровки.");

                Painter::DrawText(10 + dX, 55 + dY, "Поправка нуля 1к :");
                Painter::DrawText(10 + dX, 80 + dY, "Поправка нуля 2к :");
                for (int i = 0; i < RangeSize; i++)
                {
                    Painter::DrawFormatText(95 + i * 16 + dX, 55 + dY, COLOR_FILL, "%d", RSHIFT_ADD(A, i, 0));
                    Painter::DrawFormatText(95 + i * 16 + dX, 65 + dY, COLOR_FILL, "%d", RSHIFT_ADD(A, i, 1));
                    Painter::DrawFormatText(95 + i * 16 + dX, 80 + dY, COLOR_FILL, "%d", RSHIFT_ADD(B, i, 0));
                    Painter::DrawFormatText(95 + i * 16 + dX, 90 + dY, COLOR_FILL, "%d", RSHIFT_ADD(B, i, 1));
                }
                
                Painter::DrawFormatText(10 + dX, 110 + dY, COLOR_FILL, "Коэффициент калибровки 1к : %f, %d", STRETCH_ADC_A, (int)(STRETCH_ADC_A * 0x80));
                Painter::DrawFormatText(10 + dX, 130 + dY, COLOR_FILL, "Коэфффициент калибровки 2к : %f, %d", STRETCH_ADC_B, (int)(STRETCH_ADC_B * 0x80));

                DrawParametersChannel(A, 10 + dX, 150 + dY, false);
                DrawParametersChannel(B, 10 + dX, 200 + dY, false);
        }
            break;

        case StateCalibration_ADCinProgress:
            DrawParametersChannel(A, 5, 25, true);
            DrawParametersChannel(B, 5, 75, true);
            break;

        case StateCalibration_RShift0start:
            Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "Подключите ко входу канала 1 выход калибратора и нажмите кнопку ПУСК/СТОП. \
Если вы не хотите калибровать первый канала, нажмите любую другую кнопку.");
            break;

        case StateCalibration_RShift0inProgress:
            break;

        case StateCalibration_RShift1start:
            Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "Подключите ко входу канала 2 выход калибратора и нажмите кнопку ПУСК/СТОП. \
Если вы не хотите калибровать второй канал, нажмите любую другую кнопку.");
            break;

        case StateCalibration_RShift1inProgress:
            break;

        case StateCalibration_ErrorCalibration0:
            Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "Внимание !!! Канал 1 не скалиброван.");
            break;

        case StateCalibration_ErrorCalibration1:
            Painter::DrawTextInRect(50, y + 25, SCREEN_WIDTH - 100, 200, "Внимание !!! Канал 2 не скалиброван.");
            break;
    }

    /*
    if(stateFPGA.stateCalibration == kNone || stateFPGA.stateCalibration == kRShift0start || stateFPGA.stateCalibration == kRShift1start) {
        x = 230;
        y = 187;
        int delta = 32;
        width = 80;
        height = 25;
        DrawRectangle(x, y, width, height, COLOR_BLACK);
        DrawStringInCenterRect(x, y, width, height, "ПРОДОЛЖИТЬ", COLOR_BLACK, false);
        DrawRectangle(x, y - delta, width, height, COLOR_BLACK);
        DrawStringInCenterRect(x, y - delta, width, height, "ОТМЕНИТЬ", COLOR_BLACK, false);
    }
    */
    char buffer[100];
    sprintf(buffer, "%.1f", (gTimerMS - startTime) / 1000.0f);
    Painter::DrawTextC(0, 0, buffer, COLOR_BLACK);

    Painter::EndScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void DrawParametersChannel(Channel chan, int eX, int eY, bool inProgress)
{
    Painter::SetColor(COLOR_FILL);
    if(inProgress)
    {
        Painter::DrawText(eX, eY + 4, chan == 0 ? "КАНАЛ 1" : "КАНАЛ 2");
        ProgressBar *bar = (chan == A) ? &bar0 : &bar1;
        bar->width = 240;
        bar->height = 15;
        bar->y = eY;
        bar->x = 60;
        ProgressBar_Draw(bar);
    }

    if(Settings::DebugModeEnable())
    {
        int x = inProgress ? 5 : eX;
        int y = eY + (inProgress ? 110 : 0);
        Painter::DrawText(x, y, "Отклонение от нуля:");
        char buffer[100] = {0};
        sprintf(buffer, "АЦП1 = %.2f/%.2f, АЦП2 = %.2f/%.2f, d = %.2f/%.2f", avrADC1old[chan] - AVE_VALUE, avrADC1[chan] - AVE_VALUE, 
                                                                             avrADC2old[chan] - AVE_VALUE, avrADC2[chan] - AVE_VALUE,
                                                                             deltaADCold[chan], deltaADC[chan]);
        y += 10;
        Painter::DrawText(x, y, buffer);
        buffer[0] = 0;
        sprintf(buffer, "Расхождение AЦП = %.2f/%.2f %%", deltaADCPercentsOld[chan], deltaADCPercents[chan]);
        Painter::DrawText(x, y + 11, buffer);
        buffer[0] = 0;
        sprintf(buffer, "Записано %d", SET_BALANCE_ADC(chan));
        Painter::DrawText(x, y + 19, buffer);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateDeltaADC(Channel chan, float *avgADC1, float *avgADC2, float *delta)
{
    uint *startTime = (chan == A) ? &startTimeChan0 : &startTimeChan1;
    *startTime = gTimerMS;
    
    ProgressBar *bar = (chan == A) ? &bar0 : &bar1;
    bar->passedTime = 0;
    bar->fullTime = 0;

    FPGA::SetTrigSource((TrigSource)chan);
    FPGA::SetTrigLev((TrigSource)chan, TrigLevZero);

    uint8 *address1 = chan == A ? RD_ADC_A1 : RD_ADC_B1;
    uint8 *address2 = chan == A ? RD_ADC_A2 : RD_ADC_B2;

    static const int numCicles = 10;
    for(int cicle = 0; cicle < numCicles; cicle++)
    {
        FSMC::Write(WR_START, 1);
        while(_GET_BIT(FSMC::Read(RD_FL), 2) == 0) {};
        FPGA::SwitchingTrig();
        while(_GET_BIT(FSMC::Read(RD_FL), 0) == 0) {};
        FSMC::Write(WR_STOP, 1);

        for(int i = 0; i < FPGA_MAX_POINTS; i++)
        {
            if(chan == A)
            {
                *avgADC1 += FSMC::Read(address1);
                *avgADC2 += FSMC::Read(address2);
                FSMC::Read(RD_ADC_B1);
                FSMC::Read(RD_ADC_B2);
            }
            else
            {
                FSMC::Read(RD_ADC_A1);
                FSMC::Read(RD_ADC_A2);
                *avgADC1 += FSMC::Read(address1);
                *avgADC2 += FSMC::Read(address2);
            }
        }
        
        bar->passedTime = gTimerMS - *startTime;
        bar->fullTime = bar->passedTime * (float)numCicles / (cicle + 1);
    }

    *avgADC1 /= (FPGA_MAX_POINTS * numCicles);
    *avgADC2 /= (FPGA_MAX_POINTS * numCicles);

    *delta = *avgADC1 - *avgADC2;

    return ((*avgADC1) - (*avgADC2)) / 255.0f * 100;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void AlignmentADC(void)
{
    shiftADC0 = (deltaADCold[0] > 0) ? (deltaADCold[0] + 0.5) : (deltaADCold[0] - 0.5);
    SET_BALANCE_ADC_A = shiftADC0;
    shiftADC1 = (deltaADCold[1] > 0) ? (deltaADCold[1] + 0.5) : (deltaADCold[1] - 0.5);
    SET_BALANCE_ADC_B = shiftADC1;
    FSMC::Write(WR_ADD_RSHIFT_DAC1, (uint8)SET_BALANCE_ADC_A);
    FSMC::Write(WR_ADD_RSHIFT_DAC2, (uint8)SET_BALANCE_ADC_B);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
int16 CalculateAdditionRShift(Channel chan, Range range)
{
    FPGA::SetRange(chan, range);
    FPGA::SetRShift(chan, RShiftZero);
    FPGA::SetTBase(TBase_200us);
    FPGA::SetTrigSource(chan == A ? TrigSource_ChannelA : TrigSource_ChannelB);
    FPGA::SetTrigPolarity(TrigPolarity_Front);
    FPGA::SetTrigLev((TrigSource)chan, TrigLevZero);

    FPGA::WriteToHardware(WR_UPR, BINARY_U8(00000000), false);   // Устанавливаем выход калибратора в ноль

    int numMeasures = 8;
    int sum = 0;
    int numPoints = 0;

    uint time = gTimerMS;

    while(gTimerMS - time < 50) {};

    for(int i = 0; i < numMeasures; i++)
    {
        uint startTime = gTimerMS;
        const uint timeWait = 100;

        FSMC::Write(WR_START, 1);
        while(_GET_BIT(FSMC::Read(RD_FL), 2) == 0 && (gTimerMS - startTime < timeWait)) {}; 
        if(gTimerMS - startTime > timeWait)                 // Если прошло слишком много времени - 
        {
            return ERROR_VALUE_INT16;                       // выход с ошибкой.
        }

        FPGA::SwitchingTrig();

        startTime = gTimerMS;

        while(_GET_BIT(FSMC::Read(RD_FL), 0) == 0 && (gTimerMS - startTime < timeWait)) {};
        if(gTimerMS - startTime > timeWait)                 // Если прошло слишком много времени - 
        {
            return ERROR_VALUE_INT16;                       // выход с ошибкой.
        }

        FSMC::Write(WR_STOP, 1);

        uint8 *addressRead1 = chan == A ? RD_ADC_A1 : RD_ADC_B1;
        uint8 *addressRead2 = chan == A ? RD_ADC_A2 : RD_ADC_B2;

        for(int j = 0; j < FPGA_MAX_POINTS; j += 2)
        {
            sum += FSMC::Read(addressRead1);
            sum += FSMC::Read(addressRead2);
            numPoints += 2;
        }
    }

    float aveValue = (float)sum / numPoints;
    int16 retValue = -(aveValue - AVE_VALUE) * 2;

    if(retValue < - 100 || retValue > 100)
    {
        return ERROR_VALUE_INT16;
    }
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float CalculateKoeffCalibration(Channel chan)
{
    FPGA::WriteToHardware(WR_UPR, BINARY_U8(00000100), false);

    FPGA::SetRShift(chan, RShiftZero - 40 * 4);
    FPGA::SetModeCouple(chan, ModeCouple_DC);
    FPGA::SetTrigSource((TrigSource)chan);
    FPGA::SetTrigLev((TrigSource)chan, TrigLevZero + 40 * 4);
    
    int numMeasures = 16;
    int sumMIN = 0;
    int numMIN = 0;
    int sumMAX = 0;
    int numMAX = 0;

    for(int i = 0; i < numMeasures; i++)
    {
        uint startTime = gTimerMS;
        const uint timeWait = 1000;

        while(gTimerMS - startTime < 20) {}
        startTime = gTimerMS;

        FSMC::Write(WR_START, 1);
        while(_GET_BIT(FSMC::Read(RD_FL), 2) == 0 && (gTimerMS - startTime > timeWait)) {};
        if(gTimerMS - startTime > timeWait)
        {
            return ERROR_VALUE_FLOAT;
        }

        FPGA::SwitchingTrig();
        startTime = gTimerMS;

        while(_GET_BIT(FSMC::Read(RD_FL), 0) == 0 && (gTimerMS - startTime > timeWait)) {};
        if(gTimerMS - startTime > timeWait)
        {
            return ERROR_VALUE_FLOAT;
        }

        FSMC::Write(WR_STOP, 1);

        uint8 *addressRead1 = chan == A ? RD_ADC_A1 : RD_ADC_B1;
        uint8 *addressRead2 = chan == A ? RD_ADC_A2 : RD_ADC_B2;

        for(int j = 0; j < FPGA_MAX_POINTS; j += 2)
        {
            uint8 val0 = FSMC::Read(addressRead1);
            if(val0 > AVE_VALUE + 60)
            {
                numMAX++;
                sumMAX += val0;
            }
            else if(val0 < AVE_VALUE - 60)
            {
                numMIN++;
                sumMIN += val0;
            }

            uint8 val1 = FSMC::Read(addressRead2);
            if(val1 > AVE_VALUE + 60)
            {
                numMAX++;
                sumMAX += val1;
            }
            else if(val1 < AVE_VALUE - 60)
            {
                numMIN++;
                sumMIN += val1;
            }
        }
    }

    float aveMin = (float)sumMIN / (float)numMIN;
    float aveMax = (float)sumMAX / (float)numMAX;

    float retValue = 160.0f / (aveMax - aveMin);

    if(retValue < 0.5f || retValue > 1.5f)
    {
        return ERROR_VALUE_FLOAT;
    }
    return retValue * 1.004f;
}
