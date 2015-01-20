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

#ifndef _IP_H_
#define _IP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"

	/* Macros: */
		/** Protocol IP address of the host (client) machine, once assigned by DHCP. */
		#define CLIENT_IP_ADDRESS                { 10,     0,    0,    1}

		/** Protocol IP address of the virtual server machine. */
		#define SERVER_IP_ADDRESS                { 10,     0,    0,    2}

		/** Protocol IP address of the broadcast address. */
		#define BROADCAST_IP_ADDRESS             {0xFF, 0xFF, 0xFF, 0xFF}

		/** Default Time To Live (TTL) value for sent packets, indicating the maximum allowable hops until their destination
		 *  is reached.
		 */
		#define DEFAULT_TTL                      128

		/** Performs a comparison between two IP addresses, indicating if they are identical.
		 *
		 *  \param[in] IP1  First IP address
		 *  \param[in] IP2  Second IP address
		 *
		 *  \return True if the addresses match, false otherwise
		 */
		#define IP_COMPARE(IP1, IP2)             (memcmp(IP1, IP2, sizeof(IP_Address_t)) == 0)

	/* Type Defines: */
		/** Type define of an IP packet header. */
		typedef struct
		{
			unsigned     HeaderLength   : 4; /**< Total length of the packet header, in 4-byte blocks */
			unsigned     Version        : 4; /**< IP protocol version */
			uint8_t      TypeOfService; /**< Special service type identifier, indicating delay/throughput/reliability levels */
			uint16_t     TotalLength; /**< Total length of the IP packet, in bytes */

			uint16_t     Identification; /**< Identification value for identifying fragmented packets */
			unsigned     FragmentOffset : 13; /**< Offset of this IP fragment */
			unsigned     Flags          : 3; /**< Fragment flags, to indicate if a packet is fragmented */

			uint8_t      TTL; /**< Maximum allowable number of hops to reach the packet destination */
			uint8_t      Protocol; /**< Encapsulated protocol type */
			uint16_t     HeaderChecksum; /**< Ethernet checksum of the IP header */

			IP_Address_t SourceAddress; /**< Source protocol IP address of the packet */
			IP_Address_t DestinationAddress; /**< Destination protocol IP address of the packet */
		} IP_Header_t;

	/* Function Prototypes: */
		int16_t IP_ProcessIPPacket(Ethernet_Frame_Info_t* const FrameIN,
		                           void* InDataStart,
		                           void* OutDataStart);

#endif

