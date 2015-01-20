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
 *  Header file for IP.c.
 */

#ifndef _UDP_H_
#define _UDP_H_

	/* Includes: */
		#include <avr/io.h>

		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
		#include "DHCP.h"

	/* Macros: */
		/** Source UDP port for a DHCP request. */
		#define UDP_PORT_DHCP_REQUEST 67

		/** Destination UDP port for a DHCP reply. */
		#define UDP_PORT_DHCP_REPLY   68

	/* Type Defines: */
		/** Type define for a UDP packet header. */
		typedef struct
		{
			uint16_t SourcePort; /**< Packet source port */
			uint16_t DestinationPort; /**< Packet destination port */
			uint16_t Length; /**< Total packet length, in bytes */
			uint16_t Checksum; /**< Optional UDP packet checksum */
		} UDP_Header_t;

	/* Function Prototypes: */
		int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart,
		                             void* UDPHeaderInStart,
		                             void* UDPHeaderOutStart);

#endif

