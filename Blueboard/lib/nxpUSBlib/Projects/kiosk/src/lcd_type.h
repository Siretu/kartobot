/***********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: Selection of LCD Type.
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
#ifndef __LCD_TYPE_H
#define __LCD_TYPE_H

// Enumerate LCD types
#define SHARP_LM057QC	0
#define SHARP_LM057QB	1
#define SHARP_LQ064		2	/* TFT 16-bit, 1:5:5:5 mode */

// Define LCD under test
#define LCD_TYPE 		SHARP_LQ064
#define LCD_FRAME_BUF 	0x28000000
#define LCD_TEXT_FRAME_BUF (LCD_FRAME_BUF + 640 * 480 * 2 * 11)

#if (LCD_TYPE == SHARP_LM057QC)
	#define LCD_DISPLAY sharp_lm057qc
#elif (LCD_TYPE == SHARP_LM057QB)
	#define LCD_DISPLAY sharp_lm057qb
#elif (LCD_TYPE == SHARP_LQ064)
	#define LCD_DISPLAY sharp_lq064
#else
	#define LCD_DISPLAY truly_g240320ltsw
#endif

#define DISPLAY_HEIGHT 	LCD_DISPLAY.lines_per_panel
#define DISPLAY_WIDTH 	LCD_DISPLAY.pixels_per_line

#endif /* __LCD_TYPE_H */

/**********************************************************************
 **                            End Of File
 **********************************************************************/
