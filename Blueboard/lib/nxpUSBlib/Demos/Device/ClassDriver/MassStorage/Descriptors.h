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


/** \file
 *
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_
#if ARCH==ARCH_LPC
		#include "USB.h"
#else
	/* Includes: */
		#include <avr/pgmspace.h>
		#include <LUFA/Drivers/USB/USB.h>
#endif

	/* Macros: */
		/** Endpoint number of the Mass Storage device-to-host data IN endpoint. */
#ifdef __LPC17XX__
		#define MASS_STORAGE_IN_EPNUM          5
#else
		#define MASS_STORAGE_IN_EPNUM          3
#endif
		/** Endpoint number of the Mass Storage host-to-device data OUT endpoint. */
		#define MASS_STORAGE_OUT_EPNUM         2

		/** Size in bytes of the Mass Storage data endpoints. */
		#define MASS_STORAGE_IO_EPSIZE         64

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;

			// Mass Storage Interface
			USB_Descriptor_Interface_t            MS_Interface;
			USB_Descriptor_Endpoint_t             MS_DataInEndpoint;
			USB_Descriptor_Endpoint_t             MS_DataOutEndpoint;
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

