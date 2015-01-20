/**********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: Draws color bars on the LCD. Requires RGB565 mode.
 *
 * Copyright(C) 2011, NXP Semiconductor
 * All rights reserved.
 *
 **********************************************************************
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
 *********************************************************************/

#include "lpc_types.h"
#include "lpc_swim.h"
#include "lpc_swim_font.h"
#include "lpc_swim.h"
#include "lpc_rom8x16.h"
#include "lpc_winfreesystem14x16.h"
#include "lpc_x6x13.h"

#include "lcd_params.h"      /* LCD panel parameters */
//#include "type.h"
#include "timer.h"
#include "lcd_type.h"
#include "kiosk_usb.h"

extern long dev_lcd ;
extern volatile uint32_t timer0_counter;

#define FONT  font_x6x13 // font_rom8x16 font_rom8x16 font_winfreesys14x16

/***********************************************************************
 *
 * Function: make_number_str
 *
 * Purpose: Convert a number to a positive decimal string
 *
 * Processing:
 *     Using successive division, compute the base10 value of a number
 *     into a string and return the string value to the caller.
 *
 * Parameters:
 *     str      : Where to place the generated strinjg
 *     iteration: Number to generate a string from
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void make_number_str(CHAR *str,
                     INT_32 iteration)
{
    UNS_8 tmp[32];
    INT_32 dvv1, dvv2;
    INT_32 cnt = 0;

    /* Number 0 in string */
    str[cnt] = '0';

    /* Main loop */
    while (iteration > 0)
    {
        dvv1 = iteration / 10;
        dvv2 = dvv1 * 10;
        dvv1 = iteration - dvv2;

        tmp[cnt] = dvv1;
        iteration = iteration / 10;
        cnt++;
    }

    if (cnt > 0)
    {
        for (dvv1 = 0; dvv1 < cnt; dvv1++)
        {
            str[dvv1] = (CHAR) ('0' + tmp[cnt - (dvv1 + 1)]);
        }
    }
    
    str[cnt + 1] = '\0';
}

/**********************************************************************
 *
 * Function: lcd_colorbars
 *
 * Purpose: Draw colorbars on LCD panel
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 *********************************************************************/
void lcd_colorbars(void)
{
    SWIM_WINDOW_T win1;
    COLOR_T clr, *fblog;
    CHAR str[32];
	int idx;
    UNS_16 xgs, ygs, curx, cury, curym, xidx;
    int last=timer0_counter;	
	int oldballx,oldbally,ballx,bally,balldx,balldy;

    /* Set LCD frame buffer address */
	fblog = (COLOR_T *)LCD_FRAME_BUF;

    /* Create a SWIM window */
    swim_window_open(&win1, DISPLAY_WIDTH,
        DISPLAY_HEIGHT, fblog, 0, 0,
		(DISPLAY_WIDTH - 1), 
		(DISPLAY_HEIGHT - 1),1, WHITE, BLACK, BLACK);

    /* Compute vertical size for 3 color bars */
    ygs = DISPLAY_HEIGHT / 3;

    /* Draw Red bars */
    cury = 0;											// Start at 
    curx = 0;											// 0,0 position
    curym = ygs - 1;									// End of this color bar is 1/3 of panel 
    xgs = DISPLAY_WIDTH / RED_COLORS; 	// Divide pixels/line by # of red colors possible in this mode (32 shades red in RGB565)
    clr = BLACK;										// start with black then increase to full color across the line
    for (xidx = 0; xidx < RED_COLORS; xidx++)
    {
        swim_set_pen_color(&win1, clr);
        for (idx = 0; idx <= xgs; idx++)
        {
            swim_put_line(&win1, curx, cury, curx, curym);	// Draw line
            curx++;
        }
        clr = clr + 0x0001;		// increment color value for a gradient, Red=bits[14:10] in memory (RGB1:5:5:5)
    }

    /* Draw green bars */
    cury = cury + ygs;									// Next bar starts here (y offset) 
    curx = 0;											// 
    curym = cury + (ygs - 1);							// Ends here 
    xgs = DISPLAY_WIDTH / GREEN_COLORS;	// 
    clr = BLACK;										// gradient from black to greem maximum
    for (xidx = 0; xidx < GREEN_COLORS; xidx++)
    {
        swim_set_pen_color(&win1, clr);
        for (idx = 0; idx <= xgs; idx++)
        {
            swim_put_line(&win1, curx, cury, curx, curym);
            curx++;
        }
        clr = clr + 0x0020;		// incement green color value for a grdient, Green=bits[9:5] in memory (RGB1:5:5:5 mode)
    }

    /* Draw blue bars */
    cury = cury + ygs;
    curx = 0;
    curym = cury + (ygs - 1);
    xgs = DISPLAY_WIDTH / BLUE_COLORS;	// 
    clr = BLACK;
    for (xidx = 0; xidx < BLUE_COLORS; xidx++)
    {
        swim_set_pen_color(&win1, clr);
        for (idx = 0; idx <= xgs; idx++)
        {
            swim_put_line(&win1, curx, cury, curx, curym);
            curx++;
        }
        clr = clr + 0x0400;		// incement blue color value for a gradient, Blue=bits[4:0] in memory	(RGB1:5:5:5 mode)
    }

  /* select the font to use */
  swim_set_font(&win1, (FONT_T *)&FONT );

  /* set the pen color to use */
  swim_set_pen_color(&win1, WHITE);

  /* Add a title bar */
  swim_set_title(&win1, "NXP SWIM Graphics Library", BLACK);

  /* set the location to write text */
  swim_set_xy(&win1, 60, 160);

  /* set the pen color to use */
  swim_set_pen_color(&win1, LIGHTGRAY);

  /* put the timer tick on the panel */
  oldballx = ballx = 0;
  oldbally = bally = 0;
  balldx = balldy = 9;

  while(1){
	  kiosk_usb_task();
  	if(timer0_counter != last)	
	{		
		ballx += balldx;
		if(ballx >= win1.xvsize)
			balldx*=-1,ballx+=balldx;
		if(ballx < 0)
			balldx*=-1,ballx+=balldx;

		bally += balldy;
		if(bally >= win1.yvsize)
			balldy*=-1,bally+=balldy;
		if(bally < 0)
			balldy*=-1,bally+=balldy;

		swim_set_pen_color(&win1, BLACK);
		swim_set_fill_color(&win1, BLACK);
		swim_put_diamond(&win1, oldballx, oldbally, 7, 7);
		swim_set_pen_color(&win1, WHITE);
		swim_set_fill_color(&win1, RED);
		swim_put_diamond(&win1, ballx, bally, 7, 7);

		oldballx = ballx;
		oldbally = bally;	
        
		swim_set_xy(&win1, 0, 0);
        swim_put_text(&win1, "Tick #");
        make_number_str(str, timer0_counter);
        swim_put_text(&win1, str);
        swim_put_text(&win1, "\n");

		last = timer0_counter;
	}
  }	    
}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
