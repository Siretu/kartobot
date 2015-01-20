/*********************************************************************** 
 * $Id$
 * 
 * Project: NXP LPC1850 LCD example.
 * 
 * Description: LCD Parameters
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
#include "lcd_params.h"

const LCD_PARAM_T sharp_lq064 =
{
  89,       /* Horizontal back porch */
  50,       /* Horizontal front porch */
  14,       /* HSYNC pulse width */
  640,      /* Pixels per line */
  18,       /* Vertical back porch */
  94,       /* Vertical front porch */
  17,       /* VSYNC pulse width */
  480,      /* Lines per panel */
  0,        /* Invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  25180000, /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type */
  0,        /* Single panel display */
};

/* Sharp LM057QB STN display */
const LCD_PARAM_T sharp_lm057qb =
{
  5,          /* Horizontal back porch */
  5,          /* Horizontal front porch */
  3,          /* HSYNC pulse width */
  320,        /* Pixels per line */    
  1,          /* Vertical back porch */
  1,          /* Vertical front porch */
  1,          /* VSYNC pulse width */
  240,        /* Lines per panel */  
  0,          /* Do not invert output enable (NA) */
  0,          /* Do not invert panel clock */
  0,          /* Do not invert HSYNC */
  0,          /* Do not invert VSYNC */
  1,          /* AC bias frequency */
  4,          /* Bits per pixel */
  4500000,    /* Optimal clock rate (Hz) */
  MONO_4BIT,  /* LCD panel type */
  0,          /* Dual panel display */
};

/* Sharp LM057QC STN display */
const LCD_PARAM_T sharp_lm057qc =
{
  5,          /* Horizontal back porch */
  5,          /* Horizontal front porch */
  3,          /* HSYNC pulse width */
  320,        /* Pixels per line */      
  0,          /* Vertical back porch */
  0,          /* Vertical front porch */
  1,          /* VSYNC pulse width */
  240,        /* Lines per panel */        
  0,          /* Do not invert output enable (NA) */
  0,          /* Do not invert panel clock */
  0,          /* Do not invert HSYNC */
  0,          /* Do not invert VSYNC */
  0,          /* AC bias frequency */  
  8,          /* Bits per pixel */
  2500000,    /* Optimal clock rate (Hz) */  
  CSTN,       /* LCD panel type */
  0,          /* Dual panel display */
};

/**********************************************************************
 **                            End Of File
 **********************************************************************/
