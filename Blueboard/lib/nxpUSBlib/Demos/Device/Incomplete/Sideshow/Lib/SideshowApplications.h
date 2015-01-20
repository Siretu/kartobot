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


#ifndef _SIDESHOW_APPLICATIONS_H_
#define _SIDESHOW_APPLICATIONS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		#include <stdbool.h>

		#include "SideshowCommon.h"

	/* Type Defines: */
		typedef struct
		{
			bool                   InUse;
			GUID_t                 ApplicationID;
			GUID_t                 EndpointID;
			UNICODE_STRING_t(50)   ApplicationName;
			uint32_t               CachePolicy;
			uint32_t               OnlineOnly;
			bool                   HaveContent;
			uint32_t               CurrentContentID;
			uint8_t                CurrentContent[MAX_CONTENTBUFFER_PER_APP];
		} SideShow_Application_t;

	/* External Variables: */
		extern SideShow_Application_t InstalledApplications[MAX_APPLICATIONS];

	/* Function Prototypes: */
		SideShow_Application_t* SideShow_GetFreeApplication(void);
		SideShow_Application_t* SideShow_GetApplicationFromGUID(GUID_t* const GUID);

#endif

