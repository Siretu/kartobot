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

#if defined(__LPC18XX__)

#include "HAL_LPC18xx.h"
#include "lpc18xx_cgu.h"
#include "../../../USBTask.h"

#if defined(USB_CAN_BE_DEVICE)
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL18XX_USBConnect (uint32_t con)
{
  if (con)
    LPC_USB0->USBCMD_D |= (1<<0);
  else
    LPC_USB0->USBCMD_D &= ~(1<<0);
}
#endif

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL18XX_USBInit(void)
 {
	/* Set up USB0 clock */
	CGU_SetPLL0();
	CGU_EnableEntity(CGU_CLKSRC_PLL0, ENABLE);
	CGU_EntityConnect(CGU_CLKSRC_PLL0, CGU_BASE_USB0);

#if defined(USB_CAN_BE_HOST)
#endif

#if defined(USB_CAN_BE_DEVICE)
	/* reset the controller */
	LPC_USB0->USBCMD_D = USBCMD_D_Reset;
	/* wait for reset to complete */
	while (LPC_USB0->USBCMD_D & USBCMD_D_Reset);

	/* Program the controller to be the USB device controller */
	LPC_USB0->USBMODE_D =   (0x2<<0)/*| (1<<4)*//*| (1<<3)*/ ;

	/* set OTG transcever in proper state, device is present
	on the port(CCS=1), port enable/disable status change(PES=1). */
	LPC_USB0->OTGSC = (1<<3) | (1<<0) /*| (1<<16)| (1<<24)| (1<<25)| (1<<26)| (1<<27)| (1<<28)| (1<<29)| (1<<30)*/;

	HAL18XX_Reset();
#endif
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL18XX_USBDeInit(void)
 {
#if defined(USB_CAN_BE_HOST)
 	NVIC_DisableIRQ(USB0_IRQn); 				/* disable USB0 interrupts */
 	LPC_USB0->USBSTS_H = 0xFFFFFFFF; 			/* clear all current interrupts */
 	LPC_USB0->PORTSC1_H &= ~(1<<12);			/* clear port power */
 	LPC_USB0->USBMODE_H =   (1<<0);				/* set USB mode reserve */
#endif
#if defined(USB_CAN_BE_DEVICE)
 	NVIC_DisableIRQ(USB0_IRQn); 				/* disable USB0 interrupts */
	/* Clear all pending interrupts */
	LPC_USB0->USBSTS_D   = 0xFFFFFFFF;
	LPC_USB0->ENDPTNAK   = 0xFFFFFFFF;
	LPC_USB0->ENDPTNAKEN = 0;
	LPC_USB0->ENDPTSETUPSTAT = LPC_USB0->ENDPTSETUPSTAT;
	LPC_USB0->ENDPTCOMPLETE  = LPC_USB0->ENDPTCOMPLETE;
	while (LPC_USB0->ENDPTPRIME);                  /* Wait until all bits are 0 */
	LPC_USB0->ENDPTFLUSH = 0xFFFFFFFF;
	while (LPC_USB0->ENDPTFLUSH); /* Wait until all bits are 0 */
#endif
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL18XX_EnableUSBInterrupt(void)
{
	NVIC_EnableIRQ(USB0_IRQn); //  enable USB0 interrupts
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL18XX_DisableUSBInterrupt(void)
{
	NVIC_DisableIRQ(USB0_IRQn); //  disable USB0 interrupts
}

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void USB0_IRQHandler (void)
{
#ifdef USB_CAN_BE_HOST
	HcdIrqHandler(0);
#endif

#ifdef USB_CAN_BE_DEVICE
	DcdIrqHandler(0);
#endif
	return;
}

#endif /*__LPC18XX__*/
