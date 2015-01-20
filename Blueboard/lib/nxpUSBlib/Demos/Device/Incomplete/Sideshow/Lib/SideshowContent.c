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


#define  INCLUDE_FROM_SIDESHOWCONTENT_C
#include "SideshowContent.h"

bool SideShow_AddSimpleContent(SideShow_PacketHeader_t* const PacketHeader,
                               SideShow_Application_t* const Application)
{
	uint32_t ContentSize;
	uint32_t ContentID;

	Endpoint_Read_Stream_LE(&ContentID, sizeof(uint32_t), NULL);

	PacketHeader->Length -= sizeof(uint32_t);

	if (Application->CurrentContentID != ContentID)
	{
		Endpoint_Discard_Stream(PacketHeader->Length, NULL);

		return false;
	}

	Endpoint_Read_Stream_LE(&ContentSize, sizeof(uint32_t), NULL);
	Endpoint_Read_Stream_LE(&Application->CurrentContent, (sizeof(XML_START_TAG) - 1), NULL);

	PacketHeader->Length -= sizeof(uint32_t) + (sizeof(XML_START_TAG) - 1);

	if (!(memcmp(&Application->CurrentContent, XML_START_TAG, (sizeof(XML_START_TAG) - 1))))
	{
		SideShow_ProcessXMLContent(&Application->CurrentContent, (ContentSize - (sizeof(XML_END_TAG) - 1)));

		Endpoint_Discard_Stream((sizeof(XML_END_TAG) - 1), NULL);

		Application->HaveContent = true;
	}
	else
	{
		printf(" BINARY");
		Endpoint_Discard_Stream(ContentSize, NULL);
	}

	return true;
}

static void SideShow_ProcessXMLContent(void* ContentData,
                                       uint32_t ContentSize)
{
	printf(" XML");
	Endpoint_Discard_Stream(ContentSize, NULL);
}

