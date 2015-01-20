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


#ifndef __LPC_HAL_H__
#define __LPC_HAL_H__

#ifdef __CODE_RED
	#include <cr_section_macros.h>
#elif defined(__CC_ARM) // FIXME temporarily fix for Keil (work only for lpc17xx)
	#define __DATA(x)
#endif

/* Includes ------------------------------------------------------------------- */
#if defined(__LPC18XX__)
	#include "LPC18XX/HAL_LPC18xx.h"
#elif defined(__LPC17XX__)
	#include "LPC17XX/HAL_LPC17xx.h"
#elif defined(__LPC11UXX__)
	#include "LPC11UXX/HAL_LPC11Uxx.h"
#endif

void HAL_USBInit(void);
void HAL_USBDeInit(void);
void HAL_EnableUSBInterrupt(void);
void HAL_DisableUSBInterrupt(void);

static __INLINE void HAL_USBConnect (uint32_t con);
static __INLINE void HAL_USBConnect (uint32_t con)
{
#if defined(__LPC18XX__)
	HAL18XX_USBConnect(con);
#elif defined(__LPC17XX__)
	HAL17XX_USBConnect(con);
#elif defined(__LPC11UXX__)
	HAL11UXX_USBConnect(con);
#endif
}

/**
 * @}
 */
#endif /*__LPC_HAL_H__*/
/* --------------------------------- End Of File ------------------------------ */
