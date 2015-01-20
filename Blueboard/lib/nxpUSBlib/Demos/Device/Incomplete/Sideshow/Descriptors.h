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


#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

		#include "Sideshow.h"

	/* Macros: */
		#define SIDESHOW_IN_EPNUM          3
		#define SIDESHOW_OUT_EPNUM         4
		#define SIDESHOW_IO_EPSIZE         64

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            SSHOW_Interface;
			USB_Descriptor_Endpoint_t             SSHOW_DataInEndpoint;
			USB_Descriptor_Endpoint_t             SSHOW_DataOutEndpoint;
		} USB_Descriptor_Configuration_t;

		typedef struct
		{
			USB_Descriptor_Header_t                Header;

			int                                    Signature[7];
			uint16_t                               VendorCode;
		} USB_OSDescriptor_t;

		typedef struct
		{
			uint8_t                                FirstInterfaceNumber;

			uint8_t                                Reserved;

			uint8_t                                CompatibleID[8];
			uint8_t                                SubCompatibleID[8];

			uint8_t                                Reserved2[6];
		} USB_OSCompatibleSection_t;

		typedef struct
		{
			uint32_t                               TotalLength;
			uint16_t                               Version;
			uint16_t                               Index;
			uint8_t                                TotalSections;

			uint8_t                                Reserved[7];

			USB_OSCompatibleSection_t              SideshowCompatID;
		} USB_OSCompatibleIDDescriptor_t;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

		uint16_t USB_GetOSFeatureDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

