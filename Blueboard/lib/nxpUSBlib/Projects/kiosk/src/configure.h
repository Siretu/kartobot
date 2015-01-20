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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Includes ------------------------------------------------------------------- */
#include <stdio.h>
#include "lpc18xx_uart.h"
#include "scu.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define sei()		/* AVR global interrupt enable */

/*********************************************************************//**
 * @brief TERMINAL functions
 **********************************************************************/
#if !defined(DISABLE_TERMINAL_CODES)
	#define ANSI_ESCAPE_SEQUENCE(EscapeSeq)  "\33[" EscapeSeq
#else
	#define ANSI_ESCAPE_SEQUENCE(EscapeSeq)
#endif
#define ESC_BOLD_ON                     ANSI_ESCAPE_SEQUENCE("1m")
#define ESC_ITALICS_ON                  ANSI_ESCAPE_SEQUENCE("3m")
#define ESC_UNDERLINE_ON                ANSI_ESCAPE_SEQUENCE("4m")
#define ESC_INVERSE_ON                  ANSI_ESCAPE_SEQUENCE("7m")
#define ESC_STRIKETHROUGH_ON            ANSI_ESCAPE_SEQUENCE("9m")
#define ESC_BOLD_OFF                    ANSI_ESCAPE_SEQUENCE("22m")
#define ESC_ITALICS_OFF                 ANSI_ESCAPE_SEQUENCE("23m")
#define ESC_UNDERLINE_OFF               ANSI_ESCAPE_SEQUENCE("24m")
#define ESC_INVERSE_OFF                 ANSI_ESCAPE_SEQUENCE("27m")
#define ESC_STRIKETHROUGH_OFF           ANSI_ESCAPE_SEQUENCE("29m")
#define ESC_FG_BLACK                    ANSI_ESCAPE_SEQUENCE("30m")
#define ESC_FG_RED                      ANSI_ESCAPE_SEQUENCE("31m")
#define ESC_FG_GREEN                    ANSI_ESCAPE_SEQUENCE("32m")
#define ESC_FG_YELLOW                   ANSI_ESCAPE_SEQUENCE("33m")
#define ESC_FG_BLUE                     ANSI_ESCAPE_SEQUENCE("34m")
#define ESC_FG_MAGENTA                  ANSI_ESCAPE_SEQUENCE("35m")
#define ESC_FG_CYAN                     ANSI_ESCAPE_SEQUENCE("36m")
#define ESC_FG_WHITE                    ANSI_ESCAPE_SEQUENCE("37m")
#define ESC_FG_DEFAULT                  ANSI_ESCAPE_SEQUENCE("39m")
#define ESC_BG_BLACK                    ANSI_ESCAPE_SEQUENCE("40m")
#define ESC_BG_RED                      ANSI_ESCAPE_SEQUENCE("41m")
#define ESC_BG_GREEN                    ANSI_ESCAPE_SEQUENCE("42m")
#define ESC_BG_YELLOW                   ANSI_ESCAPE_SEQUENCE("43m")
#define ESC_BG_BLUE                     ANSI_ESCAPE_SEQUENCE("44m")
#define ESC_BG_MAGENTA                  ANSI_ESCAPE_SEQUENCE("45m")
#define ESC_BG_CYAN                     ANSI_ESCAPE_SEQUENCE("46m")
#define ESC_BG_WHITE                    ANSI_ESCAPE_SEQUENCE("47m")
#define ESC_BG_DEFAULT                  ANSI_ESCAPE_SEQUENCE("49m")
#define ESC_CURSOR_POS_SAVE             ANSI_ESCAPE_SEQUENCE("s")
#define ESC_CURSOR_POS_RESTORE          ANSI_ESCAPE_SEQUENCE("u")
#define ESC_CURSOR_POS(Line, Column)    ANSI_ESCAPE_SEQUENCE(#Line ";" #Column "H")
#define ESC_CURSOR_UP(Lines)            ANSI_ESCAPE_SEQUENCE(#Lines "A")
#define ESC_CURSOR_DOWN(Lines)          ANSI_ESCAPE_SEQUENCE(#Lines "B")
#define ESC_CURSOR_FORWARD(Columns)     ANSI_ESCAPE_SEQUENCE(#Columns "C")
#define ESC_CURSOR_BACKWARD(Columns)    ANSI_ESCAPE_SEQUENCE(#Columns "D")
#define ESC_RESET                       ANSI_ESCAPE_SEQUENCE("0m")
#define ESC_ERASE_DISPLAY               ANSI_ESCAPE_SEQUENCE("2J")
#define ESC_ERASE_LINE                  ANSI_ESCAPE_SEQUENCE("K")

#define puts_P(x)		puts(x)
#define PSTR(x)			x
#define printf_P		printf

void Serial_Disable(void);
void Serial_CreateStream(FILE* Stream);
void Serial_Init(const uint32_t BaudRate,uint32_t DoubleSpeed);

/*********************************************************************//**
 * @brief LED functions
 **********************************************************************/
#define LEDS_LED1      0
#define LEDS_LED2      0
#define LEDS_LED3      0
#define LEDS_LED4      0

static inline void LEDs_Init(void) {}
static inline void LEDs_TurnOnLEDs(uint32_t LEDMask) {}
static inline void LEDs_TurnOffLEDs(uint32_t LEDMask) {}
static inline void LEDs_SetAllLEDs(uint32_t LEDMask) {}
static inline void LEDs_ChangeLEDs(uint32_t LEDMask, uint32_t ActiveMask) {}
static inline void LEDs_ToggleLEDs(uint32_t LEDMask) {}
static inline uint32_t LEDs_GetLEDs(void) { return 0; }

/*********************************************************************//**
 * @brief Board support functions
 **********************************************************************/
#define BOARD_LPCXpressoBase_RevB       50
#define BOARD_EA1800					51
#define BOARD_HITEX1800					52
#define BOARD_MCB1000					53

#if ((BOARD==BOARD_EA1800)||(BOARD == BOARD_HITEX1800))
#define USED_UART_DEBUG_PORT	1
#endif
void bsp_init(void);

#ifdef __cplusplus
}
#endif

#endif /*__CONFIG_H__*/
