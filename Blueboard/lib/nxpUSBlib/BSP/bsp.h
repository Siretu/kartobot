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


#ifndef __BSP_H__
#define __BSP_H__

#include "lpc_types.h"
#include <stdio.h>
#include <stdbool.h>
#include "TerminalCodes.h"	// FIXME temporarily

#define LEDS_LED1      0
#define LEDS_LED2      0
#define LEDS_LED3      0
#define LEDS_LED4      0
#define LEDS_NO_LEDS	0x00

#define BUTTONS_BUTTON1      	0x01

#define JOY_UP									0x01
#define JOY_DOWN								0x02
#define JOY_LEFT								0x04
#define JOY_RIGHT								0x08
#define JOY_PRESS								0x10
#define NO_BUTTON_PRESSED						0x00

void bsp_init(void);

void DeInitTimer(void);
void InitTimer(uint32_t frequency);
/**
 * Serial API
 */
void Serial_Disable(void);
void Serial_CreateStream(void* Stream);
void Serial_Init(const uint32_t BaudRate, const bool DoubleSpeed);

/**
 * Button API
 */
void Buttons_Init(void);
uint32_t Buttons_GetStatus(void);


/**
 * LED API
 */
void LEDs_Init(void);
void LEDs_TurnOnLEDs(uint32_t LEDMask);
void LEDs_TurnOffLEDs(uint32_t LEDMask);
void LEDs_SetAllLEDs(uint32_t LEDMask);
void LEDs_ChangeLEDs(uint32_t LEDMask, uint32_t ActiveMask);
void LEDs_ToggleLEDs(uint32_t LEDMask);
uint32_t LEDs_GetLEDs(void);

/**
 * JoyStick API
 */
void Joystick_Init(void);
uint8_t Joystick_GetStatus(void);

void USB_Connect(void);

/**
 * Deprecated functions, used for compatible only
 */
#define sei()
#define puts_P(x)		puts(x)
#define PSTR(x)			x
#define printf_P		printf

#endif
