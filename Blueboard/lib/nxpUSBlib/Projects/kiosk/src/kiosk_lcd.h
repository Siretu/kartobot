
/** \file
 *
 *  Header file for kiosk LCD controller routines
 */

#ifndef __KIOSK_LCD_H
#define __KIOSK_LCD_H

#include "LPC18xx.h"
#include "config.h"
#include "EMC.h"
#include "timer.h"
#include "lpc_types.h"
#include "lcd_params.h"
#include "lcd_driver.h"
#include "lpc_swim.h"
#include "lcd_type.h"

	/* Macros: */

#define KIOSK_SLIDE_COUNT 10

	/* Function Prototypes: */

		int kiosk_lcd_setup(void);
		void kiosk_lcd_set_display_pointer(int32_t displayPointer);
		void LCD_IRQHandler(void);
		void kiosk_lcd_idle_display(void);
		void kiosk_lcd_load_display(int slideNum);

#endif // __KIOSK_LCD_H

