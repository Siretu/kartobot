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

#include "../PipeStream.h"

uint8_t Pipe_Discard_Stream(uint16_t Length,
                            uint16_t* const BytesProcessed)
{
	uint8_t  ErrorCode;
	uint16_t BytesInTransfer = 0;
	
//	Pipe_SetPipeToken(PIPE_TOKEN_IN);

	if ((ErrorCode = Pipe_WaitUntilReady()))
	  return ErrorCode;

	if (BytesProcessed != NULL)
	  Length -= *BytesProcessed;

	while (Length)
	{
		if (!(Pipe_IsReadWriteAllowed()))
		{
			Pipe_ClearIN();
				
			if (BytesProcessed != NULL)
			{
				*BytesProcessed += BytesInTransfer;
				return PIPE_RWSTREAM_IncompleteTransfer;
			}

			if ((ErrorCode = Pipe_WaitUntilReady()))
			  return ErrorCode;
		}
		else
		{
			Pipe_Discard_8();
			
			Length--;
			BytesInTransfer++;
		}
	}

	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Null_Stream(uint16_t Length,
                         uint16_t* const BytesProcessed)
{
	if (BytesProcessed != NULL)
	  Length -= *BytesProcessed;

	while (Length)
	{
		Pipe_Write_8(0);
		Length--;
	}

	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Write_Stream_LE(const void* const Buffer,
			                 uint16_t Length,
			                 uint16_t* const BytesProcessed)
{
	uint8_t* DataStream = (uint8_t*) Buffer;
	if(BytesProcessed != NULL)
	{
		Length -= *BytesProcessed;
		DataStream += *BytesProcessed;
	}

	while(Length)
	{
		Pipe_Write_8(*DataStream);
		DataStream++;
		Length--;
	}

	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Read_Stream_LE(void* const Buffer,
			                uint16_t Length,
			                uint16_t* const BytesProcessed)
{
	uint8_t* DataStream = (uint8_t *) Buffer;

	if(BytesProcessed != NULL)
	{
		Length -= *BytesProcessed;
		DataStream += *BytesProcessed;
	}

	if(Length > byte_transfered)
	{
		if (HCD_STATUS_OK != HcdDataTransfer(pipehandle[pipeselected], &usb_data_buffer[byte_transfered], Length-byte_transfered, NULL))
		{
			return PIPE_RWSTREAM_IncompleteTransfer;
		}
		usb_data_buffer_index = 0;
	}

	while(Length)
	{
		*DataStream = Pipe_Read_8();
		DataStream++;
		Length--;
	}
	byte_transfered = 0; // TODO later
	return PIPE_RWSTREAM_NoError;
}
uint8_t Pipe_Write_Stream_BE(const void* const Buffer,
			                             uint16_t Length,
			                             uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Read_Stream_BE(void* const Buffer,
			                            uint16_t Length,
			                            uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Write_PStream_LE(const void* const Buffer,
			                              uint16_t Length,
			                              uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Write_PStream_BE(const void* const Buffer,
			                              uint16_t Length,
			                              uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Write_EStream_LE(const void* const Buffer,
			                              uint16_t Length,
			                              uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Write_EStream_BE(const void* const Buffer,
			                              uint16_t Length,
			                              uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Read_EStream_LE(void* const Buffer,
			                             uint16_t Length,
			                             uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

uint8_t Pipe_Read_EStream_BE(void* const Buffer,
			                             uint16_t Length,
			                             uint16_t* const BytesProcessed)
{
	return PIPE_RWSTREAM_NoError;
}

#endif

