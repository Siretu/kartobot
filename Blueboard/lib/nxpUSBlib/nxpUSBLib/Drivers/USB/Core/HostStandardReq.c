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
#include "USBMode.h"

#if defined(USB_CAN_BE_HOST)

#define  __INCLUDE_FROM_HOSTSTDREQ_C
#include "HostStandardReq.h"

uint8_t USB_Host_ConfigurationNumber;

#if 1

uint8_t USB_Host_SendControlRequest(void* const BufferPtr)
{
	uint8_t* DataStream   = (uint8_t*)BufferPtr;
	uint16_t DataLen      = USB_ControlRequest.wLength;

	memcpy(usb_control_request, &USB_ControlRequest, 8);

	if ((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_HOSTTODEVICE)
	{
		while(DataLen)
		{
			Pipe_Write_8(*(DataStream++));
			DataLen--;
		}
	}

	HcdControlTransfer(pipehandle[pipeselected], usb_control_request, usb_data_buffer);
	usb_data_buffer_index = 0;

	if ((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		while(DataLen)
		{
			*(DataStream++) = Pipe_Read_8();
			DataLen--;
		}
	}

	return HOST_SENDCONTROL_Successful;
}

#else // The following code is deprecated
uint8_t USB_Host_SendControlRequest(void* const BufferPtr)
{
	uint8_t* DataStream   = (uint8_t*)BufferPtr;
	bool     BusSuspended = USB_Host_IsBusSuspended();
	uint8_t  ReturnStatus = HOST_SENDCONTROL_Successful;
	uint16_t DataLen      = USB_ControlRequest.wLength;

	USB_Host_ResumeBus();

	if ((ReturnStatus = USB_Host_WaitMS(1)) != HOST_WAITERROR_Successful)
	  goto End_Of_Control_Send;

	Pipe_SetPipeToken(PIPE_TOKEN_SETUP);
	Pipe_ClearError();

	Pipe_Unfreeze();

	Pipe_Write_8(USB_ControlRequest.bmRequestType);
	Pipe_Write_8(USB_ControlRequest.bRequest);
	Pipe_Write_16_LE(USB_ControlRequest.wValue);
	Pipe_Write_16_LE(USB_ControlRequest.wIndex);
	Pipe_Write_16_LE(USB_ControlRequest.wLength);

	Pipe_ClearSETUP();

	if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_SetupSent)) != HOST_SENDCONTROL_Successful)
	  goto End_Of_Control_Send;

	Pipe_Freeze();

	if ((ReturnStatus = USB_Host_WaitMS(1)) != HOST_WAITERROR_Successful)
	  goto End_Of_Control_Send;

	if ((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		Pipe_SetPipeToken(PIPE_TOKEN_IN);

		if (DataStream != NULL)
		{
			while (DataLen)
			{
				Pipe_Unfreeze();

				if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_InReceived)) != HOST_SENDCONTROL_Successful)
				  goto End_Of_Control_Send;

				if (!(Pipe_BytesInPipe()))
				  DataLen = 0;

				while (Pipe_BytesInPipe() && DataLen)
				{
					*(DataStream++) = Pipe_Read_8();
					DataLen--;
				}

				Pipe_Freeze();
				Pipe_ClearIN();
			}
		}

		Pipe_SetPipeToken(PIPE_TOKEN_OUT);
		Pipe_Unfreeze();

		if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_OutReady)) != HOST_SENDCONTROL_Successful)
		  goto End_Of_Control_Send;

		Pipe_ClearOUT();

		if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_OutReady)) != HOST_SENDCONTROL_Successful)
		  goto End_Of_Control_Send;
	}
	else
	{
		if (DataStream != NULL)
		{
			Pipe_SetPipeToken(PIPE_TOKEN_OUT);
			Pipe_Unfreeze();

			while (DataLen)
			{
				if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_OutReady)) != HOST_SENDCONTROL_Successful)
				  goto End_Of_Control_Send;

				while (DataLen && (Pipe_BytesInPipe() < USB_Host_ControlPipeSize))
				{
					Pipe_Write_8(*(DataStream++));
					DataLen--;
				}

				Pipe_ClearOUT();
			}

			if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_OutReady)) != HOST_SENDCONTROL_Successful)
			  goto End_Of_Control_Send;

			Pipe_Freeze();
		}

		Pipe_SetPipeToken(PIPE_TOKEN_IN);
		Pipe_Unfreeze();

		if ((ReturnStatus = USB_Host_WaitForIOS(USB_HOST_WAITFOR_InReceived)) != HOST_SENDCONTROL_Successful)
		  goto End_Of_Control_Send;

		Pipe_ClearIN();
	}

End_Of_Control_Send:
	Pipe_Freeze();

	if (BusSuspended)
	  USB_Host_SuspendBus();

	Pipe_ResetPipe(PIPE_CONTROLPIPE);

	return ReturnStatus;
}

static uint8_t USB_Host_WaitForIOS(const uint8_t WaitType)
{
	#if (USB_HOST_TIMEOUT_MS < 0xFF)
	uint8_t  TimeoutCounter = USB_HOST_TIMEOUT_MS;
	#else
	uint16_t TimeoutCounter = USB_HOST_TIMEOUT_MS;
	#endif

	while (!(((WaitType == USB_HOST_WAITFOR_SetupSent)  && Pipe_IsSETUPSent())  ||
	         ((WaitType == USB_HOST_WAITFOR_InReceived) && Pipe_IsINReceived()) ||
	         ((WaitType == USB_HOST_WAITFOR_OutReady)   && Pipe_IsOUTReady())))
	{
		uint8_t ErrorCode;

		if ((ErrorCode = USB_Host_WaitMS(1)) != HOST_WAITERROR_Successful)
		  return ErrorCode;

		if (!(TimeoutCounter--))
		  return HOST_SENDCONTROL_SoftwareTimeOut;
	}

	return HOST_SENDCONTROL_Successful;
}
#endif

uint8_t USB_Host_SetDeviceConfiguration(const uint8_t ConfigNumber)
{
	uint8_t ErrorCode;

	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE),
			.bRequest      = REQ_SetConfiguration,
			.wValue        = ConfigNumber,
			.wIndex        = 0,
			.wLength       = 0,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);
	
	if ((ErrorCode = USB_Host_SendControlRequest(NULL)) == HOST_SENDCONTROL_Successful)
	{
		USB_Host_ConfigurationNumber = ConfigNumber;
		USB_HostState                = (ConfigNumber) ? HOST_STATE_Configured : HOST_STATE_Addressed;
	}

	return ErrorCode;
}

uint8_t USB_Host_GetDeviceDescriptor(void* const DeviceDescriptorPtr)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			.bRequest      = REQ_GetDescriptor,
			.wValue        = (DTYPE_Device << 8),
			.wIndex        = 0,
			.wLength       = sizeof(USB_Descriptor_Device_t),
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(DeviceDescriptorPtr);
}

uint8_t USB_Host_GetDeviceStringDescriptor(const uint8_t Index,
                                           void* const Buffer,
                                           const uint8_t BufferLength)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			.bRequest      = REQ_GetDescriptor,
			.wValue        = (DTYPE_String << 8) | Index,
			.wIndex        = 0,
			.wLength       = BufferLength,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(Buffer);
}

uint8_t USB_Host_GetDeviceStatus(uint8_t* const FeatureStatus)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			.bRequest      = REQ_GetStatus,
			.wValue        = 0,
			.wIndex        = 0,
			.wLength       = 0,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(FeatureStatus);
}

uint8_t USB_Host_ClearEndpointStall(const uint8_t EndpointAddress)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT),
			.bRequest      = REQ_ClearFeature,
			.wValue        = FEATURE_SEL_EndpointHalt,
			.wIndex        = EndpointAddress,
			.wLength       = 0,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(NULL);
}

uint8_t USB_Host_SetInterfaceAltSetting(const uint8_t InterfaceIndex,
                                        const uint8_t AltSetting)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE),
			.bRequest      = REQ_SetInterface,
			.wValue        = AltSetting,
			.wIndex        = InterfaceIndex,
			.wLength       = 0,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(NULL);
}

#endif

