/***********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: External Memory Controller driver.
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
#ifndef __EMC_H
#define __EMC_H

/* The Hitex LPC18xx Evaluation board contains a 64Mb SDRAM with a
   16-bit data bus */
#define SDRAM_BASE_ADDR			LCD_FRAME_BUF
#define SDRAM_SIZE_BYTES		(1024UL * 1024UL * 8UL)
#define SDRAM_WIDTH				EMC_SDRAM_WIDTH_16_BITS
#define SDRAM_SIZE_MBITS		EMC_SDRAM_SIZE_64_MBITS
#define SDRAM_DATA_BUS_BITS		EMC_SDRAM_DATA_BUS_16_BITS
#define SDRAM_COL_ADDR_BITS		8

#define EMC_SDRAM_WIDTH_8_BITS		0
#define EMC_SDRAM_WIDTH_16_BITS		1
#define EMC_SDRAM_WIDTH_32_BITS		2

#define EMC_SDRAM_SIZE_16_MBITS		0
#define EMC_SDRAM_SIZE_64_MBITS		1
#define EMC_SDRAM_SIZE_128_MBITS	2
#define EMC_SDRAM_SIZE_256_MBITS	3
#define EMC_SDRAM_SIZE_512_MBITS	4

#define EMC_SDRAM_DATA_BUS_16_BITS	0
#define EMC_SDRAM_DATA_BUS_32_BITS	1

void vEMC_InitSRDRAM(uint32_t u32BaseAddr, uint32_t u32Width, uint32_t u32Size, uint32_t u32DataBus, uint32_t u32ColAddrBits);

#endif /* end __EMC_H */

/**********************************************************************
 **                            End Of File
 **********************************************************************/
