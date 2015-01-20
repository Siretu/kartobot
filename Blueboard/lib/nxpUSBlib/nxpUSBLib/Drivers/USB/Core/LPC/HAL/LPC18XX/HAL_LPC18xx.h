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

#ifndef __HAL_LPC18XX_H__
#define __HAL_LPC18XX_H__

#include "LPC18xx.h"
#include "lpc_types.h"

#define  __INCLUDE_FROM_USB_DRIVER
#include "../../../USBMode.h"

#define USBRAM_SECTION	RAM4
extern void HcdIrqHandler(uint8_t HostID);
extern void DcdIrqHandler (uint8_t HostID);

void HAL18XX_USBInit(void);
void HAL18XX_USBDeInit(void);
void HAL18XX_EnableUSBInterrupt(void);
void HAL18XX_DisableUSBInterrupt(void);
void HAL18XX_Reset (void);
void HAL18XX_USBConnect (uint32_t con);

/* bit defines for DEVICEADDR register */
#define USBDEV_ADDR_AD	(1<<24)
#define USBDEV_ADDR(n)	(((n) & 0x7F)<<25)

#endif // __HAL_LPC18XX_H__
