/***********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: Simple timer driver.
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
#ifndef __TIMER_H
#define __TIMER_H

/* The test is either MAT_OUT or CAP_IN. Default is MAT_OUT. */
/* If running DMA test program which uses MAT_OUT to trigger DMA, set
TIMER_MATCH to 1 */
#define TIMER_MATCH		0

/* MHZ_PRESCALE is a value to set the prescaler to in order to
   clock the timer at 1 MHz. Clock needs to be a multiple of 1 MHz or
   this will not work. */
#define MHZ_PRESCALE    (AHBSystemFrequency/1000000)

/* TIME_INTERVALmS is a value to load the timer match register with
   to get a 1 mS delay */
#define TIME_INTERVALmS	1000

void TIMER0_IRQHandler (void);
void TIMER1_IRQHandler (void);
void TIMER2_IRQHandler (void);
void TIMER3_IRQHandler (void);
void vTimer_Enable(uint8_t u8Timer);
void vTimer_Disable(uint8_t u8Timer);
uint32_t u32Timer_Init(uint8_t u8Timer, uint32_t u32Location, uint32_t u32TimerInterval);

extern volatile uint32_t timer0_counter;

#endif /* end __TIMER_H */

/**********************************************************************
 **                            End Of File
 **********************************************************************/
