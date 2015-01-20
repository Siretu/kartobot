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
 *  General Ethernet protocol constants and type defines, for use by
 *  all network protocol portions of the TCP/IP stack.
 */

#ifndef _ETHERNET_PROTOCOLS_H_
#define _ETHERNET_PROTOCOLS_H_

	/* Macros: */
		#define ETHERTYPE_IPV4                   0x0800
		#define ETHERTYPE_ARP                    0x0806
		#define ETHERTYPE_RARP                   0x8035
		#define ETHERTYPE_APPLETALK              0x809b
		#define ETHERTYPE_APPLETALKARP           0x80f3
		#define ETHERTYPE_IEEE8021Q              0x8100
		#define ETHERTYPE_NOVELLIPX              0x8137
		#define ETHERTYPE_NOVELL                 0x8138
		#define ETHERTYPE_IPV6                   0x86DD
		#define ETHERTYPE_COBRANET               0x8819
		#define ETHERTYPE_PROVIDERBRIDGING       0x88a8
		#define ETHERTYPE_MPLSUNICAST            0x8847
		#define ETHERTYPE_MPLSMULTICAST          0x8848
		#define ETHERTYPE_PPPoEDISCOVERY         0x8863
		#define ETHERTYPE_PPPoESESSION           0x8864
		#define ETHERTYPE_EAPOVERLAN             0x888E
		#define ETHERTYPE_HYPERSCSI              0x889A
		#define ETHERTYPE_ATAOVERETHERNET        0x88A2
		#define ETHERTYPE_ETHERCAT               0x88A4
		#define ETHERTYPE_SERCOSIII              0x88CD
		#define ETHERTYPE_CESoE                  0x88D8
		#define ETHERTYPE_MACSECURITY            0x88E5
		#define ETHERTYPE_FIBRECHANNEL           0x8906
		#define ETHERTYPE_QINQ                   0x9100
		#define ETHERTYPE_VLLT                   0xCAFE

		#define PROTOCOL_ICMP                    1
		#define PROTOCOL_IGMP                    2
		#define PROTOCOL_TCP                     6
		#define PROTOCOL_UDP                     17
		#define PROTOCOL_OSPF                    89
		#define PROTOCOL_SCTP                    132

	/* Type Defines: */
		/** Type define for a physical MAC address of a device on a network. */
		typedef struct
		{
			uint8_t       Octets[6]; /**< Individual bytes of a MAC address */
		} RNDIS_MAC_Address_t;

		/** Type define for a protocol IP address of a device on a network. */
		typedef struct
		{
			uint8_t       Octets[4]; /**< Individual bytes of an IP address */
		} IP_Address_t;

#endif

