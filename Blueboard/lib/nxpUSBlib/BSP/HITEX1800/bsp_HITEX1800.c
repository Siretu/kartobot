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

#if (BOARD == BOARD_HITEX1800)

void bsp_init(void)
{
	/* Set up core clock */
	CGU_Init();

	/* Turn on 5V USB VBUS TODO Should be Host-only */
	scu_pinmux(0x6,3,MD_PUP|(0x1<<6),FUNC1);		// P6_3 USB0_PWR_EN, USB0 VBus function
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

void Buttons_Init(void)	// FIXME not functional ATM
{
	I2C_M_SETUP_Type transferMCfg;
	uint8_t sendbuf[4]; //,receivebuf,tem;

	/* Initiate I2C */
	I2C_Init(LPC_I2C0, 100000);
	I2C_Cmd(LPC_I2C0, ENABLE);

	/* Configure PCA9502 */
	sendbuf[0] = PCA9502_REG_ADDR(PCA9502_REG_IODIR);
	sendbuf[1] = 0x0F;						/* IO[7:4]=input IO[3:0]=output */
	transferMCfg.sl_addr7bit = I2CDEV_PCA9502_ADDR;
	transferMCfg.tx_data = sendbuf ;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_POLLING);

	sendbuf[0] = PCA9502_REG_ADDR(PCA9502_REG_IOSTATE);
	sendbuf[1] = 0x00;						/* all output low */
	transferMCfg.sl_addr7bit = I2CDEV_PCA9502_ADDR;
	transferMCfg.tx_data = sendbuf ;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_POLLING);

	sendbuf[0] = PCA9502_REG_ADDR(PCA9502_REG_IOINTENA);
	sendbuf[1] = 0xF0;						/* enable interrupt for IO[7:4] */
	transferMCfg.sl_addr7bit = I2CDEV_PCA9502_ADDR;
	transferMCfg.tx_data = sendbuf ;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_POLLING);

	/* Configure GPIO_IRQ pin */
	scu_pinmux(0xA,1,MD_PUP|(0x1<<6),FUNC0);			// PA_1  pullup, func0 --> GPIO4[8]

	GPIO_SetDir(BUTTONS_BUTTON1_GPIO_PORT_NUM,(1<<BUTTONS_BUTTON1_GPIO_BIT_NUM),0); // input
}

// TODO not yet functional
//void LEDs_Init(void)
//{
//}

// TODO not yet functional
//void LEDs_SetAllLEDs(uint32_t LEDMask)
//{
//	I2C_M_SETUP_Type transferMCfg;
//	uint8_t sendbuf[2];
//
//	sendbuf[0] = PCA9502_REG_ADDR(PCA9502_REG_IOSTATE);
//	sendbuf[1] = ~(uint8_t)(LEDMask & 0x0F);
//	transferMCfg.sl_addr7bit = I2CDEV_PCA9502_ADDR;
//	transferMCfg.tx_data = sendbuf ;
//	transferMCfg.tx_length = 2;
//	transferMCfg.rx_data = NULL;
//	transferMCfg.rx_length = 0;
//	transferMCfg.retransmissions_max = 3;
//	I2C_MasterTransferData(LPC_I2C0, &transferMCfg, I2C_TRANSFER_POLLING);
//}

void USB_Connect(void) {};
void Joystick_Init(void) {}
uint8_t Joystick_GetStatus(void)
{
	return NO_BUTTON_PRESSED;
}

#endif
