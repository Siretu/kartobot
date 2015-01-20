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

#if defined(USB_CAN_BE_DEVICE)
#include "../Endpoint.h"

#if !defined(FIXED_CONTROL_ENDPOINT_SIZE)
uint8_t USB_Device_ControlEndpointSize = ENDPOINT_CONTROLEP_DEFAULT_SIZE;
#endif

uint8_t usb_data_buffer[512] __DATA(USBRAM_SECTION);

uint32_t usb_data_buffer_index;

uint8_t endpointselected;
uint8_t endpointhandle[ENDPOINT_TOTAL_ENDPOINTS];

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
                                    const uint8_t UECFG0XData,
                                    const uint8_t UECFG1XData)
{
#if defined(CONTROL_ONLY_DEVICE) || defined(ORDERED_EP_CONFIG)
	Endpoint_SelectEndpoint(Number);
	Endpoint_EnableEndpoint();

//	UECFG1X = 0;
//	UECFG0X = UECFG0XData;
//	UECFG1X = UECFG1XData;

	return Endpoint_IsConfigured();
#else	
	for (uint8_t EPNum = Number; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
	{
//		uint8_t UECFG0XTemp;
//		uint8_t UECFG1XTemp;
//		uint8_t UEIENXTemp;

		Endpoint_SelectEndpoint(EPNum);
		
		if (EPNum == Number)
		{
//			UECFG0XTemp = UECFG0XData;
//			UECFG1XTemp = UECFG1XData;
//			UEIENXTemp  = 0;
		}
		else
		{
//			UECFG0XTemp = UECFG0X;
//			UECFG1XTemp = UECFG1X;
//			UEIENXTemp  = UEIENX;
		}

//		if (!(UECFG1XTemp & (1 << ALLOC)))
//		  continue;

		Endpoint_DisableEndpoint();
//		UECFG1X &= ~(1 << ALLOC);

		Endpoint_EnableEndpoint();
//		UECFG0X = UECFG0XTemp;
//		UECFG1X = UECFG1XTemp;
//		UEIENX  = UEIENXTemp;
			
		if (!(Endpoint_IsConfigured()))
		  return false;			
	}
	
	Endpoint_SelectEndpoint(Number);
	return true;
#endif
}

void Endpoint_ClearEndpoints(void)
{
//	UEINT = 0;

	for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
	{
		Endpoint_SelectEndpoint(EPNum);
//		UEIENX  = 0;
//		UEINTX  = 0;
//		UECFG1X = 0;
		Endpoint_DisableEndpoint();
	}
}

void Endpoint_ClearStatusStage(void)
{
	if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST)
	{
		Endpoint_ClearOUT();
	}
	else
	{
		Endpoint_ClearIN();
	}
}

#if !defined(CONTROL_ONLY_DEVICE)
uint8_t Endpoint_WaitUntilReady(void)
{
	return ENDPOINT_READYWAIT_NoError;
}
#endif

#endif

