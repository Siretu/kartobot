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

#if defined(__LPC11UXX__)

#include "HAL_LPC11Uxx.h"
#include "../../../USBTask.h"


/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL11UXX_USBInit(void)
 {
  /* Enable AHB clock to the USB block and USB RAM. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((0x1<<14)|(0x1<<27));

  LPC_USB->EPBUFCFG = 0x3FC;

  /* configure usb_soft connect */
  LPC_IOCON->PIO0_6 = 0x01;

  HAL11UXX_Reset();
  return;
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL11UXX_USBDeInit(void)
 {
	 NVIC_DisableIRQ(USB_IRQn);								/* disable USB interrupt */
	 LPC_SYSCON->SYSAHBCLKCTRL &= ~((0x1<<14)|(0x1<<27));	/* disable USB clock     */
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL11UXX_EnableUSBInterrupt(void)
{
	NVIC_EnableIRQ(USB_IRQn);
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL11UXX_DisableUSBInterrupt(void)
{
	NVIC_DisableIRQ(USB_IRQn);
}

#endif /*__LPC11UXX__*/
