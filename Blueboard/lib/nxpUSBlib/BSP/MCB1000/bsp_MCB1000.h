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


#ifndef __BSP_MCB1100_H__
#define __BSP_MCB1100_H__

#include "lpc11uxx_pinsel.h"
#include "lpc11uxx_gpio.h"

#define USB_CONNECT_GPIO_PORT_NUM			0
#define USB_CONNECT_GPIO_BIT_NUM			6

#define BUTTONS_BUTTON1_GPIO_PORT_NUM			1
#define BUTTONS_BUTTON1_GPIO_BIT_NUM			5

#define JOYSTICK_PRESS_GPIO_PORT_NUM			1
#define JOYSTICK_PRESS_GPIO_BIT_NUM				4

#endif
