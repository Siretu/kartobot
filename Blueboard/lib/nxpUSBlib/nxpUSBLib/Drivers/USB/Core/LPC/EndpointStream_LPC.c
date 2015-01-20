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

#include "EndpointStream_LPC.h"

#if !defined(CONTROL_ONLY_DEVICE)
uint8_t Endpoint_Discard_Stream(uint16_t Length,
                                uint16_t* const BytesProcessed)
{
	uint32_t i;
	for (i=0; i < Length; i++)
	{
		Endpoint_Discard_8();	
	}
	return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Null_Stream(uint16_t Length,
                             uint16_t* const BytesProcessed)
{
	uint32_t i;
	for (i=0; i < Length; i++)
	{
		Endpoint_Write_8(0);	
	}
	return ENDPOINT_RWSTREAM_NoError;
}
uint8_t Endpoint_Write_Stream_LE(const void* const Buffer,
			                                 uint16_t Length,
			                                 uint16_t* const BytesProcessed)
{
	uint16_t i;

	while ( !Endpoint_IsINReady() ) /*-- Wait until ready --*/
	{
		Delay_MS(2);
	}
	for(i=0;i<Length;i++)
	{
		Endpoint_Write_8(((uint8_t*)Buffer)[i]);
	}

	return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Write_Stream_BE(const void* const Buffer,
			                                 uint16_t Length,
			                                 uint16_t* const BytesProcessed)
{
	uint16_t i;

	for(i=0;i<Length;i++)
	{
		Endpoint_Write_8(((uint8_t*)Buffer)[Length -1 -i]);
	}
	return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Read_Stream_LE(void* const Buffer,
			                                uint16_t Length,
			                                uint16_t* const BytesProcessed)
{
	uint16_t i;

	for(i=0;i<Length;i++)
	{
		((uint8_t*)Buffer)[i] = Endpoint_Read_8();
	}
	return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Read_Stream_BE(void* const Buffer,
			                                uint16_t Length,
			                                uint16_t* const BytesProcessed)
{
	return ENDPOINT_RWSTREAM_NoError;
}

#endif


uint8_t Endpoint_Write_Control_Stream_LE(const void* const Buffer,
			                                         uint16_t Length)
{
	Endpoint_Write_Stream_LE((uint8_t*)Buffer, MIN(Length, USB_ControlRequest.wLength), NULL);
	Endpoint_ClearIN();
// 	while (!(Endpoint_IsOUTReceived()))
// 	{
// 	}
	return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Endpoint_Write_Control_Stream_BE(const void* const Buffer,
			                                         uint16_t Length)
{
	return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Endpoint_Read_Control_Stream_LE(void* const Buffer,
			                                        uint16_t Length)
{
	return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Endpoint_Read_Control_Stream_BE(void* const Buffer,
			                                        uint16_t Length)
{
	return ENDPOINT_RWCSTREAM_NoError;
}

#endif