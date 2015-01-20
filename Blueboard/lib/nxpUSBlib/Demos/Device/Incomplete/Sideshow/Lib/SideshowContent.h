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


#ifndef _SIDESHOW_CONTENT_H_
#define _SIDESHOW_CONTENT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <string.h>
		#include <stdio.h>

		#include "SideshowCommon.h"
		#include "SideshowApplications.h"

	/* Enums: */
		enum SideShow_ContentTypes_t
		{
			Content_Menu            = 0,
			Content_Content         = 1,
			Content_MenuItem        = 2,
			Content_Button          = 3,
			Content_Text            = 4,
			Content_EndOfContent    = 5
		};

		enum SideShow_ActionTypes_t
		{
			TODO
		};

		enum SideShow_AlignmentTypes_t
		{
			TODO2
		};

	/* Type Defines: */
		typedef struct
		{
			uint8_t ContentType;
			uint8_t ContentSize;
		} SideShow_Content_Header_t;

		typedef struct
		{
			SideShow_Content_Header_t Header;

			uint32_t ItemID;
			uint8_t  ActionType;
			char     Title[];
		} SideShow_Content_Menu_t;

		typedef struct
		{
			SideShow_Content_Header_t Header;

			uint32_t ItemID;
			uint32_t Target;
			bool     IsSelected;
			char     Text[];
		} SideShow_Content_MenuItem_t;

		typedef struct
		{
			SideShow_Content_Header_t Header;

			uint8_t  Key;
			uint32_t Target;
		} SideShow_Content_Button_t;

		typedef struct
		{
			SideShow_Content_Header_t Header;

			uint32_t ItemID;
			uint32_t AssociatedMenuID;
			char     Title[];
		} SideShow_Content_Content_t;

		typedef struct
		{
			SideShow_Content_Header_t Header;

			char     Text[];
		} SideShow_Content_Text_t;

	/* Defines: */
		#define XML_START_TAG         "<body>"
		#define XML_END_TAG           "</body>"

	/* Function Prototypes: */
		bool SideShow_AddSimpleContent(SideShow_PacketHeader_t* const PacketHeader,
		                               SideShow_Application_t* const Application);

		#if defined(INCLUDE_FROM_SIDESHOWCONTENT_C)
			static void SideShow_ProcessXMLContent(void* ContentData,
			                                       uint32_t ContentSize);
		#endif

#endif
