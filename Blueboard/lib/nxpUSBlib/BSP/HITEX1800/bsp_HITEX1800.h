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


#ifndef __BSP_EA1800_H__
#define __BSP_EA1800_H__

#include "lpc18xx_uart.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_gpio.h"
#include "lpc18xx_timer.h"
#include "lpc18xx_i2c.h"

#define DEBUG_UART_PORT	((LPC_USARTn_Type*)LPC_UART1)

#define BUTTONS_BUTTON1_GPIO_PORT_NUM	4
#define BUTTONS_BUTTON1_GPIO_BIT_NUM	8
#define I2CDEV_PCA9502_ADDR		(0x9A>>1)
#define PCA9502_REG_IODIR		0x0A
#define PCA9502_REG_IOSTATE		0x0B
#define PCA9502_REG_IOINTENA	0x0C
#define PCA9502_REG_IOCONTROL	0x0E
#define PCA9502_REG_ADDR(x)		(((x) & 0x0F)<<3)

#endif
