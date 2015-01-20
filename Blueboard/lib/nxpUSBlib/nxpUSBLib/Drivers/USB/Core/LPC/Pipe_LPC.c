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


#define  __INCLUDE_FROM_USB_DRIVER
#include "../USBMode.h"

#if defined(USB_CAN_BE_HOST)

#include "../Pipe.h"

// === TODO: Unify USBRAM Section ===
uint8_t usb_control_request[8] __DATA(USBRAM_SECTION);
uint8_t usb_data_buffer[USB_DATA_BUFFER_TEM_LENGTH] __DATA(USBRAM_SECTION);
uint32_t usb_data_buffer_index;

uint32_t byte_transfered;
uint8_t pipeselected;
uint32_t pipehandle[PIPE_TOTAL_PIPES];

HCD_USB_SPEED hostportspeed[MAX_HOST_CONTROLLER];
uint8_t hostselected;

bool Pipe_ConfigurePipe(const uint8_t Number,
                        const uint8_t Type,
                        const uint8_t Token,
                        const uint8_t EndpointNumber,
                        const uint16_t Size,
                        const uint8_t Banks)
{
	return (HCD_STATUS_OK == HcdOpenPipe(hostselected,				/* HostID */
										(Type == EP_TYPE_CONTROL && USB_HostState < HOST_STATE_Default_PostAddressSet) ? 0 : USB_HOST_DEVICEADDRESS,		/* DeviceAddr */
										hostportspeed[hostselected],/* DeviceSpeed */
										EndpointNumber,				/* EndpointNo */
										(HCD_TRANSFER_TYPE) Type,	/* TransferType */
										(HCD_TRANSFER_DIR) Token,	/* TransferDir */
										Size,						/* MaxPacketSize */
										1,							/* Interval */
										1,							/* Mult */
										0,							/* HSHubDevAddr */
										0,							/* HSHubPortNum */
										&pipehandle[Number]			/* PipeHandle */
									)
	);
}

void Pipe_ClosePipe(uint8_t pipenum)
{
	uint8_t i;

	if(pipenum == WRAPPER_CLOSE_PIPE_ALL)
	{
		HcdClosePipe(pipehandle[PIPE_CONTROLPIPE]);
		for(i=(PIPE_CONTROLPIPE+1);i<PIPE_TOTAL_PIPES;i++)
		{
			if(pipehandle[i] != 0)
				HcdClosePipe(pipehandle[i]);
		}
	}
	else if(pipenum < PIPE_TOTAL_PIPES)
	{
		HcdClosePipe(pipehandle[pipenum]);
	}
}

void Pipe_ClearPipes(void)
{

}

bool Pipe_IsEndpointBound(const uint8_t EndpointAddress)
{
	return false;
}

uint8_t Pipe_WaitUntilReady(void)
{
	return 0;
}

#endif

