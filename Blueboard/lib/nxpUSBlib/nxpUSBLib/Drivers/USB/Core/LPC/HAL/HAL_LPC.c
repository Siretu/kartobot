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


/* Includes ------------------------------------------------------------------- */
#include "HAL_LPC.h"

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL_USBInit(void)
 {
	#if defined(__LPC18XX__)
		HAL18XX_USBInit();
	#elif defined(__LPC17XX__)
		HAL17XX_USBInit();
	#elif defined(__LPC11UXX__)
		HAL11UXX_USBInit();
	#endif
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
 void HAL_USBDeInit(void)
 {
	#if defined(__LPC18XX__)
		HAL18XX_USBDeInit();
	#elif defined(__LPC17XX__)
		HAL17XX_USBDeInit();
	#elif defined(__LPC11UXX__)
		HAL11UXX_USBDeInit();
	#endif
 }
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL_EnableUSBInterrupt(void)
{
#if defined(__LPC18XX__)
	HAL18XX_EnableUSBInterrupt();
#elif defined(__LPC17XX__)
	HAL17XX_EnableUSBInterrupt();
#elif defined(__LPC11UXX__)
	HAL11UXX_EnableUSBInterrupt();
#endif
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL_DisableUSBInterrupt(void)
{
#if defined(__LPC18XX__)
	HAL18XX_DisableUSBInterrupt();
#elif defined(__LPC17XX__)
	HAL17XX_DisableUSBInterrupt();
#elif defined(__LPC11UXX__)
	HAL11UXX_DisableUSBInterrupt();
#endif
}

/* --------------------------------- End Of File ------------------------------ */
