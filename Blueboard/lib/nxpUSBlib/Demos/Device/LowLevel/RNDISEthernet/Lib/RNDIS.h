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


/** \file
 *
 *  Header file for RNDIS.c.
 */

#ifndef _RNDIS_H_
#define _RNDIS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "RNDISEthernet.h"
		#include "Ethernet.h"

	/* External Variables: */
		extern uint8_t                 RNDISMessageBuffer[];
		extern RNDIS_Message_Header_t* MessageHeader;
		extern bool                    ResponseReady;
		extern uint8_t                 CurrRNDISState;

	/* Function Prototypes: */
		void ProcessRNDISControlMessage(void);

		#if defined(INCLUDE_FROM_RNDIS_C)
			static bool ProcessNDISQuery(const uint32_t OId,
			                             void* QueryData,
			                             uint16_t QuerySize,
			                             void* ResponseData,
			                             uint16_t* ResponseSize);
			static bool ProcessNDISSet(const uint32_t OId,
			                           void* SetData,
			                           uint16_t SetSize);
		#endif

#endif

