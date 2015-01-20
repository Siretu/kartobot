/*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
*         LUFA Library
* Copyright (C) Dean Camera, 2011.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* 
* Permission to use, copy, modify, and distribute this software
* and its documentation for any purpose is hereby granted without
* fee, provided that it is used in conjunction with NXP Semiconductors
* microcontrollers.  This copyright, permission, and disclaimer notice
* must appear in all copies of this code.
*/

#if defined(__LPC17XX__)

#include "HAL_LPC17xx.h"
#include "../../../USBTask.h"

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL17XX_USBInit(void)
 {
  	LPC_PINCON->PINSEL1 &= ~((3<<26)|(3<<28));  /* P0.29 D+, P0.30 D- */
  	LPC_PINCON->PINSEL1 |=  ((1<<26)|(1<<28));  /* PINSEL1 26.27, 28.29  = 01 */
  	LPC_SC->PCONP |= (1UL<<31);                	/* USB PCLK -> enable USB Per.*/
#if defined(USB_CAN_BE_DEVICE)
	LPC_USB->USBClkCtrl = 0x1A;                  /* Dev, PortSel, AHB clock enable */
	while ((LPC_USB->USBClkSt & 0x1A) != 0x1A);

	HAL17XX_Reset();
#endif
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL17XX_USBDeInit(void)
 {
 	NVIC_DisableIRQ(USB_IRQn);               	/* disable USB interrupt */
 	LPC_SC->PCONP &= (~(1UL<<31));                /* disable USB Per.      */
 	LPC_PINCON->PINSEL1 &= ~((3<<26)|(3<<28));  /* P0.29 D+, P0.30 D- reset to GPIO function */
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL17XX_EnableUSBInterrupt(void)
{
	NVIC_EnableIRQ(USB_IRQn);               	/* enable USB interrupt */
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL17XX_DisableUSBInterrupt(void)
{
	NVIC_DisableIRQ(USB_IRQn);               	/* enable USB interrupt */
}

// TODO moving stuff to approriate places
extern void DcdIrqHandler (uint8_t DeviceID);
void USB_IRQHandler (void)
{
#ifdef USB_CAN_BE_HOST
	HcdIrqHandler(0);
#endif

#ifdef USB_CAN_BE_DEVICE
	DcdIrqHandler(0);
#endif
	return;
}

#endif /*__LPC17XX__*/
