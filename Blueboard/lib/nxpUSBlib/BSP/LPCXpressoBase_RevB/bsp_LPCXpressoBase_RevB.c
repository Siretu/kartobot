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


#include "../bsp_internal.h"

#if (BOARD == BOARD_LPCXpressoBase_RevB)

void bsp_init(void)
{
}

// TODO Serial is only available for 1700
void Serial_Init(const uint32_t BaudRate, const bool DoubleSpeed)
{
#ifdef __LPC17XX__
	UART_CFG_Type UARTConfigStruct;
	PINSEL_CFG_Type PinCfg;

	PinCfg.Portnum = UART_PORTNUM;
	PinCfg.Pinnum = UART_TX_PINNUM;
	PinCfg.Funcnum = UART_FUNCNUM;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = UART_RX_PINNUM;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate
	UARTConfigStruct.Baud_rate = BaudRate;

	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(DEBUG_UART_PORT, &UARTConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(DEBUG_UART_PORT, ENABLE);
#endif
}

void USB_Connect(void)
{
#ifdef __LPC17XX__
	GPIO_SetDir(USB_CONNECT_GPIO_PORT_NUM,(1<<USB_CONNECT_GPIO_BIT_NUM),1);				//output
	GPIO_ClearValue(USB_CONNECT_GPIO_PORT_NUM,(1<<USB_CONNECT_GPIO_BIT_NUM));			//pull up D+
#endif
}


void Buttons_Init(void)
{
	GPIO_SetDir(BUTTONS_BUTTON1_GPIO_PORT_NUM,(1<<BUTTONS_BUTTON1_GPIO_BIT_NUM),0); // input
}

void Joystick_Init(void)
{
	GPIO_SetDir(JOYSTICK_UP_GPIO_PORT_NUM,(1<<JOYSTICK_UP_GPIO_BIT_NUM),0); 		// input
	GPIO_SetDir(JOYSTICK_DOWN_GPIO_PORT_NUM,(1<<JOYSTICK_DOWN_GPIO_BIT_NUM),0); 	// input
	GPIO_SetDir(JOYSTICK_LEFT_GPIO_PORT_NUM,(1<<JOYSTICK_LEFT_GPIO_BIT_NUM),0); 	// input
	GPIO_SetDir(JOYSTICK_RIGHT_GPIO_PORT_NUM,(1<<JOYSTICK_RIGHT_GPIO_BIT_NUM),0); 	// input
	GPIO_SetDir(JOYSTICK_PRESS_GPIO_PORT_NUM,(1<<JOYSTICK_PRESS_GPIO_BIT_NUM),0); 	// input
}

uint8_t Joystick_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;

	if((GPIO_ReadValue(JOYSTICK_UP_GPIO_PORT_NUM) & (1<<JOYSTICK_UP_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= JOY_UP;
	}
	else if((GPIO_ReadValue(JOYSTICK_DOWN_GPIO_PORT_NUM) & (1<<JOYSTICK_DOWN_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= JOY_DOWN;
	}
	else if((GPIO_ReadValue(JOYSTICK_LEFT_GPIO_PORT_NUM) & (1<<JOYSTICK_LEFT_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= JOY_LEFT;
	}
	else if((GPIO_ReadValue(JOYSTICK_RIGHT_GPIO_PORT_NUM) & (1<<JOYSTICK_RIGHT_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= JOY_RIGHT;
	}
	else if((GPIO_ReadValue(JOYSTICK_PRESS_GPIO_PORT_NUM) & (1<<JOYSTICK_PRESS_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= JOY_PRESS;
	}

	return ret;
}

#endif
