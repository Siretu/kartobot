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
 *  Header file for RFCOMMControl.c.
 */

#ifndef _RFCOMM_CONTROL_H_
#define _RFCOMM_CONTROL_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <string.h>
		#include <stdbool.h>
		#include <stdio.h>

		#include <LUFA/Common/Common.h>
		#include <LUFA/Drivers/Peripheral/Serial.h>

		#include "BluetoothStack.h"
		#include "RFCOMM.h"

	/* Macros: */
		#define RFCOMM_SIGNAL_FC               (1 << 1)
		#define RFCOMM_SIGNAL_RTC              (1 << 2)
		#define RFCOMM_SIGNAL_RTR              (1 << 3)
		#define RFCOMM_SIGNAL_IC               (1 << 6)
		#define RFCOMM_SIGNAL_DV               (1 << 7)

		#define RFCOMM_CONFIG_REMOTESIGNALS    (1 << 0)
		#define RFCOMM_CONFIG_LOCALSIGNALS     (1 << 1)
		#define RFCOMM_CONFIG_LOCALSIGNALSSENT (1 << 2)
		#define RFCOMM_CONFIG_ABMMODESET       (1 << 3)

	/* Enums: */
		enum RFCOMM_Control_Commands_t
		{
			RFCOMM_Control_Test                    = (0x20 >> 2),
			RFCOMM_Control_FlowControlEnable       = (0xA0 >> 2),
			RFCOMM_Control_FlowControlDisable      = (0x60 >> 2),
			RFCOMM_Control_ModemStatus             = (0xE0 >> 2),
			RFCOMM_Control_RemotePortNegotiation   = (0x90 >> 2),
			RFCOMM_Control_RemoteLineStatus        = (0x50 >> 2),
			RFCOMM_Control_DLCParameterNegotiation = (0x80 >> 2),
			RFCOMM_Control_NonSupportedCommand     = (0x10 >> 2),
		};

	/* Type Defines: */
		typedef struct
		{
			unsigned EA   : 1;
			unsigned CR   : 1;
			unsigned DLCI : 6;
		} RFCOMM_Address_t;

		typedef struct
		{
			RFCOMM_Address_t Address;
			uint8_t          Control;
		} RFCOMM_Header_t;

		typedef struct
		{
			unsigned EA      : 1;
			unsigned CR      : 1;
			unsigned Command : 6;
		} RFCOMM_Command_t;

		typedef struct
		{
			uint8_t  DLCI;
			unsigned FrameType        : 4;
			unsigned ConvergenceLayer : 4;
			uint8_t  Priority;
			uint8_t  ACKTimerTicks;
			uint16_t MaximumFrameSize;
			uint8_t  MaxRetransmissions;
			uint8_t  RecoveryWindowSize;
		} RFCOMM_DPN_Parameters_t;

		typedef struct
		{
			RFCOMM_Address_t Channel;
			uint8_t          Signals;
			uint8_t          BreakSignal;
		} RFCOMM_MSC_Parameters_t;

	/* Function Prototypes: */
		void RFCOMM_ProcessControlCommand(const uint8_t* Command,
		                                  Bluetooth_Channel_t* const Channel);

		#if defined(INCLUDE_FROM_RFCOMM_CONTROL_C)
			static void RFCOMM_ProcessTestCommand(const RFCOMM_Command_t* const CommandHeader,
			                                      const uint8_t CommandDataLen,
			                                      const uint8_t* CommandData,
			                                      Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessFCECommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessFCDCommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessMSCCommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t CommandDataLen,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessRPNCommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessRLSCommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
			static void RFCOMM_ProcessDPNCommand(const RFCOMM_Command_t* const CommandHeader,
			                                     const uint8_t* CommandData,
			                                     Bluetooth_Channel_t* const ACLChannel);
		#endif

#endif

