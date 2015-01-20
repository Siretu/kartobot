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
 *  User Datagram Protocol (UDP) packet handling routines. This protocol handles high throughput, low
 *  reliability packets which are typically used to encapsulate streaming data.
 */

#define  INCLUDE_FROM_UDP_C
#include "UDP.h"

/** Processes a UDP packet inside an Ethernet frame, and writes the appropriate response
 *  to the output Ethernet frame if a sub-protocol handler has created a response packet.
 *
 *  \param[in] IPHeaderInStart     Pointer to the start of the incoming packet's IP header
 *  \param[in] UDPHeaderInStart    Pointer to the start of the incoming packet's UDP header
 *  \param[out] UDPHeaderOutStart  Pointer to the start of the outgoing packet's UDP header
 *
 *  \return The number of bytes written to the out Ethernet frame if any, NO_RESPONSE otherwise
 */
int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart,
                             void* UDPHeaderInStart,
                             void* UDPHeaderOutStart)
{
	UDP_Header_t* UDPHeaderIN  = (UDP_Header_t*)UDPHeaderInStart;
	UDP_Header_t* UDPHeaderOUT = (UDP_Header_t*)UDPHeaderOutStart;

	int16_t RetSize = NO_RESPONSE;

	DecodeUDPHeader(UDPHeaderInStart);

	switch (SwapEndian_16(UDPHeaderIN->DestinationPort))
	{
		case UDP_PORT_DHCP_REQUEST:
			RetSize = DHCP_ProcessDHCPPacket(IPHeaderInStart,
			                                 &((uint8_t*)UDPHeaderInStart)[sizeof(UDP_Header_t)],
		                                     &((uint8_t*)UDPHeaderOutStart)[sizeof(UDP_Header_t)]);
			break;
	}

	/* Check to see if the protocol processing routine has filled out a response */
	if (RetSize > 0)
	{
		/* Fill out the response UDP packet header */
		UDPHeaderOUT->SourcePort      = UDPHeaderIN->DestinationPort;
		UDPHeaderOUT->DestinationPort = UDPHeaderIN->SourcePort;
		UDPHeaderOUT->Checksum        = 0;
		UDPHeaderOUT->Length          = SwapEndian_16(sizeof(UDP_Header_t) + RetSize);

		/* Return the size of the response so far */
		return (sizeof(UDP_Header_t) + RetSize);
	}

	return NO_RESPONSE;
}

