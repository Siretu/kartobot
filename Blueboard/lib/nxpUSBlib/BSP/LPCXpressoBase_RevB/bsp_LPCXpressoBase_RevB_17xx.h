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


#ifndef __BSP_LPCXPRESSOBASE_REVB_17XX_H__
#define __BSP_LPCXPRESSOBASE_REVB_17XX_H__

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"

#define LPC_TIMER0			LPC_TIM0

#define DEBUG_UART_PORT		LPC_UART3
#define UART_FUNCNUM		2
#define UART_PORTNUM		0
#define UART_TX_PINNUM		0
#define UART_RX_PINNUM		1

#define USB_CONNECT_GPIO_PORT_NUM			0
#define USB_CONNECT_GPIO_BIT_NUM			21

#define BUTTONS_BUTTON1_GPIO_PORT_NUM			1
#define BUTTONS_BUTTON1_GPIO_BIT_NUM			31

#define JOYSTICK_UP_GPIO_PORT_NUM				2
#define JOYSTICK_UP_GPIO_BIT_NUM				3
#define JOYSTICK_DOWN_GPIO_PORT_NUM				0
#define JOYSTICK_DOWN_GPIO_BIT_NUM				15
#define JOYSTICK_LEFT_GPIO_PORT_NUM				2
#define JOYSTICK_LEFT_GPIO_BIT_NUM				4
#define JOYSTICK_RIGHT_GPIO_PORT_NUM			0
#define JOYSTICK_RIGHT_GPIO_BIT_NUM				16
#define JOYSTICK_PRESS_GPIO_PORT_NUM			0
#define JOYSTICK_PRESS_GPIO_BIT_NUM				17

#endif
