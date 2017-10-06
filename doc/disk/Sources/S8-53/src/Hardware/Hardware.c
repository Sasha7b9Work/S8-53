// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stm32f207xx.h>
#include <stm32f2xx_hal.h>
#include <stm32f2xx_hal_tim.h>
#include <stm32f2xx_hal_gpio.h>
#include <stm32f2xx_hal_rcc.h>
#include <stm32f2xx_hal_dac.h>
#include "Hardware.h"
#include "ADC.h"
#include "FLASH.h"
#include "Sound.h"
#include "Hardware/RTC.h"
#include "FSMC.h"
#include "Utils/GlobalFunctions.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Ethernet/Ethernet.h"
#include "Display/Display.h"
#include "FlashDrive/FlashDrive.h"
#include "Panel/Panel.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static TIM_HandleTypeDef handleTIM6forTimer =
{
    TIM6,
    {
        119,                    // Init.Prescaler
        TIM_COUNTERMODE_UP,     // Init.CounterMode
        500,                    // Init.Period
        TIM_CLOCKDIVISION_DIV1  // Init.ClockDivision
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SystemClock_Config(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Hardware_Init(void)
{
    HAL_Init();

    SystemClock_Config();

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __DMA1_CLK_ENABLE();        // Для DAC1 (бикалка)
    
    __TIM6_CLK_ENABLE();        // Для отсчёта миллисекунд
    __TIM2_CLK_ENABLE();        // Для тиков
    __TIM7_CLK_ENABLE();        // Для DAC1 (бикалка)
    __DAC_CLK_ENABLE();         // Для бикалки
    __PWR_CLK_ENABLE();

    __SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    // Timer  /////////////////////////////////////////////////////////////////
    //RCC_PCLK1Config(RCC_HCLK_Div1);

    // Таймер для мс
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    if (HAL_TIM_Base_Init(&handleTIM6forTimer) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    if (HAL_TIM_Base_Start_IT(&handleTIM6forTimer) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    // Таймер для тиков
    TIM_HandleTypeDef tim2handle =
    {
        TIM2,
        {
            0,
            TIM_COUNTERMODE_UP,
            0xffffffff,
            TIM_CLOCKDIVISION_DIV1
        }
    };

    if (HAL_TIM_Base_Init(&tim2handle) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    if (HAL_TIM_Base_Start(&tim2handle) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    Sound_Init();
    
    Panel_Init();

    FSMC_Init();

    ADC_Init();

    FDrive_Init();
    
// Analog and DAC programmable SPI ////////////////////////////////////////

    GPIO_InitTypeDef isGPIOG =
    {
        GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7,     // GPIO_PIN_1 - для работы с дисплеем
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF0_MCO
    };
    HAL_GPIO_Init(GPIOG, &isGPIOG);

    isGPIOG.Pin = GPIO_PIN_1;
    isGPIOG.Mode = GPIO_MODE_OUTPUT_PP;
    isGPIOG.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOG, &isGPIOG);
    
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);                   // PG1 - когда равен 1, чтение дисплея, в остальных случаях 0

    RTC_Init();
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
void Hardware_WriteToAnalog(TypeWriteAnalog type, uint data)
{
#define pinSelect   GPIO_PIN_5

    char buffer[19];
    char *str = Bin2String16(data, buffer);
    if(type == TypeWriteAnalog_Range0 && IS_SHOW_REG_RANGE_A)
    {
        LOG_WRITE("range 0 = %s", str);
    }
    else if(type == TypeWriteAnalog_Range1 && IS_SHOW_REG_RANGE_B)
    {
        LOG_WRITE("range 1 = %s", str);
    }
    else if(type == TypeWriteAnalog_TrigParam && IS_SHOW_REG_TRIGPARAM)
    {
        LOG_WRITE("парам. синхр. = %s", str);
    }
    else if(type == TypeWriteAnalog_ChanParam0 && IS_SHOW_REG_PARAM_A)
    {
        LOG_WRITE("парам. кан. 1 = %s", str);
    }
    else if(type == TypeWriteAnalog_ChanParam1 && IS_SHOW_REG_PARAM_B)
    {
        LOG_WRITE("парам. кан. 2 = %s", str);
    }
    else if(type == TypeWriteAnalog_All && (IS_SHOW_REG_TRIGPARAM || IS_SHOW_REG_RANGE_A || IS_SHOW_REG_RANGE_B || IS_SHOW_REG_PARAM_A || IS_SHOW_REG_PARAM_B))
    {
        LOG_WRITE("полная запись в аналоговую часть = %s", str);
    }

    CHIP_SELECT_IN_LOW
    for(int i = 23; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        CLC_HI
        CLC_LOW
    }
    CHIP_SELECT_IN_HI;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Hardware_WriteToDAC(TypeWriteDAC type, uint16 data)
{
#undef pinSelect
#define pinSelect   GPIO_PIN_7
    char buffer[19];
    if(type == TypeWriteDAC_RShift0 && IS_SHOW_REG_RSHIFT_A)
    {
        LOG_WRITE("rShift 0 = %s", Bin2String16(data, buffer));
    }
    else if(type == TypeWriteDAC_RShift1 && IS_SHOW_REG_RSHIFT_B)
    {
        LOG_WRITE("rShfit 1 = %s", Bin2String16(data, buffer));
    }
    else if(type == TypeWriteDAC_TrigLev && IS_SHOW_REG_TRIGLEV)
    {
        LOG_WRITE("trigLev = %s", Bin2String16(data, buffer));
    }

    CHIP_SELECT_IN_LOW
    for(int i = 15; i >= 0; i--)
    {
        DATA_SET((data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        CLC_HI
        CLC_LOW
    }
    CHIP_SELECT_IN_HI;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void TIM6_DAC_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&handleTIM6forTimer, TIM_FLAG_UPDATE) == SET && __HAL_TIM_GET_ITSTATUS(&handleTIM6forTimer, TIM_IT_UPDATE))
    {
        Timer_Update1ms();
        __HAL_TIM_CLEAR_FLAG(&handleTIM6forTimer, TIM_FLAG_UPDATE);
        __HAL_TIM_CLEAR_IT(&handleTIM6forTimer, TIM_IT_UPDATE);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        // Initialization Error
        HARDWARE_ERROR
    }
    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        // Initialization Error
        HARDWARE_ERROR
    }
}
