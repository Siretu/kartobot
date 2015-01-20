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


#ifndef _CONFIGDESCRIPTOR_H_
#define _CONFIGDESCRIPTOR_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include "PrinterHost.h"
		#include "Lib/PrinterCommands.h"

	/* Macros: */
		/** Pipe number of the Printer data IN pipe. */
		#define PRINTER_DATA_IN_PIPE             1

		/** Pipe number of the Printer data OUT pipe. */
		#define PRINTER_DATA_OUT_PIPE            2

	/* Enums: */
		/** Enum for the possible return codes of the \ref ProcessConfigurationDescriptor() function. */
		enum PrinterHost_GetConfigDescriptorDataCodes_t
		{
			SuccessfulConfigRead            = 0, /**< Configuration Descriptor was processed successfully */
			ControlError                    = 1, /**< A control request to the device failed to complete successfully */
			DescriptorTooLarge              = 2, /**< The device's Configuration Descriptor is too large to process */
			InvalidConfigDataReturned       = 3, /**< The device returned an invalid Configuration Descriptor */
			NoCompatibleInterfaceFound      = 4, /**< A compatible interface with the required endpoints was not found */
		};

	/* External Variables: */
		/** Interface index of the Bidirectional Printer interface within the device, once the Configuration
		 *  Descriptor has been processed.
		 */
		extern uint8_t PrinterInterfaceNumber;

		/** Interface Alternate Setting index of the Bidirectional Printer interface within the device, once
		 *  the Configuration Descriptor has been processed.
		 */
		extern uint8_t PrinterAltSetting;

	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);

		uint8_t DComp_NextBidirectionalPrinterInterface(void* CurrentDescriptor);
		uint8_t DComp_NextPrinterInterfaceBulkDataEndpoint(void* CurrentDescriptor);

#endif

