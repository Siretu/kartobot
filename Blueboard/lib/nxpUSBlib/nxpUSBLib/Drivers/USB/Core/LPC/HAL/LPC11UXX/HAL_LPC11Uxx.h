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

#ifndef __HAL_LPC11UXX_H__
#define __HAL_LPC11UXX_H__

#include "LPC11Uxx.h"
#define  __INCLUDE_FROM_USB_DRIVER
#include "../../../USBMode.h"

#define USBRAM_SECTION RAM2
typedef struct {
	uint16_t BufferAddrOffset;
	
	__IO uint16_t NBytes : 10;
	uint16_t Type : 1;
	uint16_t RateFeedback_Toogle : 1;
	uint16_t ToogleReset : 1;
	__IO uint16_t Stall : 1;
	uint16_t Disabled : 1;
	__IO uint16_t Active : 1;
	
}/* __attribute__ ((packed)) */USB_CMD_STAT;

void HAL11UXX_USBInit(void);
void HAL11UXX_USBDeInit(void);
void HAL11UXX_EnableUSBInterrupt(void);
void HAL11UXX_DisableUSBInterrupt(void);
void HAL11UXX_Reset (void);
void HAL11UXX_SetDeviceAddress (uint8_t Address);
void HAL11UXX_USBConnect (uint32_t con);

#endif // __HAL_LPC11UXX_H__
