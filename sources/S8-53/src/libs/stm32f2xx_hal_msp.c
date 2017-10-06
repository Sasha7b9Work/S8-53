

#include "Hardware/Hardware.h"
#include <stm32f2xx_hal.h>
#include <stm32f2xx_hal_sram.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef oscIS;
    RCC_PeriphCLKInitTypeDef periphClkIS;

    __PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    oscIS.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    oscIS.PLL.PLLState = RCC_PLL_NONE;
    oscIS.LSEState = RCC_LSE_ON;
    oscIS.LSIState = RCC_LSI_OFF;

    if (HAL_RCC_OscConfig(&oscIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    periphClkIS.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periphClkIS.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&periphClkIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }
    __HAL_RCC_RTC_ENABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    __HAL_RCC_RTC_DISABLE();
    HAL_PWR_DisableBkUpAccess();
    __PWR_CLK_DISABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_11 ||
                    GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
    GPIO_InitTypeDef structGPIO =
    {
        GPIO_PIN_4,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOA, &structGPIO);

    static DMA_HandleTypeDef hdmaDAC1 =
    {
        DMA1_Stream5,
        {
            DMA_CHANNEL_7,
            DMA_MEMORY_TO_PERIPH,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_CIRCULAR,
            DMA_PRIORITY_HIGH,
            DMA_FIFOMODE_DISABLE,
            DMA_FIFO_THRESHOLD_HALFFULL,
            DMA_MBURST_SINGLE,
            DMA_PBURST_SINGLE
        }
    };

    HAL_DMA_Init(&hdmaDAC1);

    __HAL_LINKDMA(hdac, DMA_Handle1, hdmaDAC1);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOs clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* Ethernet pins configuration ************************************************/
    /*
    ETH_MDIO -------------------------> PA2     PA2
    ETH_MDC --------------------------> PC1     PC1
    ETH_PPS_OUT ----------------------> PB5
    ETH_MII_CRS ----------------------> PH2     PA1     // WARN Тут с PA0 напутано чего-то. Но работает. Странно.
    ETH_MII_COL ----------------------> PH3     PA3
    ETH_MII_RX_ER --------------------> PI10    PB10
    ETH_MII_RXD2 ---------------------> PH6     PB0
    ETH_MII_RXD3 ---------------------> PH7     PB1
    ETH_MII_TX_CLK -------------------> PC3     PC3
    ETH_MII_TXD2 ---------------------> PC2     PC2
    ETH_MII_TXD3 ---------------------> PB8     PB8
    ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1     PA1
    ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7     PA7
    ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4     PC4
    ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5     PC5
    ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11    PB11
    ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13    PG13
    ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14    PG14
    */
    /*
    PA - 1, 2, 3, 7
    PB - 0, 1, 8, 10, 11
    PC - 1, 2, 3, 4, 5
    PG - 13, 14
    */

    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

    /* Enable ETHERNET clock  */
    __ETH_CLK_ENABLE();

    if (heth->Init.MediaInterface == ETH_MEDIA_INTERFACE_MII)
    {
        /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
        HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_CLK_ENABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
}
