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
#include "LPC18xx.h"
#include "type.h"
#include "timer.h"
#include "scu.h"

volatile uint32_t timer0_counter = 0;
volatile uint32_t timer1_m0_counter = 0;
volatile uint32_t timer2_m0_counter = 0;
volatile uint32_t timer3_m0_counter = 0;
volatile uint32_t timer0_m1_counter = 0;
volatile uint32_t timer1_m1_counter = 0;
volatile uint32_t timer2_m1_counter = 0;
volatile uint32_t timer3_m1_counter = 0;

volatile uint32_t timer0_capture0 = 0;
volatile uint32_t timer1_capture0 = 0;
volatile uint32_t timer2_capture0 = 0;
volatile uint32_t timer3_capture0 = 0;
volatile uint32_t timer0_capture1 = 0;
volatile uint32_t timer1_capture1 = 0;
volatile uint32_t timer2_capture1 = 0;
volatile uint32_t timer3_capture1 = 0;

/******************************************************************************
 ** Function name:		u32Timer_Init
 **
 ** Descriptions:		Initialize timer, set timer interval, reset timer,
 **						install timer interrupt handler
 **
 ** parameters:			timer number and timer interval
 ** Returned value:		true or false, if the interrupt handler can't be
 **						installed, return false.
 **
 ******************************************************************************/
uint32_t u32Timer_Init(uint8_t u8Timer, uint32_t u32Location, uint32_t u32TimerInterval)
{
  	uint32_t u32Result = FALSE;

	if (u8Timer == 0)
  	{
		LPC_TIMER0->TCR = 0;
		timer0_capture0   = 0;
		timer0_capture1   = 0;
		timer0_counter = 0;
		timer0_m1_counter = 0;

		LPC_TIMER0->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
		LPC_TIMER0->MR[0] = u32TimerInterval; //TIME_INTERVALmS * 10; /* Set up 10 mS interval */
		LPC_TIMER0->MR[1] = u32TimerInterval; //TIME_INTERVALmS * 10; /* Set up 10 mS interval */

		#if TIMER_MATCH
			LPC_TIMER0->EMR &= ~(0xFF<<4);
			LPC_TIMER0->EMR |= ((0x3<<4)|(0x3<<6));
		#else
			/* Capture 0 and 1 on rising edge, interrupt enable. */
			LPC_TIMER0->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
		#endif

		LPC_TIMER0->MCR = (0x3UL << 0)|(0x3UL << 3);	/* Interrupt and Reset on MR0 and MR1 */
		NVIC_EnableIRQ(TIMER0_IRQn);
		u32Result = TRUE;
  	}
  	else if (u8Timer == 1)
  	{
		LPC_TIMER1->TCR = 0;
		timer1_m0_counter = 0;
		timer1_m1_counter = 0;
		timer1_capture0 = 0;
		timer1_capture1 = 0;

		LPC_TIMER1->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
		LPC_TIMER1->MR[0] = u32TimerInterval; //TIME_INTERVALmS * 10; /* Set up 10 mS interval */
		LPC_TIMER1->MR[1] = u32TimerInterval; //TIME_INTERVALmS * 10; /* Set up 10 mS interval */

		#if TIMER_MATCH
			LPC_TIMER1->EMR &= ~(0xFF<<4);
			LPC_TIMER1->EMR |= ((0x3<<4)|(0x3<<6));
		#else
			/* Capture 0/1 on rising edge, interrupt enable. */
			LPC_TIMER1->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
		#endif

		LPC_TIMER1->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
		NVIC_EnableIRQ(TIMER1_IRQn);
		return (TRUE);
  	}
  	else if (u8Timer == 2)
  	{
		LPC_TIMER2->TCR = 0;
		timer2_m0_counter = 0;
		timer2_m1_counter = 0;
		timer2_capture0 = 0;
		timer2_capture1 = 0;

		LPC_TIMER2->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
		LPC_TIMER2->MR[0] = u32TimerInterval; /* Set up 10 mS interval */
		LPC_TIMER2->MR[1] = u32TimerInterval; /* Set up 10 mS interval */

		#if TIMER_MATCH
			LPC_TIMER2->EMR &= ~(0xFF<<4);
			LPC_TIMER2->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));
		#else
			/* Capture 0/1 on rising edge, interrupt enable. */
			LPC_TIMER2->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
		#endif

		LPC_TIMER2->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
		NVIC_EnableIRQ(TIMER2_IRQn);
		return (TRUE);
  	}
  	else if (u8Timer == 3)
  	{
		LPC_TIMER3->TCR = 0;
		timer3_m0_counter = 0;
		timer3_m1_counter = 0;
		timer3_capture0 = 0;
		timer3_capture1 = 0;

		LPC_TIMER3->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
		LPC_TIMER3->MR[0] = u32TimerInterval; /* Set up 10 mS interval */
		LPC_TIMER3->MR[1] = u32TimerInterval; /* Set up 10 mS interval */

		#if TIMER_MATCH
			LPC_TIMER3->EMR &= ~(0xFF<<4);
			LPC_TIMER3->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));
		#else
			/* Capture 0/1 on rising edge, interrupt enable. */
			LPC_TIMER3->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
		#endif

		LPC_TIMER3->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
		NVIC_EnableIRQ(TIMER3_IRQn);
		return (TRUE);
  	}
  	return (u32Result);
}

/******************************************************************************
 ** Function name:		vTimer_Enable
 **
 ** Descriptions:		Enable timer
 **
 ** parameters:			timer number: 0 or 1 or 2 or 3
 ** Returned value:		None
 **
 ******************************************************************************/
void vTimer_Enable(uint8_t u8Timer)
{
  	if (u8Timer == 0)
  	{
		LPC_TIMER0->TCR = 1;
  	}
  	else if (u8Timer == 1)
  	{
		LPC_TIMER1->TCR = 1;
  	}
  	else if (u8Timer == 2)
  	{
		LPC_TIMER2->TCR = 1;
  	}
  	else if (u8Timer == 3)
  	{
		LPC_TIMER3->TCR = 1;
  	}
}

/******************************************************************************
 ** Function name:		vTimer_Enable
 **
 ** Descriptions:		Enable timer
 **
 ** parameters:			timer number: 0 or 1 or 2 or 3
 ** Returned value:		None
 **
 ******************************************************************************/
void vTimer_Disable(uint8_t u8Timer)
{
  	if (u8Timer == 0)
  	{
		LPC_TIMER0->TCR = 0;
  	}
  	else if (u8Timer == 1)
  	{
		LPC_TIMER1->TCR = 0;
  	}
  	else if (u8Timer == 2)
  	{
		LPC_TIMER2->TCR = 0;
  	}
  	else if (u8Timer == 3)
  	{
		LPC_TIMER3->TCR = 0;
  	}
}

/******************************************************************************
 ** Function name:		Timer0_IRQHandler
 **
 ** Descriptions:		Timer/Counter 0 interrupt handler
 **
 ** parameters:			None
 ** Returned value:		None
 **
 ******************************************************************************/
void TIMER0_IRQHandler (void)
{

	if (LPC_TIMER0->IR & (0x1UL << 0))
  	{
    	/* clear interrupt flag */
		LPC_TIMER0->IR = 0x1UL << 0;
    	timer0_counter++;
  	}
  	if (LPC_TIMER0->IR & (0x1UL << 1))
  	{
    	/* clear interrupt flag */
		LPC_TIMER0->IR = 0x1UL << 1;
    	timer0_m1_counter++;
  	}
  	if (LPC_TIMER0->IR & (0x1UL << 4))
  	{
		/* clear interrupt flag */
		LPC_TIMER0->IR = 0x1UL << 4;
		timer0_capture0++;
  	}
  	if (LPC_TIMER0->IR & (0x1UL << 5))
  	{
		/* clear interrupt flag */
		LPC_TIMER0->IR = 0x1UL << 5;
		timer0_capture1++;
  	}
}

/******************************************************************************
 ** Function name:		Timer1_IRQHandler
 **
 ** Descriptions:		Timer/Counter 1 interrupt handler
 **
 ** parameters:			None
 ** Returned value:		None
 **
 ******************************************************************************/
void TIMER1_IRQHandler (void)
{
  	if (LPC_TIMER1->IR & (0x1UL << 0))
  	{
    	/* clear interrupt flag */
		LPC_TIMER1->IR = 0x1UL << 0;
    	timer1_m0_counter++;
    	kiosk_advance_slide();
  	}
  	if (LPC_TIMER1->IR & (0x1UL << 1))
  	{
    	/* clear interrupt flag */
		LPC_TIMER1->IR = 0x1UL << 1;
    	timer1_m1_counter++;
  	}
  	if (LPC_TIMER1->IR & (0x1UL << 4))
  	{
		/* clear interrupt flag */
		LPC_TIMER1->IR = 0x1UL << 4;
		timer1_capture0++;
  	}
  	if (LPC_TIMER1->IR & (0x1UL << 5))
  	{
		/* clear interrupt flag */
		LPC_TIMER1->IR = 0x1UL << 5;
		timer1_capture1++;
  	}
}

/******************************************************************************
 ** Function name:		Timer2_IRQHandler
 **
 ** Descriptions:		Timer/Counter 2 interrupt handler
 **
 ** parameters:			None
 ** Returned value:		None
 **
 ******************************************************************************/
void TIMER2_IRQHandler (void)
{
  	if (LPC_TIMER2->IR & (0x1UL << 0))
  	{
    	/* clear interrupt flag */
		LPC_TIMER2->IR = 0x1UL << 0;
    	timer2_m0_counter++;
		kiosk_usb_task();
  	}
  	if (LPC_TIMER2->IR & (0x1UL << 1))
  	{
    	/* clear interrupt flag */
		LPC_TIMER2->IR = 0x1UL << 1;
    	timer2_m1_counter++;
  	}
  	if (LPC_TIMER2->IR & (0x1UL << 4))
  	{
		/* clear interrupt flag */
		LPC_TIMER2->IR = 0x1UL << 4;
		timer2_capture0++;
  	}
  	if (LPC_TIMER2->IR & (0x1UL << 5))
  	{
		/* clear interrupt flag */
		LPC_TIMER2->IR = 0x1UL << 5;
		timer2_capture1++;
  	}
}

/******************************************************************************
 ** Function name:		Timer3_IRQHandler
 **
 ** Descriptions:		Timer/Counter 3 interrupt handler
 **
 ** parameters:			None
 ** Returned value:		None
 **
 ******************************************************************************/
void TIMER3_IRQHandler (void)
{
  	if (LPC_TIMER3->IR & (0x1UL << 0))
  	{
    	/* clear interrupt flag */
		LPC_TIMER3->IR = 0x1UL << 0;
    	timer3_m0_counter++;
  	}
  	if (LPC_TIMER3->IR & (0x1UL << 1))
  	{
    	/* clear interrupt flag */
		LPC_TIMER3->IR = 0x1UL << 1;
    	timer3_m1_counter++;
  	}
  	if (LPC_TIMER3->IR & (0x1UL << 4))
  	{
		/* clear interrupt flag */
		LPC_TIMER3->IR = 0x1UL << 4;
		timer3_capture0++;
  	}
  	if (LPC_TIMER3->IR & (0x1UL << 5))
  	{
		/* clear interrupt flag */
		LPC_TIMER3->IR = 0x1UL << 5;
		timer3_capture1++;
  	}
}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
