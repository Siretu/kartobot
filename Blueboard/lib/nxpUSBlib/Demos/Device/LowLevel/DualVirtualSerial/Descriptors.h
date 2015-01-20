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

	/* Macros: */
		/** Endpoint number of the first CDC interface's device-to-host data IN endpoint. */
		#define CDC1_TX_EPNUM                  1

		/** Endpoint number of the first CDC interface's host-to-device data OUT endpoint. */
		#define CDC1_RX_EPNUM                  2

		/** Endpoint number of the first CDC interface's device-to-host notification IN endpoint. */
		#define CDC1_NOTIFICATION_EPNUM        3

		/** Endpoint number of the second CDC interface's device-to-host data IN endpoint. */
		#define CDC2_TX_EPNUM                  4

		/** Endpoint number of the second CDC interface's host-to-device data OUT endpoint. */
		#define CDC2_RX_EPNUM                  5

		/** Endpoint number of the second CDC interface's device-to-host notification IN endpoint. */
		#define CDC2_NOTIFICATION_EPNUM        6

		/** Size in bytes of the CDC device-to-host notification IN endpoints. */
		#define CDC_NOTIFICATION_EPSIZE        8

		/** Size in bytes of the CDC data IN and OUT endpoints. */
		#define CDC_TXRX_EPSIZE                16

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;
			USB_Descriptor_Interface_Association_t   CDC1_IAD;
			USB_Descriptor_Interface_t               CDC1_CCI_Interface;
			USB_CDC_Descriptor_FunctionalHeader_t    CDC1_Functional_Header;
			USB_CDC_Descriptor_FunctionalACM_t       CDC1_Functional_ACM;
			USB_CDC_Descriptor_FunctionalUnion_t     CDC1_Functional_Union;
			USB_Descriptor_Endpoint_t                CDC1_ManagementEndpoint;
			USB_Descriptor_Interface_t               CDC1_DCI_Interface;
			USB_Descriptor_Endpoint_t                CDC1_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                CDC1_DataInEndpoint;
			USB_Descriptor_Interface_Association_t   CDC2_IAD;
			USB_Descriptor_Interface_t               CDC2_CCI_Interface;
			USB_CDC_Descriptor_FunctionalHeader_t    CDC2_Functional_Header;
			USB_CDC_Descriptor_FunctionalACM_t       CDC2_Functional_ACM;
			USB_CDC_Descriptor_FunctionalUnion_t     CDC2_Functional_Union;
			USB_Descriptor_Endpoint_t                CDC2_ManagementEndpoint;
			USB_Descriptor_Interface_t               CDC2_DCI_Interface;
			USB_Descriptor_Endpoint_t                CDC2_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                CDC2_DataInEndpoint;
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
