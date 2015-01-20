/***********************************************************************
 * $Id: scu.h 5553 2010-11-09 22:14:19Z nxp27266 $
 *
 * Project: LPC18xx Common
 *
 * Description:
 *     Header file for PINMUX configuration
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
#ifndef __SCU_H 
#define __SCU_H

#define PORT_OFFSET 0x80
#define PIN_OFFSET  0x04

/* Pin modes */
#define MD_PLN  (0x2<<2)
#define MD_PUP  (0x0<<2)
#define MD_PDN  (0x3<<2)
#define MD_BUK  (0x1<<2)

/* Pin function */
#define FUNC0 0x0
#define FUNC1 0x1
#define FUNC2 0x2
#define FUNC3 0x3

extern void scu_pinmux(unsigned port, unsigned pin, unsigned mode, unsigned func);

 

#endif /* end __SCU_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
