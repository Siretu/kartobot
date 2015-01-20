/***********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: Main application.
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
#include "config.h"
#include "EMC.h"
#include "timer.h"
#include "lpc_types.h"
#include "lcd_params.h"
#include "lcd_driver.h"
#include "lpc_swim.h"
#include "lcd_type.h"

#include "kiosk_usb.h"
#include "kiosk_lcd.h"
#include "timer.h"
#include "adc.h"

/**********************************************************************
 ** Function prototypes
 **********************************************************************/
extern void lcd_colorbars(void);

/**********************************************************************
 ** Local variables
 **********************************************************************/

/**********************************************************************
 ** Function name:
 **
 ** Description:
 **
 ** Parameters:
 **
 ** Returned value:
 **********************************************************************/
typedef enum
{
	KIOSK_SLIDES_LOADED,
	KIOSK_SLIDES_UNLOADED,
}KIOSK_STATE;

KIOSK_STATE kioskState = KIOSK_SLIDES_UNLOADED;

int main (void)
{

	SystemInit();

#if USE_XTAL // Select XTAL or IRC in config.h
 	/* Set the XTAL oscillator frequency to 12MHz*/
	SetClock(XTAL, (CLKSRC_Type)XTAL_FREQ, DIV1); 
	/* Set PL160M */
	SetPL160M(SRC_XTAL, 7); 							
	/* Run base M3 clock from PL160M, no division */
	SetClock(BASE_M3_CLK, SRC_PL160M_0, DIV1);		
	/* Show base out clock on output */
	SetClock(BASE_OUT_CLK, SRC_XTAL, DIV1);			
#else // USE IRC
	/* Set PL160M */
	SetPL160M(SRC_IRC, 6);							
	/* Run base M3 clock from PL160M, div by 1 = no division */
	SetClock(BASE_M3_CLK, SRC_PL160M_0, DIV1);
	/* Show base out clock on output */
	SetClock(BASE_OUT_CLK, SRC_IRC, DIV1);			
#endif

	/* Initialize ADC  */
	ADCInit( ADC_CLK );

	SetClock(BASE_LCD_CLK, SRC_PL160M_0, DIV1);

  	/* Configure the external memory controller for SDRAM */
	vEMC_InitSRDRAM(SDRAM_BASE_ADDR, SDRAM_WIDTH, SDRAM_SIZE_MBITS, SDRAM_DATA_BUS_BITS, SDRAM_COL_ADDR_BITS);

	/* 84 MHz / 840 ticks = 10 kHz */
	u32Timer_Init(2, 0, 8400UL);

	/* 84 MHz / 840000 ticks = 100 Hz */
	u32Timer_Init(1, 0, 840000UL);

  	kiosk_usb_setup();

//	vTimer_Enable(2); // timer 0 calls the USB stack

  	kiosk_lcd_setup();

  	/* Enable LCD interrupts */
  	NVIC_EnableIRQ(LCD_IRQn);

	if (!kioskUsbDeviceAttached)
		kiosk_lcd_idle_display();

	while(1)
	{
		switch(kioskState)
		{
		case KIOSK_SLIDES_UNLOADED:
			if (kioskUsbDeviceAttached)
			{
				if (!kiosk_usb_ready())
					break;

				kiosk_usb_load_slides();

				kiosk_usb_run_slides();

				kioskState = KIOSK_SLIDES_LOADED;
			}
			break;
		case KIOSK_SLIDES_LOADED:
			if (!kioskUsbDeviceAttached)
			{
				kiosk_usb_stop_slides();

				kioskState = KIOSK_SLIDES_UNLOADED;
			}
			break;
		}
		kiosk_usb_task();

	}

}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
