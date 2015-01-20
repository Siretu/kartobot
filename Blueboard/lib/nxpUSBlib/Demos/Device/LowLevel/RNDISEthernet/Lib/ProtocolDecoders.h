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
 *  Header file for ProtocolDecoders.c.
 */

#ifndef _PROTOCOL_DECODERS_H_
#define _PROTOCOL_DECODERS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#include <LUFA/Drivers/Peripheral/Serial.h>

		#include "EthernetProtocols.h"
		#include "Ethernet.h"

	/* Function Prototypes: */
		void DecodeEthernetFrameHeader(void* InDataStart);
		void DecodeARPHeader(void* InDataStart);
		void DecodeIPHeader(void* InDataStart);
		void DecodeICMPHeader(void* InDataStart);
		void DecodeTCPHeader(void* InDataStart);
		void DecodeUDPHeader(void* InDataStart);
		void DecodeDHCPHeader(void* InDataStart);

#endif

