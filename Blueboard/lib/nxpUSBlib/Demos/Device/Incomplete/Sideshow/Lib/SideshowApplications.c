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


#include "SideshowApplications.h"

SideShow_Application_t InstalledApplications[MAX_APPLICATIONS];


SideShow_Application_t* SideShow_GetFreeApplication(void)
{
	for (uint8_t App = 0; App < ARRAY_ELEMENTS(InstalledApplications); App++)
	{
		if (!(InstalledApplications[App].InUse))
		  return &InstalledApplications[App];
	}

	return NULL;
}

SideShow_Application_t* SideShow_GetApplicationFromGUID(GUID_t* const GUID)
{
	for (uint8_t App = 0; App < ARRAY_ELEMENTS(InstalledApplications); App++)
	{
		if (InstalledApplications[App].InUse)
		{
			if (GUID_COMPARE(&InstalledApplications[App].ApplicationID, GUID))
			  return &InstalledApplications[App];
		}
	}

	return NULL;
}

