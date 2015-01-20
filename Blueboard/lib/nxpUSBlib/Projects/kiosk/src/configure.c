/**********************************************************************
* $Id$
*//**
* @file
* @brief
* @version
* @date
* @author	NXP MCU Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
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
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "configure.h"

#if (USED_UART_DEBUG_PORT==0)
	#define DEBUG_UART_PORT	((LPC_USARTn_Type*)LPC_UART0)
#elif (USED_UART_DEBUG_PORT==1)
	#define DEBUG_UART_PORT	((LPC_USARTn_Type*)LPC_UART1)
#elif (USED_UART_DEBUG_PORT==3)
	#define DEBUG_UART_PORT	((LPC_USARTn_Type*)LPC_UART3)
#endif

/*********************************************************************//**
* @brief
* @param[in]
* @return
**********************************************************************/
void Serial_Init(const uint32_t BaudRate,uint32_t DoubleSpeed)
{
	UART_CFG_Type UARTConfigStruct;

	#if (USED_UART_DEBUG_PORT==0)
	/*
	 * Initialize UART0 pin connect
	 */
	scu_pinmux(0xF ,10 , MD_PDN, FUNC1); 	// PF.10 : UART0_TXD
	scu_pinmux(0xF ,11 , MD_PDN, FUNC1); 	// PF.11 : UART0_RXD
	#elif (USED_UART_DEBUG_PORT==1)
	/*
	 * Initialize UART1 pin connect
	 */
	scu_pinmux(0xC ,13 , MD_PDN, FUNC2); 	// PC.13 : UART1_TXD
	scu_pinmux(0xC ,14 , MD_PDN, FUNC2); 	// PC.14 : UART1_RXD
	#endif

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
}
/*********************************************************************//**
* @brief
* @param[in]
* @return
**********************************************************************/
void Serial_Disable(void)
{
	// implement later
}
/*********************************************************************//**
* @brief
* @param[in]
* @return
**********************************************************************/
void Serial_CreateStream(FILE* Stream)
{
	// implement later
}
/*********************************************************************//**
* @brief
* @param[in]
* @return
**********************************************************************/
void bsp_init(void)
{
	/* Set up core clock */
	CGU_Init();

	/* Set up USB0 clock */
	CGU_SetPLL0();
	CGU_EnableEntity(CGU_CLKSRC_PLL0, ENABLE);
	CGU_EntityConnect(CGU_CLKSRC_PLL0, CGU_BASE_USB0);

#if defined(USB_HOST_ONLY)
	/* Turn on 5V USB VBUS */
	scu_pinmux(0x6,3,MD_PUP,FUNC0);		// P6_3 pullup, func0 --> GPIO
	FIO_ByteSetDir(0x03,0x00,1<<2,1);
	#if (BOARD == BOARD_EA1800)
		FIO_ByteClearValue(0x03,0x00,1<<2);	// clear P6_3(GPIO3[2]) evaluation board
	#elif (BOARD == BOARD_HITEX1800)
		FIO_ByteSetValue(0x03,0x00,1<<2);		// set P6_3(GPIO3[2]) hitex board
	#endif
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function __write() / __sys_write
//
// Called by bottom level of printf routine within RedLib C library to write
// a character. With the default semihosting stub, this would write the character
// to the debugger console window . But this version writes
// the character to the LPC1768/RDB1768 UART.
int __sys_write (int iFileHandle, char *pcBuffer, int iLength)
{
	//	unsigned int i;
	// 	for (i = 0; i<iLength; i++)
	// 	{
	// 		_DBC(pcBuffer[i]); // print each character
	// 	}

	UART_Send(DEBUG_UART_PORT, (uint8_t*) pcBuffer, iLength, BLOCKING);
	return iLength;
}

// Function __readc() / __sys_readc
//
// Called by bottom level of scanf routine within RedLib C library to read
// a character. With the default semihosting stub, this would read the character
// from the debugger console window (which acts as stdin). But this version reads
// the character from the LPC1768/RDB1768 UART.
int __sys_readc (void)
{
	/*-- UARTGetChar --*/
	uint8_t tmp = 0;
	UART_Receive(DEBUG_UART_PORT, &tmp, 1, BLOCKING);
	return (int) tmp;
}
/************************************************************************/
/* KEIL                                                                     */
/************************************************************************/

/************************************************************************/
/* IAR                                                                     */
/************************************************************************/
