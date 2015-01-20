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
 *  Header file for Ethernet.c.
 */

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include "EthernetProtocols.h"
		#include "ProtocolDecoders.h"
		#include "ICMP.h"
		#include "TCP.h"
		#include "UDP.h"
		#include "DHCP.h"
		#include "ARP.h"
		#include "IP.h"

	/* Macros: */
		/** Physical MAC address of the USB RNDIS network adapter. */
		#define ADAPTER_MAC_ADDRESS              {0x02, 0x00, 0x02, 0x00, 0x02, 0x00}

		/** Physical MAC address of the virtual server on the network. */
		#define SERVER_MAC_ADDRESS               {0x00, 0x01, 0x00, 0x01, 0x00, 0x01}

		/** Physical MAC address of the network broadcast address. */
		#define BROADCAST_MAC_ADDRESS            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

		/** Performs a comparison between two MAC addresses, indicating if they are identical.
		 *
		 *  \param[in] MAC1  First MAC address
		 *  \param[in] MAC2  Second MAC address
		 *
		 *  \return True if the addresses match, false otherwise
		 */
		#define MAC_COMPARE(MAC1, MAC2)          (memcmp(MAC1, MAC2, sizeof(MAC_Address_t)) == 0)

		/** Maximum size of an incoming or outgoing Ethernet frame in bytes. */
		#define ETHERNET_FRAME_SIZE_MAX          1500

		/** Minimum size of an Ethernet packet in bytes, to conform to the Ethernet V2 packet standard. */
		#define ETHERNET_VER2_MINSIZE            0x0600

		/** Return value for all sub protocol handling routines, indicating that no response packet has been generated. */
		#define NO_RESPONSE                      0

		/** Return value for all sub protocol handling routines, indicating that the packet has not yet been handled. */
		#define NO_PROCESS                       -1

	/* Type Defines: */
		/** Type define for an Ethernet frame header. */
		typedef struct
		{
			MAC_Address_t Destination; /**< Physical MAC address of the packet recipient */
			MAC_Address_t Source; /**< Physics MAC address of the packet source */
			uint16_t      EtherType; /**< Ethernet packet sub-protocol type, for Ethernet V2 packets */
		} Ethernet_Frame_Header_t;

	/* External Variables: */
		extern Ethernet_Frame_Info_t FrameIN;
		extern Ethernet_Frame_Info_t FrameOUT;

		extern const MAC_Address_t ServerMACAddress;
		extern const IP_Address_t  ServerIPAddress;
		extern const MAC_Address_t BroadcastMACAddress;
		extern const IP_Address_t  BroadcastIPAddress;
		extern const IP_Address_t  ClientIPAddress;

	/* Function Prototypes: */
		void     Ethernet_ProcessPacket(void);
		uint16_t Ethernet_Checksum16(void* Data,
		                             uint16_t Bytes);

#endif

