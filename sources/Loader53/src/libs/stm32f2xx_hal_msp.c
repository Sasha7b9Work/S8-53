#include "Hardware/Hardware.h"
#include <stm32f2xx_hal.h>
#include <stm32f2xx_hal_sram.h>


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{

}


void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    __FSMC_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef isGPIOD = {
        GPIO_PIN_0 |        // DA2
        GPIO_PIN_1 |        // DA3
        GPIO_PIN_4 |        // RD
        GPIO_PIN_5 |        // WR
        GPIO_PIN_7 |        // NE1
        GPIO_PIN_11 |       // A16
        GPIO_PIN_12 |       // A17
        GPIO_PIN_13 |       // A18
        GPIO_PIN_14 |       // DA0
        GPIO_PIN_15,        // DA1
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FSMC
    };
    HAL_GPIO_Init(GPIOD, &isGPIOD);

    GPIO_InitTypeDef isGPIOE_B = {
        GPIO_PIN_2 |        // A23
        GPIO_PIN_3 |        // A19
        GPIO_PIN_4 |        // A20
        GPIO_PIN_5 |        // A21
        GPIO_PIN_6 |        // A22
        GPIO_PIN_7 |        // DA4
        GPIO_PIN_8 |        // DA5
        GPIO_PIN_9 |        // DA6
        GPIO_PIN_10,        // DA7
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FAST,
        GPIO_AF12_FSMC
    };
    HAL_GPIO_Init(GPIOE, &isGPIOE_B);

    isGPIOE_B.Pin = GPIO_PIN_7;     // NL1
    HAL_GPIO_Init(GPIOB, &isGPIOE_B);

}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_11 ||
                    GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
    __GPIOB_CLK_ENABLE();
    __USB_OTG_HS_CLK_ENABLE();
    __SYSCFG_CLK_ENABLE();

    GPIO_InitTypeDef isGPIO;
    isGPIO.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    isGPIO.Mode = GPIO_MODE_AF_PP;
    isGPIO.Pull = GPIO_NOPULL;
    isGPIO.Speed = GPIO_SPEED_FAST;
    isGPIO.Alternate = GPIO_AF12_OTG_HS_FS;
    HAL_GPIO_Init(GPIOB, &isGPIO);

    /* Set USBHS Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 15, 0);

    /* Enable USBHS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
    __USB_OTG_HS_CLK_DISABLE();
}
