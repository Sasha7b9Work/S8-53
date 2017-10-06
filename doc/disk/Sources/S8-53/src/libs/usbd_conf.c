/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_conf.c
  * @author  MCD Application Team
  * @version V1.1.3
  * @date    17-March-2017
  * @brief   This file implements the USB Device library callbacks and MSP
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "defines.h"
#include "stm32f2xx_hal.h"
#include "usbd_core.h"
#include "VCP/VCP.h"
#include "Hardware/Timer.h"
#include "Display/Display.h"
#include "Log.h"
#include "Settings/Settings.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern PCD_HandleTypeDef handlePCD;
extern USBD_HandleTypeDef handleUSBD;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


//----------------------------------------------------------------------------------------------------------------------------------------------------
/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/
/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Configure USB FS GPIOs */
    __GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Enable USB FS Clocks */
    __USB_OTG_FS_CLK_ENABLE();

    /* Set USBFS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 1, 0);

    /* Enable USBFS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    __USB_OTG_FS_CLK_DISABLE();
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{

    USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);

    USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef*)(hpcd->pData);  //
    USBD_SetupReqTypedef request = pdev->request;                   //
                                                                    //
    static uint16 prevLength = 7;                                   //
                                                                    //
    if (request.wLength == 0)                                       //
    {                                                               //
        if (gBF.cableVCPisConnected == 1)                           //
        {                                                           //
            if (prevLength != 0)                                    //
            {                                                       //
                gBF.connectToHost = 1;                              // GOVNOCODE ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (
            }                                                       //
            else                                                    //
            {                                                       //
                gBF.connectToHost = 0;                              //
                Settings_Save();                                    // ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ // WARN ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
            }                                                       //
            gBF.connectToHost = (prevLength != 0) ? 1 : 0;          // 
        }                                                           //
    }                                                               //
    prevLength = request.wLength;                                   //
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
    
    // \todo WARN ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½. ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF(hpcd->pData);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{ 
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    /* Set USB Current Speed */
    switch(hpcd->Init.speed)
    {
    case PCD_SPEED_HIGH:
        speed = USBD_SPEED_HIGH;
        break;
    
    case PCD_SPEED_FULL:
        speed = USBD_SPEED_FULL;    
        break;
	
	default:
        speed = USBD_SPEED_FULL;    
        break;
    }
    USBD_LL_SetSpeed(hpcd->pData, speed);  
  
    /* Reset Device */
    USBD_LL_Reset(hpcd->pData);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Suspend(hpcd->pData);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Resume(hpcd->pData);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevConnected(hpcd->pData);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevDisconnected(hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef  USBD_LL_Init (USBD_HandleTypeDef *pdev)
{ 
    /* Change Systick prioity */
    NVIC_SetPriority (SysTick_IRQn, 0);  
  
    /*Set LL Driver parameters */
    handlePCD.Instance = USB_OTG_FS;
    handlePCD.Init.dev_endpoints = 4; 
    handlePCD.Init.use_dedicated_ep1 = 0;
    handlePCD.Init.ep0_mps = 0x40;  
    handlePCD.Init.dma_enable = 0;
    handlePCD.Init.low_power_enable = 0;
    handlePCD.Init.phy_itface = PCD_PHY_EMBEDDED; 
    handlePCD.Init.Sof_enable = 0;
    handlePCD.Init.speed = PCD_SPEED_FULL;
    handlePCD.Init.vbus_sensing_enable = 1;
    /* Link The driver to the stack */
    handlePCD.pData = pdev;
    pdev->pData = &handlePCD;
    /*Initialize LL Driver */
    HAL_PCD_Init(&handlePCD);
  
    HAL_PCD_SetRxFiFo(&handlePCD, 0x80);
    HAL_PCD_SetTxFiFo(&handlePCD, 0, 0x40);
    HAL_PCD_SetTxFiFo(&handlePCD, 1, 0x80); 

    return USBD_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_PCD_DeInit(pdev->pData);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_PCD_Start(pdev->pData);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_PCD_Stop(pdev->pData);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, 
                                  uint8_t  ep_addr,                                      
                                  uint8_t  ep_type,
                                  uint16_t ep_mps)
{
    HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)   
{
    HAL_PCD_EP_Close(pdev->pData, ep_addr);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)   
{
    HAL_PCD_EP_Flush(pdev->pData, ep_addr);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)   
{
    HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)   
{
    HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);  
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)   
{
    PCD_HandleTypeDef *hpcd = pdev->pData; 
  
    if((ep_addr & 0x80) == 0x80)
    {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall; 
    }
    else
    {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall; 
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)   
{
    HAL_PCD_SetAddress(pdev->pData, dev_addr);
    return USBD_OK; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, 
                                    uint8_t  ep_addr,                                      
                                    uint8_t  *pbuf,
                                    uint16_t  size)
{
    HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;   
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, 
                                          uint8_t  ep_addr,                                      
                                          uint8_t  *pbuf,
                                          uint16_t  size)
{
    HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;   
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t  ep_addr)  
{
    return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void  USBD_LL_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);  
}
