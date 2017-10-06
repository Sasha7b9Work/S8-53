#include "defines.h"
#include "Log.h"


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void ADC_IRQHandler(void)
{
}

// This interrupt call soft NSS for spi (see Hardware::SPIforPanel.c::
// PanelInit() and HAL_GPIO_EXTI_Callback())
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

#ifdef SPI_ENABLE
// See Hardware::SPIforPanel.c::HAL_SPI_RxCpltCallback()
void SPI1_IRQHandler(void)
{
}
#endif

#ifdef OTG_FS_USBH_ENBALE                   // FlashDrive for TE207
void OTG_FS_IRQHandler(void)
{
    HAL_HCD_IRQHandler(gHandleHCD);
}
#endif

void OTG_HS_IRQHandler(void)
{
}

void DMA1_Stream5_IRQHandler()
{
}
