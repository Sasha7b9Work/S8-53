

#include <stm32f2xx_hal.h>
#include <stm32f2xx_hal_hcd.h>
#include <usbh_core.h>
#include <stm32f207xx.h>

/*
    Функции инициализации и деинициализации аппаратной части USBH, используемого для работы с флеш-диском.
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd) {

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
