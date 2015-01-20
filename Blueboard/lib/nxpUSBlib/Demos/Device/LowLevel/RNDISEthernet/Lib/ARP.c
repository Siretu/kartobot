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
 *  Address Resolution Protocol (ARP) packet handling routines. This protocol handles the
 *  conversion of physical MAC addresses to protocol IP addresses between the host and the
 *  device.
 */

#include "ARP.h"

/** Processes an ARP packet inside an Ethernet frame, and writes the appropriate response
 *  to the output Ethernet frame if the host is requesting the IP or MAC address of the
 *  virtual server device on the network.
 *
 *  \param[in] InDataStart    Pointer to the start of the incoming packet's ARP header
 *  \param[out] OutDataStart  Pointer to the start of the outgoing packet's ARP header
 *
 *  \return The number of bytes written to the out Ethernet frame if any, NO_RESPONSE otherwise
 */
int16_t ARP_ProcessARPPacket(void* InDataStart,
                             void* OutDataStart)
{
	DecodeARPHeader(InDataStart);

	ARP_Header_t* ARPHeaderIN  = (ARP_Header_t*)InDataStart;
	ARP_Header_t* ARPHeaderOUT = (ARP_Header_t*)OutDataStart;

	/* Ensure that the ARP request is a IPv4 request packet */
	if ((SwapEndian_16(ARPHeaderIN->ProtocolType) == ETHERTYPE_IPV4) &&
	    (SwapEndian_16(ARPHeaderIN->Operation) == ARP_OPERATION_REQUEST))
	{
		/* If the ARP packet is requesting the MAC or IP of the virtual webserver, return the response */
		if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) ||
		    MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
		{
			/* Fill out the ARP response header */
			ARPHeaderOUT->HardwareType = ARPHeaderIN->HardwareType;
			ARPHeaderOUT->ProtocolType = ARPHeaderIN->ProtocolType;
			ARPHeaderOUT->HLEN         = ARPHeaderIN->HLEN;
			ARPHeaderOUT->PLEN         = ARPHeaderIN->PLEN;
			ARPHeaderOUT->Operation    = SwapEndian_16(ARP_OPERATION_REPLY);

			/* Copy over the sender MAC/IP to the target fields for the response */
			ARPHeaderOUT->THA = ARPHeaderIN->SHA;
			ARPHeaderOUT->TPA = ARPHeaderIN->SPA;

			/* Copy over the new sender MAC/IP - MAC and IP addresses of the virtual webserver */
			ARPHeaderOUT->SHA = ServerMACAddress;
			ARPHeaderOUT->SPA = ServerIPAddress;

			/* Return the size of the response so far */
			return sizeof(ARP_Header_t);
		}
	}

	return NO_RESPONSE;
}

