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

#define  __INCLUDE_FROM_HOST_C
#include "../Host.h"

static uint8_t CurrentHostID = 0;
uint8_t USB_Host_ControlPipeSize = PIPE_CONTROLPIPE_DEFAULT_SIZE;

void USB_Host_ProcessNextHostState(void)
{
	uint8_t ErrorCode    = HOST_ENUMERROR_NoError;
	uint8_t SubErrorCode = HOST_ENUMERROR_NoError;

	static uint16_t WaitMSRemaining;
	static uint8_t  PostWaitState;

	switch (USB_HostState)
	{
		case HOST_STATE_WaitForDevice:
			if (WaitMSRemaining)
			{
				if ((SubErrorCode = USB_Host_WaitMS(1)) != HOST_WAITERROR_Successful)
				{
					USB_HostState = PostWaitState;
					ErrorCode     = HOST_ENUMERROR_WaitStage;
					break;
				}

				if (!(--WaitMSRemaining))
				  USB_HostState = PostWaitState;
			}

			break;
		case HOST_STATE_Powered:
			WaitMSRemaining = HOST_DEVICE_SETTLE_DELAY_MS;

			USB_HostState = HOST_STATE_Powered_WaitForDeviceSettle;
			break;
		case HOST_STATE_Powered_WaitForDeviceSettle:
			if (WaitMSRemaining--)
			{
				Delay_MS(1);
				break;
			}
			else
			{
				USB_Host_VBUS_Manual_Off();

				USB_OTGPAD_On();
				USB_Host_VBUS_Auto_Enable();
				USB_Host_VBUS_Auto_On();

				USB_HostState = HOST_STATE_Powered_WaitForConnect;
			}

			break;
		case HOST_STATE_Powered_WaitForConnect:
			HOST_TASK_NONBLOCK_WAIT(100, HOST_STATE_Powered_DoReset);
			break;
		case HOST_STATE_Powered_DoReset:
		{
			HCD_USB_SPEED DeviceSpeed;
			HcdRhPortReset(CurrentHostID,1);
			HcdGetDeviceSpeed(CurrentHostID, 1, &DeviceSpeed); // skip checking status
			USB_Host_SetDeviceSpeed(CurrentHostID,DeviceSpeed);
			HOST_TASK_NONBLOCK_WAIT(200, HOST_STATE_Powered_ConfigPipe);
		}
			break;
		case HOST_STATE_Powered_ConfigPipe:
			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, EP_TYPE_CONTROL,
							   PIPE_TOKEN_SETUP, ENDPOINT_CONTROLEP,
							   PIPE_CONTROLPIPE_DEFAULT_SIZE, PIPE_BANK_SINGLE);

			if (!(Pipe_IsConfigured()))
			{
				ErrorCode    = HOST_ENUMERROR_PipeConfigError;
				SubErrorCode = 0;
				break;
			}

			USB_HostState = HOST_STATE_Default;
			break;
		case HOST_STATE_Default:
			USB_ControlRequest = (USB_Request_Header_t)
				{
					.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
					.bRequest      = REQ_GetDescriptor,
					.wValue        = (DTYPE_Device << 8),
					.wIndex        = 0,
					.wLength       = 8,
				};

			uint8_t DataBuffer[8];

			if ((SubErrorCode = USB_Host_SendControlRequest(DataBuffer)) != HOST_SENDCONTROL_Successful)
			{
				ErrorCode = HOST_ENUMERROR_ControlError;
				break;
			}

			USB_Host_ControlPipeSize = DataBuffer[offsetof(USB_Descriptor_Device_t, Endpoint0Size)];

			Pipe_ClosePipe(PIPE_CONTROLPIPE);
			HcdRhPortReset(CurrentHostID,1);

			HOST_TASK_NONBLOCK_WAIT(200, HOST_STATE_Default_PostReset);
			break;
		case HOST_STATE_Default_PostReset:
			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, EP_TYPE_CONTROL,
			                   PIPE_TOKEN_SETUP, ENDPOINT_CONTROLEP,
			                   USB_Host_ControlPipeSize, PIPE_BANK_SINGLE);

			if (!(Pipe_IsConfigured()))
			{
				ErrorCode    = HOST_ENUMERROR_PipeConfigError;
				SubErrorCode = 0;
				break;
			}

			USB_ControlRequest = (USB_Request_Header_t)
				{
					.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE),
					.bRequest      = REQ_SetAddress,
					.wValue        = USB_HOST_DEVICEADDRESS,
					.wIndex        = 0,
					.wLength       = 0,
				};

			if ((SubErrorCode = USB_Host_SendControlRequest(NULL)) != HOST_SENDCONTROL_Successful)
			{
				ErrorCode = HOST_ENUMERROR_ControlError;
				break;
			}

			HOST_TASK_NONBLOCK_WAIT(100, HOST_STATE_Default_PostAddressSet);
			break;
		case HOST_STATE_Default_PostAddressSet:
			Pipe_ClosePipe(PIPE_CONTROLPIPE);
			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, EP_TYPE_CONTROL,
								PIPE_TOKEN_SETUP, ENDPOINT_CONTROLEP,
								USB_Host_ControlPipeSize, PIPE_BANK_SINGLE);

			USB_Host_SetDeviceAddress(USB_HOST_DEVICEADDRESS);

			USB_HostState = HOST_STATE_Addressed;

			EVENT_USB_Host_DeviceEnumerationComplete();
			break;
	}

	if ((ErrorCode != HOST_ENUMERROR_NoError) && (USB_HostState != HOST_STATE_Unattached))
	{
		EVENT_USB_Host_DeviceEnumerationFailed(ErrorCode, SubErrorCode);

		USB_Host_VBUS_Auto_Off();

		EVENT_USB_Host_DeviceUnattached();

		USB_ResetInterface();
	}
}

uint8_t USB_Host_WaitMS(uint8_t MS)
{
	return HOST_WAITERROR_Successful;
}

void USB_Host_Enumerate (uint8_t HostId)
{
	CurrentHostID = HostId;
	EVENT_USB_Host_DeviceAttached();
	USB_HostState = HOST_STATE_Powered;
}

void USB_Host_DeEnumerate(uint8_t HostId)
{
	Pipe_ClosePipe(WRAPPER_CLOSE_PIPE_ALL);
	EVENT_USB_Host_DeviceUnattached();
	USB_HostState = HOST_STATE_Unattached;
}

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void USB_Host_SetActiveHost(uint8_t hostid)
{
	hostselected = hostid;
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
uint8_t USB_Host_GetActiveHost(void)
{
	return hostselected;
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void USB_Host_SetDeviceSpeed(uint8_t hostid, HCD_USB_SPEED speed)
{
	hostportspeed[hostid] = speed;
}
/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
HCD_USB_SPEED USB_Host_GetDeviceSpeed(uint8_t hostid)
{
	if(hostid<=1)
		return hostportspeed[hostid];
	else return LOW_SPEED;
}
#endif

