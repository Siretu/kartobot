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
#ifndef __LCD_PARAM_H
#define __LCD_PARAM_H

#include <stdint.h>

/* LCD display types */
typedef enum {
  TFT = 0,   /* standard TFT */
  ADTFT,     /* advanced TFT */
  HRTFT,     /* highly reflective TFT */
  MONO_4BIT, /* 4-bit mono */
  MONO_8BIT, /* 8-bit mono */
  CSTN       /* color STN */
} LCD_PANEL_T;

/* Structure containing the parameters for the LCD panel */
typedef struct {
  uint8_t         h_back_porch;         /* Horizontal back porch in clocks */
  uint8_t         h_front_porch;        /* Horizontal front porch in clocks */
  uint8_t         h_sync_pulse_width;   /* HSYNC pulse width in clocks */
  uint16_t        pixels_per_line;      /* Pixels per line (horizontal resolution) */
  uint8_t         v_back_porch;         /* Vertical back porch in clocks */
  uint8_t         v_front_porch;        /* Vertical front porch in clocks */
  uint8_t         v_sync_pulse_width;   /* VSYNC pulse width in clocks */
  uint16_t        lines_per_panel;      /* Lines per panel (vertical resolution) */
  uint8_t         invert_output_enable; /* Invert output enable, 1 = invert*/
  uint8_t         invert_panel_clock;   /* Invert panel clock, 1 = invert*/
  uint8_t         invert_hsync;         /* Invert HSYNC, 1 = invert */
  uint8_t         invert_vsync;         /* Invert VSYNC, 1 = invert */
  uint8_t         ac_bias_frequency;    /* AC bias frequency in clocks */
  uint8_t         bits_per_pixel;       /* Maximum bits per pixel the display supports */
  uint32_t        optimal_clock;        /* Optimal clock rate (Hz) */
  LCD_PANEL_T     lcd_panel_type;       /* LCD panel type */
  uint8_t         dual_panel;           /* Dual panel, 1 = dual panel display */
} LCD_PARAM_T;

extern const LCD_PARAM_T sharp_lq064;
extern const LCD_PARAM_T sharp_lm057qb;
extern const LCD_PARAM_T sharp_lm057qc;

#endif /* __LCD_PARAM_H */

/**********************************************************************
 **                            End Of File
 **********************************************************************/
