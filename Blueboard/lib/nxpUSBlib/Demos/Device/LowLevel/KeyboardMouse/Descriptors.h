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

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            HID1_KeyboardInterface;
			USB_HID_Descriptor_HID_t              HID1_KeyboardHID;
	        USB_Descriptor_Endpoint_t             HID1_ReportINEndpoint;
	        USB_Descriptor_Endpoint_t             HID1_ReportOUTEndpoint;
			USB_Descriptor_Interface_t            HID2_MouseInterface;
			USB_HID_Descriptor_HID_t              HID2_MouseHID;
	        USB_Descriptor_Endpoint_t             HID2_ReportINEndpoint;
		} USB_Descriptor_Configuration_t;

	/* Macros: */
		/** Endpoint number of the Keyboard HID reporting IN endpoint. */
		#define KEYBOARD_IN_EPNUM         1

		/** Endpoint number of the Keyboard HID reporting OUT endpoint. */
		#define KEYBOARD_OUT_EPNUM        2

		/** Endpoint number of the Mouse HID reporting IN endpoint. */
		#define MOUSE_IN_EPNUM            3

		/** Size in bytes of each of the HID reporting IN and OUT endpoints. */
		#define HID_EPSIZE                8

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

