#include <stm32f2xx_hal.h>

void InitHardware() 
{

    GPIO_InitTypeDef isGPIO_ =
    {
        GPIO_PIN_11,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH,
        GPIO_AF0_MCO,
    };
    // Сигнал готовности дисплея  к приёму команды
    HAL_GPIO_Init(GPIOG, &isGPIO_);
}
