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


#include "SideshowCommon.h"

uint16_t SideShow_Read_Unicode_String(void* const UnicodeString,
                                      const uint16_t MaxBytes)
{
	Unicode_String_t* const UnicodeStruct = (Unicode_String_t*)UnicodeString;
	uint32_t                UnicodeCharsToRead;

	Endpoint_Read_Stream_LE(&UnicodeCharsToRead, sizeof(uint32_t), NULL);

	int UnicodeData[UnicodeCharsToRead];

	UnicodeStruct->LengthInBytes = (UnicodeCharsToRead << 1);

	Endpoint_Read_Stream_LE(&UnicodeData, UnicodeStruct->LengthInBytes, NULL);

	if (UnicodeStruct->LengthInBytes > MaxBytes)
	  UnicodeStruct->LengthInBytes = MaxBytes;

	memcpy(&UnicodeStruct->UnicodeString, &UnicodeData, UnicodeStruct->LengthInBytes);

	return ((UnicodeCharsToRead << 1) + sizeof(uint32_t));
}

void SideShow_Write_Unicode_String(void* const UnicodeString)
{
	Unicode_String_t* const UnicodeStruct = (Unicode_String_t*)UnicodeString;

	uint32_t StringSizeInCharacters = (UnicodeStruct->LengthInBytes >> 1);

	Endpoint_Write_Stream_LE(&StringSizeInCharacters, sizeof(uint32_t), NULL);
	Endpoint_Write_Stream_LE(&UnicodeStruct->UnicodeString, UnicodeStruct->LengthInBytes, NULL);
}

void SideShow_Discard_Byte_Stream(void)
{
	uint32_t StreamSize;

	Endpoint_Read_Stream_LE(&StreamSize, sizeof(uint32_t), NULL);
	Endpoint_Discard_Stream(StreamSize, NULL);
}

