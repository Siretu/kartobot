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
 *  Internet Control Message Protocol (ICMP) packet handling routines. This protocol handles
 *  Echo requests from the host, to indicate a successful network connection between the host
 *  and the virtual server.
 */

#include "ICMP.h"

/** Processes an ICMP packet inside an Ethernet frame, and writes the appropriate response
 *  to the output Ethernet frame if the host is issuing a ICMP ECHO request.
 *
 *  \param[in] FrameIN        Pointer to the incoming Ethernet frame information structure
 *  \param[in] InDataStart    Pointer to the start of the incoming packet's ICMP header
 *  \param[out] OutDataStart  Pointer to the start of the outgoing packet's ICMP header
 *
 *  \return The number of bytes written to the out Ethernet frame if any, NO_RESPONSE otherwise
 */
int16_t ICMP_ProcessICMPPacket(Ethernet_Frame_Info_t* const FrameIN,
                               void* InDataStart,
                               void* OutDataStart)
{
	ICMP_Header_t* ICMPHeaderIN  = (ICMP_Header_t*)InDataStart;
	ICMP_Header_t* ICMPHeaderOUT = (ICMP_Header_t*)OutDataStart;

	DecodeICMPHeader(InDataStart);

	/* Determine if the ICMP packet is an echo request (ping) */
	if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
	{
		/* Fill out the ICMP response packet */
		ICMPHeaderOUT->Type     = ICMP_TYPE_ECHOREPLY;
		ICMPHeaderOUT->Code     = 0;
		ICMPHeaderOUT->Checksum = 0;
		ICMPHeaderOUT->Id       = ICMPHeaderIN->Id;
		ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

		intptr_t DataSize = FrameIN->FrameLength - ((((intptr_t)InDataStart + sizeof(ICMP_Header_t)) - (intptr_t)FrameIN->FrameData));

		/* Copy the remaining payload to the response - echo requests should echo back any sent data */
		memmove(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
		        &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)],
			    DataSize);

		ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT, (DataSize + sizeof(ICMP_Header_t)));

		/* Return the size of the response so far */
		return (DataSize + sizeof(ICMP_Header_t));
	}

	return NO_RESPONSE;
}

