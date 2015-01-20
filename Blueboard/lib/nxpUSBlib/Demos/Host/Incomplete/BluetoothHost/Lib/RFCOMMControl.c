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
 *  RFCOMM multiplexer control layer module. This module handles multiplexer
 *  channel commands to the control DLCI in the RFCOMM layer, to open, configure,
 *  test and close logical RFCOMM channels.
 */

#define  INCLUDE_FROM_RFCOMM_CONTROL_C
#include "RFCOMMControl.h"

void RFCOMM_ProcessControlCommand(const uint8_t* Command,
                                  Bluetooth_Channel_t* const ACLChannel)
{
	const RFCOMM_Command_t* CommandHeader  = (const RFCOMM_Command_t*)Command;
	const uint8_t*          CommandData    = (const uint8_t*)Command + sizeof(RFCOMM_Command_t);
	uint8_t                 CommandDataLen = RFCOMM_GetVariableFieldValue(&CommandData);

	switch (CommandHeader->Command)
	{
		case RFCOMM_Control_Test:
			RFCOMM_ProcessTestCommand(CommandHeader, CommandDataLen, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_FlowControlEnable:
			RFCOMM_ProcessFCECommand(CommandHeader, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_FlowControlDisable:
			RFCOMM_ProcessFCDCommand(CommandHeader, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_ModemStatus:
			RFCOMM_ProcessMSCCommand(CommandHeader, CommandDataLen, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_RemotePortNegotiation:
			RFCOMM_ProcessRPNCommand(CommandHeader, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_RemoteLineStatus:
			RFCOMM_ProcessRLSCommand(CommandHeader, CommandData, ACLChannel);
			break;
		case RFCOMM_Control_DLCParameterNegotiation:
			RFCOMM_ProcessDPNCommand(CommandHeader, CommandData, ACLChannel);
			break;
		default:
			BT_RFCOMM_DEBUG(1, "<< Unknown Command");
			break;
	}
}

static void RFCOMM_ProcessTestCommand(const RFCOMM_Command_t* const CommandHeader,
                                      const uint8_t CommandDataLen,
                                      const uint8_t* CommandData,
                                      Bluetooth_Channel_t* const ACLChannel)
{
	const uint8_t* Params = (const uint8_t*)CommandData;

	BT_RFCOMM_DEBUG(1, "<< TEST Command");

	struct
	{
		RFCOMM_Command_t CommandHeader;
		uint8_t          Length;
		uint8_t          TestData[CommandDataLen];
	} TestResponse;

	/* Fill out the Test response data */
	TestResponse.CommandHeader = (RFCOMM_Command_t){.Command = RFCOMM_Control_Test, .EA = true, .CR = false};
	TestResponse.Length        = (CommandDataLen << 1) | 0x01;
	memcpy(TestResponse.TestData, Params, CommandDataLen);

	BT_RFCOMM_DEBUG(1, ">> TEST Response");

	/* Send the PDN response to acknowledge the command */
	RFCOMM_SendFrame(RFCOMM_CONTROL_DLCI, false, RFCOMM_Frame_UIH, sizeof(TestResponse), &TestResponse, ACLChannel);
}

static void RFCOMM_ProcessFCECommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t* CommandData,
			                         Bluetooth_Channel_t* const ACLChannel)
{
	BT_RFCOMM_DEBUG(1, "<< FCE Command");
}

static void RFCOMM_ProcessFCDCommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t* CommandData,
			                         Bluetooth_Channel_t* const ACLChannel)
{
	BT_RFCOMM_DEBUG(1, "<< FCD Command");
}

static void RFCOMM_ProcessMSCCommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t CommandDataLen,
                                     const uint8_t* CommandData,
                                     Bluetooth_Channel_t* const ACLChannel)
{
	const RFCOMM_MSC_Parameters_t* Params = (const RFCOMM_MSC_Parameters_t*)CommandData;

	BT_RFCOMM_DEBUG(1, "<< MSC %s", (CommandHeader->CR) ? "Command" : "Response");
	BT_RFCOMM_DEBUG(2, "-- DLCI: 0x%02X", Params->Channel.DLCI);

	/* Ignore status flags sent to the control channel */
	if (Params->Channel.DLCI == RFCOMM_CONTROL_DLCI)
	  return;

	/* Retrieve existing channel configuration data, if already opened */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_GetChannelData(Params->Channel.DLCI);

	/* If the channel does not exist, abort */
	if (RFCOMMChannel == NULL)
	  return;

	/* Check if the MSC packet is a command or a response */
	if (CommandHeader->CR)
	{
		/* Save the new channel signals to the channel state structure */
		RFCOMMChannel->Remote.Signals  = Params->Signals;
		RFCOMMChannel->ConfigFlags    |= RFCOMM_CONFIG_REMOTESIGNALS;

		/* If the command contains the optional break signals field, store the value */
		if (CommandDataLen == sizeof(RFCOMM_MSC_Parameters_t))
		  RFCOMMChannel->Remote.BreakSignal = Params->BreakSignal;

		/* Notify the user application that the signals have been received */
		RFCOMM_ChannelSignalsReceived(RFCOMMChannel);

		struct
		{
			RFCOMM_Command_t        CommandHeader;
			uint8_t                 Length;
			RFCOMM_MSC_Parameters_t Params;
		} MSResponse;

		/* Fill out the MS response data */
		MSResponse.CommandHeader  = (RFCOMM_Command_t){.Command = RFCOMM_Control_ModemStatus, .EA = true, .CR = false};
		MSResponse.Length         = (CommandDataLen << 1) | 0x01;
		memcpy(&MSResponse.Params, Params, sizeof(RFCOMM_MSC_Parameters_t));

		BT_RFCOMM_DEBUG(1, ">> MSC Response");

		/* Send the MSC response to acknowledge the command */
		RFCOMM_SendFrame(RFCOMM_CONTROL_DLCI, false, RFCOMM_Frame_UIH,
						 (sizeof(MSResponse) - sizeof(MSResponse.Params) + CommandDataLen), &MSResponse, ACLChannel);
	}
	else
	{
		/* Indicate that the remote device has acknowledged the sent signals */
		RFCOMMChannel->ConfigFlags |= RFCOMM_CONFIG_LOCALSIGNALS;
	}
}

static void RFCOMM_ProcessRPNCommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t* CommandData,
			                         Bluetooth_Channel_t* const ACLChannel)
{
	BT_RFCOMM_DEBUG(1, "<< RPN Command");
}

static void RFCOMM_ProcessRLSCommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t* CommandData,
			                         Bluetooth_Channel_t* const ACLChannel)
{
	BT_RFCOMM_DEBUG(1, "<< RLS Command");
}

static void RFCOMM_ProcessDPNCommand(const RFCOMM_Command_t* const CommandHeader,
                                     const uint8_t* CommandData,
			                         Bluetooth_Channel_t* const ACLChannel)
{
	const RFCOMM_DPN_Parameters_t* Params = (const RFCOMM_DPN_Parameters_t*)CommandData;

	BT_RFCOMM_DEBUG(1, "<< DPN Command");
	BT_RFCOMM_DEBUG(2, "-- DLCI: 0x%02X", Params->DLCI);

	/* Ignore parameter negotiations to the control channel */
	if (Params->DLCI == RFCOMM_CONTROL_DLCI)
	  return;

	/* Retrieve existing channel configuration data, if already opened */
	RFCOMM_Channel_t* RFCOMMChannel = RFCOMM_GetChannelData(Params->DLCI);

	/* Check if the channel has no corresponding entry - remote did not open it first */
	if (RFCOMMChannel == NULL)
	{
		/* Create a new entry in the channel table for the new channel */
		RFCOMMChannel = RFCOMM_GetFreeChannelEntry(Params->DLCI);

		/* No free entry was found, discard the request */
		if (RFCOMMChannel == NULL)
		{
			BT_RFCOMM_DEBUG(2, "-- No Free Channel");
			return;
		}
	}

	/* Save the new channel configuration */
	RFCOMMChannel->State       = RFCOMM_Channel_Configure;
	RFCOMMChannel->Priority    = Params->Priority;
	RFCOMMChannel->MTU         = Params->MaximumFrameSize;

	struct
	{
		RFCOMM_Command_t        CommandHeader;
		uint8_t                 Length;
		RFCOMM_DPN_Parameters_t Params;
	} DPNResponse;

	/* Fill out the DPN response data */
	DPNResponse.CommandHeader = (RFCOMM_Command_t){.Command = RFCOMM_Control_DLCParameterNegotiation, .EA = true, .CR = false};
	DPNResponse.Length                  = (sizeof(DPNResponse.Params) << 1) | 0x01;
	memcpy(&DPNResponse.Params, Params, sizeof(RFCOMM_DPN_Parameters_t));
	DPNResponse.Params.ConvergenceLayer = 0x00; // TODO: Enable credit based transaction support

	BT_RFCOMM_DEBUG(1, ">> DPN Response");

	/* Send the DPN response to acknowledge the command */
	RFCOMM_SendFrame(RFCOMM_CONTROL_DLCI, false, RFCOMM_Frame_UIH, sizeof(DPNResponse), &DPNResponse, ACLChannel);
}

