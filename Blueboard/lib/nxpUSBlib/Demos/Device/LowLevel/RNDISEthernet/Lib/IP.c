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
 *  Internet Protocol (IP) packet handling routines. This protocol handles IP packets from the
 *  host which typically encapsulate other protocols such as ICMP, UDP and TCP.
 */

#include "IP.h"

/** Processes an IP packet inside an Ethernet frame, and writes the appropriate response
 *  to the output Ethernet frame if one is created by a sub-protocol handler.
 *
 *  \param[in] InDataStart    Pointer to the start of the incoming packet's IP header
 *  \param[out] OutDataStart  Pointer to the start of the outgoing packet's IP header
 *
 *  \return The number of bytes written to the out Ethernet frame if any, NO_RESPONSE if no
 *           response was generated, NO_PROCESS if the packet processing was deferred until the
 *           next Ethernet packet handler iteration
 */
int16_t IP_ProcessIPPacket(void* InDataStart,
                           void* OutDataStart)
{
	DecodeIPHeader(InDataStart);

	IP_Header_t* IPHeaderIN  = (IP_Header_t*)InDataStart;
	IP_Header_t* IPHeaderOUT = (IP_Header_t*)OutDataStart;

	/* Header length is specified in number of longs in the packet header, convert to bytes */
	uint16_t HeaderLengthBytes = (IPHeaderIN->HeaderLength * sizeof(uint32_t));

	int16_t  RetSize = NO_RESPONSE;

	/* Check to ensure the IP packet is addressed to the virtual webserver's IP or the broadcast IP address */
	if (!(IP_COMPARE(&IPHeaderIN->DestinationAddress, &ServerIPAddress)) &&
	    !(IP_COMPARE(&IPHeaderIN->DestinationAddress, &BroadcastIPAddress)))
	{
		return NO_RESPONSE;
	}

	/* Pass off the IP payload to the appropriate protocol processing routine */
	switch (IPHeaderIN->Protocol)
	{
		case PROTOCOL_ICMP:
			RetSize = ICMP_ProcessICMPPacket(&((uint8_t*)InDataStart)[HeaderLengthBytes],
			                                 &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
			break;
		case PROTOCOL_TCP:
			RetSize = TCP_ProcessTCPPacket(InDataStart,
			                               &((uint8_t*)InDataStart)[HeaderLengthBytes],
			                               &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
			break;
		case PROTOCOL_UDP:
			RetSize = UDP_ProcessUDPPacket(InDataStart,
			                               &((uint8_t*)InDataStart)[HeaderLengthBytes],
			                               &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
			break;
	}

	/* Check to see if the protocol processing routine has filled out a response */
	if (RetSize > 0)
	{
		/* Fill out the response IP packet header */
		IPHeaderOUT->TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + RetSize);
		IPHeaderOUT->TypeOfService      = 0;
		IPHeaderOUT->HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
		IPHeaderOUT->Version            = 4;
		IPHeaderOUT->Flags              = 0;
		IPHeaderOUT->FragmentOffset     = 0;
		IPHeaderOUT->Identification     = 0;
		IPHeaderOUT->HeaderChecksum     = 0;
		IPHeaderOUT->Protocol           = IPHeaderIN->Protocol;
		IPHeaderOUT->TTL                = DEFAULT_TTL;
		IPHeaderOUT->SourceAddress      = IPHeaderIN->DestinationAddress;
		IPHeaderOUT->DestinationAddress = IPHeaderIN->SourceAddress;

		IPHeaderOUT->HeaderChecksum     = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));

		/* Return the size of the response so far */
		return (sizeof(IP_Header_t) + RetSize);
	}

	return RetSize;
}

