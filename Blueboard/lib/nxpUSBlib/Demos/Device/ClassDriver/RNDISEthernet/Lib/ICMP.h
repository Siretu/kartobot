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
 *  Header file for ICMP.c.
 */

#ifndef _ICMP_H_
#define _ICMP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"

	/* Macros: */
		/** ICMP message type constant, indicating an ICMP ECHO Reply message. */
		#define ICMP_TYPE_ECHOREPLY              0

		/** ICMP message type constant, indicating a packet destination is unreachable. */
		#define ICMP_TYPE_DESTINATIONUNREACHABLE 3

		/** ICMP message type constant, indicating an ICMP Source Quench message. */
		#define ICMP_TYPE_SOURCEQUENCH           4

		/** ICMP message type constant, indicating an ICMP Redirect message. */
		#define ICMP_TYPE_REDIRECTMESSAGE        5

		/** ICMP message type constant, indicating an ICMP ECHO Request message. */
		#define ICMP_TYPE_ECHOREQUEST            8

		/** ICMP message type constant, indicating an ICMP Time Exceeded message. */
		#define ICMP_TYPE_TIMEEXCEEDED           11

	/* Type Defines: */
		/** Type define for an ICMP message header. */
		typedef struct
		{
			uint8_t       Type; /**< ICMP message type, a ICMP_TYPE_* constant */
			uint8_t       Code; /**< ICMP message code, indicating the message value */
			uint16_t      Checksum; /**< Ethernet checksum of the ICMP message */
			uint16_t      Id; /**< Id of the ICMP message */
			uint16_t      Sequence; /**< Sequence number of the ICMP message, to link together message responses */
		} ICMP_Header_t;

	/* Function Prototypes: */
		int16_t ICMP_ProcessICMPPacket(Ethernet_Frame_Info_t* const FrameIN,
		                               void* InDataStart,
		                               void* OutDataStart);

#endif

