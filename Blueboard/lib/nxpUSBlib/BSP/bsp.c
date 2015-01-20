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


#include "bsp_internal.h"

/**
 * LED API
 */
void LEDs_Init(void) {}
void LEDs_TurnOnLEDs(uint32_t LEDMask) {}
void LEDs_TurnOffLEDs(uint32_t LEDMask) {}
void LEDs_SetAllLEDs(uint32_t LEDMask) {}
void LEDs_ChangeLEDs(uint32_t LEDMask, uint32_t ActiveMask) {}
void LEDs_ToggleLEDs(uint32_t LEDMask) {}
uint32_t LEDs_GetLEDs(void) { return 0; }

/**
 * JoyStick API
 */

/**
 * Serial
 */

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
void Serial_CreateStream(void* Stream)
{
	// implement later
}

uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;

	if((GPIO_ReadValue(BUTTONS_BUTTON1_GPIO_PORT_NUM) & (1UL<<BUTTONS_BUTTON1_GPIO_BIT_NUM)) == 0x00)
	{
		ret |= BUTTONS_BUTTON1;
	}
	return ret;

}

#ifndef __LPC11UXX__ // TODO PDL lib for LPC1100 is not fully supported
void InitTimer(uint32_t frequency)
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct ;
	uint32_t matchvalue;

	if(frequency > 100000)
		return;
	matchvalue = 1000000 / frequency;

	// Initialize timer 0, prescale count time of 1uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
	// Set Match value
	TIM_MatchConfigStruct.MatchValue   = matchvalue;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIMER0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIMER0,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	// To start timer 0
	TIM_Cmd(LPC_TIMER0,ENABLE);
}

/*********************************************************************//**
* @brief
* @param[in]
* @return
**********************************************************************/
void DeInitTimer(void)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
	TIM_Cmd(LPC_TIMER0,DISABLE);
	TIM_DeInit(LPC_TIMER0);
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

#endif
/************************************************************************/
/* KEIL                                                                     */
/************************************************************************/

/************************************************************************/
/* IAR                                                                     */
/************************************************************************/
