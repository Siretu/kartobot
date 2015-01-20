/***********************************************************************
 * $Id$
 *
 * Project: NXP LPC1850 LCD example.
 *
 * Description: Simple External Memory Controller driver.
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
#include "LPC18xx.H"
#include "EMC.h"
#include "scu.h"

#define EMC_B_ENABLE 					(1 << 19)
#define EMC_ENABLE 						(1 << 0)
#define EMC_CE_ENABLE 					(1 << 0)
#define EMC_CS_ENABLE 					(1 << 1)
#define EMC_CLOCK_DELAYED_STRATEGY 		(0 << 0)
#define EMC_COMMAND_DELAYED_STRATEGY 	(1 << 0)
#define EMC_COMMAND_DELAYED_STRATEGY2 	(2 << 0)
#define EMC_COMMAND_DELAYED_STRATEGY3 	(3 << 0)
#define EMC_INIT(i) 					((i) << 7)
#define EMC_NORMAL 						(0)
#define EMC_MODE 						(1)
#define EMC_PRECHARGE_ALL 				(2)
#define EMC_NOP 						(3)

/**********************************************************************
 ** Function prototypes
**********************************************************************/
static void vDelay(uint32_t u32Delay);

/**********************************************************************
 ** Function name:
 **
 ** Description:
 **
 ** Parameters:
 **
 ** Returned value:
 **********************************************************************/
void vEMC_InitSRDRAM(uint32_t u32BaseAddr, uint32_t u32Width, uint32_t u32Size, uint32_t u32DataBus, uint32_t u32ColAddrBits)
{
	int i;
	
	/* Configure the pin mux */
	scu_pinmux(0x1,  7, MD_PLN, FUNC3);	/* P1_7 - EXTBUS_D0 — External memory data line 0 */
	scu_pinmux(0x1,  8, MD_PLN, FUNC3);	/* P1_8 - EXTBUS_D1 — External memory data line 1 */
	scu_pinmux(0x1,  9, MD_PLN, FUNC3);	/* P1_9 - EXTBUS_D2 — External memory data line 2 */
	scu_pinmux(0x1, 10, MD_PLN, FUNC3);	/* P1_10 - EXTBUS_D3 — External memory data line 3 */
	scu_pinmux(0x1, 11, MD_PLN, FUNC3);	/* P1_11 - EXTBUS_D4 — External memory data line 4 */
	scu_pinmux(0x1, 12, MD_PLN, FUNC3);	/* P1_12 - EXTBUS_D5 — External memory data line 5 */
	scu_pinmux(0x1, 13, MD_PLN, FUNC3);	/* P1_13 - EXTBUS_D6 — External memory data line 6 */
	scu_pinmux(0x1, 14, MD_PLN, FUNC3);	/* P1_14 - EXTBUS_D7 — External memory data line 7 */
	scu_pinmux(0x5,  4, MD_PLN, FUNC2);	/* P5_4 - EXTBUS_D8 — External memory data line 8 */
	scu_pinmux(0x5,  5, MD_PLN, FUNC2);	/* P5_5 - EXTBUS_D9 — External memory data line 9 */
	scu_pinmux(0x5,  6, MD_PLN, FUNC2);	/* P5_6 - EXTBUS_D10 — External memory data line 10 */
	scu_pinmux(0x5,  7, MD_PLN, FUNC2);	/* P5_7 - EXTBUS_D11 — External memory data line 11 */
	scu_pinmux(0x5,  0, MD_PLN, FUNC2);	/* P5_0 - EXTBUS_D12 — External memory data line 12 */
	scu_pinmux(0x5,  1, MD_PLN, FUNC2);	/* P5_1 - EXTBUS_D13 — External memory data line 13 */
	scu_pinmux(0x5,  2, MD_PLN, FUNC2);	/* P5_2 - EXTBUS_D14 — External memory data line 14 */
	scu_pinmux(0x5,  3, MD_PLN, FUNC2);	/* P5_3 - EXTBUS_D15 — External memory data line 15 */
	scu_pinmux(0xD,  2, MD_PLN, FUNC2);	/* PD_2 - EXTBUS_D16 — External memory data line 16 */
	scu_pinmux(0xD,  3, MD_PLN, FUNC2);	/* PD_3 - EXTBUS_D17 — External memory data line 17 */
	scu_pinmux(0xD,  4, MD_PLN, FUNC2);	/* PD_4 - EXTBUS_D18 — External memory data line 18 */
	scu_pinmux(0xD,  5, MD_PLN, FUNC2);	/* PD_5 - EXTBUS_D19 — External memory data line 19 */
	scu_pinmux(0xD,  6, MD_PLN, FUNC2);	/* PD_6 - EXTBUS_D20 — External memory data line 20 */
	scu_pinmux(0xD,  7, MD_PLN, FUNC2);	/* PD_7 - EXTBUS_D21 — External memory data line 21 */
	scu_pinmux(0xD,  8, MD_PLN, FUNC2);	/* PD_8 - EXTBUS_D22 — External memory data line 22 */
	scu_pinmux(0xD,  9, MD_PLN, FUNC2);	/* PD_9 - EXTBUS_D23 — External memory data line 23 */
	scu_pinmux(0xE,  5, MD_PLN, FUNC3);	/* PE_5 - EXTBUS_D24 — External memory data line 24 */
	scu_pinmux(0xE,  6, MD_PLN, FUNC3);	/* PE_6 - EXTBUS_D25 — External memory data line 25 */
	scu_pinmux(0xE,  7, MD_PLN, FUNC3);	/* PE_7 - EXTBUS_D26 — External memory data line 26 */
	scu_pinmux(0xE,  8, MD_PLN, FUNC3);	/* PE_8 - EXTBUS_D27 — External memory data line 27 */
	scu_pinmux(0xE,  9, MD_PLN, FUNC3);	/* PE_9 - EXTBUS_D28 — External memory data line 28 */
	scu_pinmux(0xE, 10, MD_PLN, FUNC3);	/* PE_10 - EXTBUS_D29 — External memory data line 29 */
	scu_pinmux(0xE, 11, MD_PLN, FUNC3);	/* PE_11 - EXTBUS_D30 — External memory data line 30 */
	scu_pinmux(0xE, 12, MD_PLN, FUNC3);	/* PE_12 - EXTBUS_D31 — External memory data line 31 */

	scu_pinmux(0x2,  9, MD_PLN, FUNC3);	/* P2_9 - EXTBUS_A0 — External memory address line 0 */
	scu_pinmux(0x2, 10, MD_PLN, FUNC3);	/* P2_10 - EXTBUS_A1 — External memory address line 1 */	
	scu_pinmux(0x2, 11, MD_PLN, FUNC3);	/* P2_11 - EXTBUS_A2 — External memory address line 2 */	
	scu_pinmux(0x2, 12, MD_PLN, FUNC3);	/* P2_12 - EXTBUS_A3 — External memory address line 3 */
	scu_pinmux(0x2, 13, MD_PLN, FUNC3);	/* P2_13 - EXTBUS_A4 — External memory address line 4 */	
	scu_pinmux(0x1,  0, MD_PLN, FUNC2);	/* P1_0 - EXTBUS_A5 — External memory address line 5 */
	scu_pinmux(0x1,  1, MD_PLN, FUNC2);	/* P1_1 - EXTBUS_A6 — External memory address line 6 */	
	scu_pinmux(0x1,  2, MD_PLN, FUNC2);	/* P1_2 - EXTBUS_A7 — External memory address line 7 */	
	scu_pinmux(0x2,  8, MD_PLN, FUNC3);	/* P2_8 - EXTBUS_A8 — External memory address line 8 */
	scu_pinmux(0x2,  7, MD_PLN, FUNC3);	/* P2_7 - EXTBUS_A9 — External memory address line 9 */	
	scu_pinmux(0x2,  6, MD_PLN, FUNC2);	/* P2_6 - EXTBUS_A10 — External memory address line 10 */
	scu_pinmux(0x2,  2, MD_PLN, FUNC2);	/* P2_2 - EXTBUS_A11 — External memory address line 11 */
	scu_pinmux(0x2,  1, MD_PLN, FUNC2);	/* P2_1 - EXTBUS_A12 — External memory address line 12 */
	scu_pinmux(0x2,  0, MD_PLN, FUNC2);	/* P2_0 - EXTBUS_A13 — External memory address line 13 */	
	scu_pinmux(0x6,  8, MD_PLN, FUNC1);	/* P6_8 - EXTBUS_A14 — External memory address line 14 */
	scu_pinmux(0x6,  7, MD_PLN, FUNC1);	/* P6_7 - EXTBUS_A15 — External memory address line 15 */	
	scu_pinmux(0xD, 16, MD_PLN, FUNC2);	/* PD_16 - EXTBUS_A16 — External memory address line 16 */
	scu_pinmux(0xD, 15, MD_PLN, FUNC2);	/* PD_15 - EXTBUS_A17 — External memory address line 17 */	
	scu_pinmux(0xE,  0, MD_PLN, FUNC3);	/* PE_0 - EXTBUS_A18 — External memory address line 18 */
	scu_pinmux(0xE,  1, MD_PLN, FUNC3);	/* PE_1 - EXTBUS_A19 — External memory address line 19 */
	scu_pinmux(0xE,  2, MD_PLN, FUNC3);	/* PE_2 - EXTBUS_A20 — External memory address line 20 */
	scu_pinmux(0xE,  3, MD_PLN, FUNC3);	/* PE_3 - EXTBUS_A21 — External memory address line 21 */
	scu_pinmux(0xE,  4, MD_PLN, FUNC3);	/* PE_4 - EXTBUS_A22 — External memory address line 22 */	
	scu_pinmux(0xA,  4, MD_PLN, FUNC3);	/* PA_4 - EXTBUS_A23 — External memory address line 23 */
		
	scu_pinmux(0x1,  4, MD_PLN, FUNC3);	/* P1_4 - EXTBUS_BLS0 — LOW active Byte Lane select signal 0 */
	scu_pinmux(0x6,  6, MD_PLN, FUNC1);	/* P6_6 - EXTBUS_BLS1 — LOW active Byte Lane select signal 1 */	
	scu_pinmux(0xD, 13, MD_PLN, FUNC2);	/* PD_13 - EXTBUS_BLS2 — LOW active Byte Lane select signal 2 */
	scu_pinmux(0xD, 10, MD_PLN, FUNC2);	/* PD_10 - EXTBUS_BLS3 — LOW active Byte Lane select signal 3 */		
	
	scu_pinmux(0x1,  5, MD_PLN, FUNC3);	/* P1_5 - EXTBUS_CS0 — LOW active Chip Select 0 signal */	
	scu_pinmux(0x6,  3, MD_PLN, FUNC3);	/* P6_3 - EXTBUS_CS1 — LOW active Chip Select 1 signal */		
	scu_pinmux(0xD, 12, MD_PLN, FUNC2);	/* PD_12 - EXTBUS_CS2 — LOW active Chip Select 2 signal */			
	scu_pinmux(0xD, 11, MD_PLN, FUNC2);	/* PD_11 - EXTBUS_CS3 — LOW active Chip Select 3 signal */			

	scu_pinmux(0x1,  3, MD_PLN, FUNC3);	/* P1_3 - EXTBUS_OE — LOW active Output Enable signal */			
	scu_pinmux(0x1,  6, MD_PLN, FUNC3);	/* P1_6 - EXTBUS_WE — LOW active Write Enable signal */			

	scu_pinmux(0x6, 11, MD_PLN, FUNC3);	/* P6_11 - EXTBUS_CKEOUT0 — SDRAM clock enable 0 */			
	scu_pinmux(0x6,  2, MD_PLN, FUNC1);	/* P6_2 - EXTBUS_CKEOUT1 — SDRAM clock enable 1 */			
	scu_pinmux(0xD,  1, MD_PLN, FUNC2);	/* PD_1 - EXTBUS_CKEOUT2 — SDRAM clock enable 2 */
	scu_pinmux(0xE, 15, MD_PLN, FUNC3);	/* PE_15 - EXTBUS_CKEOUT3 — SDRAM clock enable 3 */

	scu_pinmux(0x6, 12, MD_PLN, FUNC3);	/* P6_12 - EXTBUS_DQMOUT0 — Data mask 0 used with SDRAM and static devices */	
	scu_pinmux(0x6, 10, MD_PLN, FUNC3);	/* P6_10 - EXTBUS_DQMOUT1 — Data mask 1 used with SDRAM and static devices */	
	scu_pinmux(0xD,  0, MD_PLN, FUNC2);	/* PD_0 - EXTBUS_DQMOUT2 — Data mask 2 used with SDRAM and static devices */	
	scu_pinmux(0xE, 13, MD_PLN, FUNC3);	/* PE_13 - EXTBUS_DQMOUT3 — Data mask 3 used with SDRAM and static devices */	

	scu_pinmux(0x6,  9, MD_PLN, FUNC3);	/* P6_9 - EXTBUS_DYCS0 — SDRAM chip select 0 */		
	scu_pinmux(0x6,  1, MD_PLN, FUNC1);	/* P6_1 - EXTBUS_DYCS1 — SDRAM chip select 1 */
	scu_pinmux(0xD, 14, MD_PLN, FUNC2);	/* PD_14 - EXTBUS_DYCS2 — SDRAM chip select 2 */	
	scu_pinmux(0xE, 14, MD_PLN, FUNC3);	/* PE_14 - EXTBUS_DYCS3 — SDRAM chip select 3 */

	scu_pinmux(0x6,  4, MD_PLN, FUNC3);	/* P6_4 - EXTBUS_CAS — LOW active SDRAM Column Address Strobe */	
	scu_pinmux(0x6,  5, MD_PLN, FUNC3);	/* P6_5 - EXTBUS_RAS — LOW active SDRAM Row Address Strobe */	
			
	LPC_SCU->SFSCLK_0 = 0;	/* CLK0 - EXTBUS_CLK0 — SDRAM clock 0 */
	LPC_SCU->SFSCLK_1 = 0;	/* CLK1 - EXTBUS_CLK1 — SDRAM clock 1 */
	LPC_SCU->SFSCLK_2 = 0;  /* CLK2 - EXTBUS_CLK2 — SDRAM clock 2 */
	LPC_SCU->SFSCLK_3 = 0;	/* CLK3 - EXTBUS_CLK3 — SDRAM clock 3 */

   /* Initialize EMC to interface with SDRAM */
	LPC_EMC->CONTROL 			= 0x00000001;   /* Enable the external memory controller */	
	LPC_EMC->CONFIG 			= 0;

	for(i = 0; i < 0x28; i += 4)
	{
		 //zero all delays
		*(uint32_t*)(0x40086d00 + i) = 0;
	}

	LPC_EMC->DYNAMICCONFIG0 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	LPC_EMC->DYNAMICCONFIG1 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	LPC_EMC->DYNAMICCONFIG2 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	LPC_EMC->DYNAMICCONFIG3 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));

	LPC_EMC->DYNAMICRASCAS0 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS1 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS2 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS3 	= (3UL << 0) | (3UL << 8);
	
	LPC_EMC->DYNAMICREADCONFIG	= EMC_COMMAND_DELAYED_STRATEGY;
	
	LPC_EMC->DYNAMICRP 			= 15;
	LPC_EMC->DYNAMICRAS 		= 15;
	LPC_EMC->DYNAMICSREX 		= 15;
	LPC_EMC->DYNAMICAPR 		= 15;
	LPC_EMC->DYNAMICDAL 		= 15;
	LPC_EMC->DYNAMICWR 			= 15;
	LPC_EMC->DYNAMICRC 			= 31;
	LPC_EMC->DYNAMICRFC 		= 31;
	LPC_EMC->DYNAMICXSR 		= 31;
	LPC_EMC->DYNAMICRRD 		= 15;
	LPC_EMC->DYNAMICMRD 		= 15;
	
	LPC_EMC->DYNAMICCONTROL 	= EMC_CE_ENABLE | EMC_CS_ENABLE | EMC_INIT(EMC_NOP);
	vDelay(100);
	
	LPC_EMC->DYNAMICCONTROL 	= EMC_CE_ENABLE | EMC_CS_ENABLE | EMC_INIT(EMC_PRECHARGE_ALL);
	LPC_EMC->DYNAMICREFRESH 	= 2;
	vDelay(100);
	
	LPC_EMC->DYNAMICREFRESH 	= 50; 
	LPC_EMC->DYNAMICRASCAS0 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS1 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS2 	= (3UL << 0) | (3UL << 8);
	LPC_EMC->DYNAMICRASCAS3 	= (3UL << 0) | (3UL << 8);
	
	LPC_EMC->DYNAMICCONFIG0 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14)); 
	LPC_EMC->DYNAMICCONFIG1 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	LPC_EMC->DYNAMICCONFIG2 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	LPC_EMC->DYNAMICCONFIG3 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14));
	
	LPC_EMC->DYNAMICCONTROL 	= EMC_CE_ENABLE | EMC_CS_ENABLE | EMC_INIT(EMC_MODE);
	
	if(u32DataBus == 0) 
	{
		/* burst size 8 */
		*((volatile uint32_t *)(u32BaseAddr | ((3UL | (3UL << 4)) << (u32ColAddrBits + 1))));
	}
	else 
	{
		/* burst size 4 */
		*((volatile uint32_t *)(u32BaseAddr | ((2UL | (3UL << 4)) << (u32ColAddrBits + 2))));
	}

	LPC_EMC->DYNAMICCONTROL 	= 0;
	LPC_EMC->DYNAMICCONFIG0 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14)) | EMC_B_ENABLE;
	LPC_EMC->DYNAMICCONFIG1 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14)) | EMC_B_ENABLE;
	LPC_EMC->DYNAMICCONFIG2 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14)) | EMC_B_ENABLE;
	LPC_EMC->DYNAMICCONFIG3 	= ((u32Width << 7) | (u32Size << 9) | (1UL << 12) | (u32DataBus << 14)) | EMC_B_ENABLE;
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
static void vDelay(uint32_t u32Delay)
{
	volatile uint32_t i;
	
	for(i = 0; i < (u32Delay * 10000); i++);
}

/**********************************************************************
 **                            End Of File
 **********************************************************************/
