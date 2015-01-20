#include "kiosk_lcd.h"
#include "emc.h"
#include "lpc_swim.h"
#include "lpc_winfreesystem14x16.h"
#include "lpc_swim_font.h"

/**********************************************************************
 ** Local variables
 **********************************************************************/
static int32_t dev_lcd;
SWIM_WINDOW_T win1;

void kiosk_lcd_load_display(int slideNum)
{
	char text[64];

	sprintf(text, "Loading slide %d", slideNum);
	swim_clear_screen(&win1, 0);
	swim_set_xy(&win1, 640/2 - 50, 480/2 - 10);
    swim_put_text(&win1, text);

	kiosk_lcd_set_display_pointer(LCD_TEXT_FRAME_BUF);

}

void kiosk_lcd_idle_display(void)
{
	swim_clear_screen(&win1, 0);
	swim_set_xy(&win1, 640/2 - 50, 480/2 - 10);
    swim_put_text(&win1, "Insert Flash Drive");

	kiosk_lcd_set_display_pointer(LCD_TEXT_FRAME_BUF);

}

int kiosk_lcd_setup(void)
{
  	/* Open LCD */
  	if ((dev_lcd = lcd_open((int32_t)&LCD_DISPLAY)) == 0x0)
  	{
    	/* Error opening the device */
    	return 0;
  	}

	/* Set LCD display pointer */
	kiosk_lcd_set_display_pointer(LCD_TEXT_FRAME_BUF);

  	/* Enable LCD */
  	lcd_ioctl(dev_lcd, LCD_ENABLE, 1);

  	/* Enable CLCDC Interrupt */
  	//lcd_ioctl(dev_lcd, LCD_SET_INTERRUPT, CLCDC_LCD_INTERRUPT_FUF);
  	//lcd_ioctl(dev_lcd, LCD_SET_INTERRUPT, CLCDC_LCD_INTERRUPT_LNBU);
  	//lcd_ioctl(dev_lcd, LCD_SET_INTERRUPT, CLCDC_LCD_INTERRUPT_VCOMP);
  	//lcd_ioctl(dev_lcd, LCD_SET_INTERRUPT, CLCDC_LCD_INTERRUPT_MBERROR);

  	/* Turn on LCD */
  	lcd_ioctl(dev_lcd, LCD_PWR_ON, 1);

  	/* Create a SWIM window */
    swim_window_open(&win1, DISPLAY_WIDTH,
        DISPLAY_HEIGHT, (COLOR_T *)LCD_TEXT_FRAME_BUF, 0, 0,
		(DISPLAY_WIDTH - 1),
		(DISPLAY_HEIGHT - 1),1, WHITE, BLACK, BLACK);

    swim_set_font(&win1, &font_winfreesys14x16);

  	return dev_lcd;
}

void kiosk_lcd_set_display_pointer(int32_t displayPointer)
{
  	/* Disable LCD */
  	//lcd_ioctl(dev_lcd, LCD_ENABLE, 0);
	/* Set LCD display pointer */
  	lcd_ioctl(dev_lcd, LCD_SET_BUFFER, displayPointer);
  	/* Enable LCD */
  	//lcd_ioctl(dev_lcd, LCD_ENABLE, 1);
}

/**********************************************************************
 ** Function name:
 **
 ** Description:
 **
 ** Parameters:
 **
 ** Returned value:
 **********************************************************************/
void LCD_IRQHandler(void)
{
	int32_t status;

  	status = lcd_ioctl(dev_lcd, LCD_GET_STATUS, LCD_GET_INTERRUPT);

  	if (status & CLCDC_LCD_INTERRUPT_FUF)
  	{
    	lcd_ioctl(dev_lcd, LCD_CLR_INTERRUPT, CLCDC_LCD_INTERRUPT_FUF);
  	}
  	if (status & CLCDC_LCD_INTERRUPT_LNBU)
  	{
    	lcd_ioctl(dev_lcd, LCD_CLR_INTERRUPT, CLCDC_LCD_INTERRUPT_LNBU);
  	}
  	if (status & CLCDC_LCD_INTERRUPT_VCOMP)
  	{
    	lcd_ioctl(dev_lcd, LCD_CLR_INTERRUPT, CLCDC_LCD_INTERRUPT_VCOMP);
  	}
  	if (status & CLCDC_LCD_INTERRUPT_MBERROR)
  	{
    	lcd_ioctl(dev_lcd, LCD_CLR_INTERRUPT, CLCDC_LCD_INTERRUPT_MBERROR);
  	}
}

