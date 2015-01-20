/*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
*         LUFA Library
* Copyright (C) Dean Camera, 2011.
*
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
* 
* Permission to use, copy, modify, and distribute this software
* and its documentation for any purpose is hereby granted without
* fee, provided that it is used in conjunction with NXP Semiconductors
* microcontrollers.  This copyright, permission, and disclaimer notice
* must appear in all copies of this code.
*/


#ifndef __BSP_INTERNAL_H
#define __BSP_INTERNAL_H

#include "bsp.h"

/**
 * BOARD DEFINE
 */
#define BOARD_LPCXpressoBase_RevB       0
#define BOARD_EA1800					1
#define BOARD_HITEX1800					2
#define BOARD_MCB1000					3
#define BOARD_MCB1700					4
#define BOARD_NGX4300					5

   
#if (BOARD == BOARD_EA1800)
	#include "EA1800/bsp_EA1800.h"
#elif (BOARD == BOARD_HITEX1800)
	#include "HITEX1800/bsp_HITEX1800.h"
#elif (BOARD == BOARD_NGX4300)
	#include "NGX4300/bsp_NGX4300.h"
#elif (BOARD == BOARD_LPCXpressoBase_RevB)
	#include "LPCXpressoBase_RevB/bsp_LPCXpressoBase_RevB.h"
#elif (BOARD == BOARD_MCB1700)
	#include "MCB1700/bsp_MCB1700.h"
#elif (BOARD == BOARD_MCB1000)
	#include "MCB1000/bsp_MCB1000.h"
#else
	#error You must chose a board profile
#endif


#endif
