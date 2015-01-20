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
 *  Header file for ARP.c.
 */

#ifndef _ARP_H_
#define _ARP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include <LUFA/Drivers/USB/USB.h>

		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"

	/* Macros: */
		/** ARP header operation constant, indicating a request from a host for an address translation. */
		#define ARP_OPERATION_REQUEST            1

		/** ARP header operation constant, indicating a reply from a host giving an address translation. */
		#define ARP_OPERATION_REPLY              2

	/* Type Defines: */
		/** Type define for an ARP packet inside an Ethernet frame. */
		typedef struct
		{
			uint16_t      HardwareType; /**< Hardware type constant, indicating the hardware used */
			uint16_t      ProtocolType; /**< Protocol being resolved, usually ETHERTYPE_IPV4 */

			uint8_t       HLEN; /**< Length in bytes of the source/destination hardware addresses */
			uint8_t       PLEN; /**< Length in bytes of the source/destination protocol addresses */
			uint16_t      Operation; /**< Type of operation, either ARP_OPERATION_REQUEST or ARP_OPERATION_REPLY */

			MAC_Address_t SHA; /**< Sender's hardware address */
			IP_Address_t  SPA; /**< Sender's protocol address */
			MAC_Address_t THA; /**< Target's hardware address */
			IP_Address_t  TPA; /**< Target's protocol address */
		} ARP_Header_t;

	/* Function Prototypes: */
		int16_t ARP_ProcessARPPacket(void* InDataStart,
		                             void* OutDataStart);

#endif

