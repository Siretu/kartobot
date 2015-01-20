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

#if (BOARD == BOARD_EA1800)

void bsp_init(void)
{
	/* Set up core clock */
	CGU_Init();

	/* Turn on 5V USB VBUS, EA1800 use active-low to enable usb bus --> cannot use configure a USB0_PWR_EN */
	/* TODO Should be Host-only */
	scu_pinmux(0x6,3,MD_PUP|(0x1<<6),FUNC0);		// P6_3 pullup, func0 --> GPIO
	FIO_ByteSetDir(0x03,0x00,1<<2,1);
	FIO_ByteClearValue(0x03,0x00,1<<2);
}

void Serial_Init(const uint32_t BaudRate, const bool DoubleSpeed)
{
	UART_CFG_Type UARTConfigStruct;

	/*
	 * Initialize UART1 pin connect
	 */
	scu_pinmux(0xC ,13 , MD_PDN|(0x1<<6), FUNC2); 	// PC.13 : UART1_TXD
	scu_pinmux(0xC ,14 , MD_PDN|(0x1<<6), FUNC2); 	// PC.14 : UART1_RXD

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = BaudRate;	// Re-configure baudrate

	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(DEBUG_UART_PORT, &UARTConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(DEBUG_UART_PORT, ENABLE);
}

void Buttons_Init(void)
{
	scu_pinmux(0x6,10,MD_PUP|(0x1<<6),FUNC0);		// P6_10 pullup, func0 --> GPIO3[6]
	GPIO_SetDir(BUTTONS_BUTTON1_GPIO_PORT_NUM,BUTTONS_BUTTON1,0); // input
}

// TODO not yet functional
//void LEDs_Init(void)
//{
//	scu_pinmux(0x8,0,MD_PUP,FUNC0);			// P8_0  pullup, func0 --> GPIO4[0]
//	scu_pinmux(0x8,1,MD_PUP,FUNC0);			// P8_1  pullup, func0 --> GPIO4[1]
//	scu_pinmux(0x8,2,MD_PUP,FUNC0);			// P8_2  pullup, func0 --> GPIO4[2]
//	scu_pinmux(0x8,3,MD_PUP,FUNC0);			// P8_3  pullup, func0 --> GPIO4[3]
//	GPIO_SetDir(4,0x0F,1); 					// output
//}
// TODO not yet functional
//void LEDs_SetAllLEDs(uint32_t LEDMask)
//{
//	GPIO_ClearValue(4,0x0F);
//	GPIO_SetValue(4,(LEDMask&0x0F));
//}

void USB_Connect(void) {};
void Joystick_Init(void) {}
uint8_t Joystick_GetStatus(void)
{
	return NO_BUTTON_PRESSED;
}

#endif
