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
 *  USB Device Configuration Descriptor processing routines, to determine the correct pipe configurations
 *  needed to communication with an attached USB device. Descriptors are special  computer-readable structures
 *  which the host requests upon device enumeration, to determine the device's capabilities and functions.
 */

#include "ConfigDescriptor.h"

/** Reads and processes an attached device's descriptors, to determine compatibility and pipe configurations. This
 *  routine will read in the entire configuration descriptor, and configure the hosts pipes to correctly communicate
 *  with compatible devices.
 *
 *  This routine searches for a HID interface descriptor containing at least one Interrupt type IN endpoint.
 *
 *  \return An error code from the \ref GenericHIDHost_GetConfigDescriptorDataCodes_t enum.
 */
uint8_t ProcessConfigurationDescriptor(void)
{
	uint8_t  ConfigDescriptorData[512];
	void*    CurrConfigLocation = ConfigDescriptorData;
	uint16_t CurrConfigBytesRem;

	USB_Descriptor_Interface_t* HIDInterface    = NULL;
	USB_Descriptor_Endpoint_t*  DataINEndpoint  = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint = NULL;

	/* Retrieve the entire configuration descriptor into the allocated buffer */
	switch (USB_Host_GetDeviceConfigDescriptor(1, &CurrConfigBytesRem, ConfigDescriptorData, sizeof(ConfigDescriptorData)))
	{
		case HOST_GETCONFIG_Successful:
			break;
		case HOST_GETCONFIG_InvalidData:
			return InvalidConfigDataReturned;
		case HOST_GETCONFIG_BuffOverflow:
			return DescriptorTooLarge;
		default:
			return ControlError;
	}

	while (!(DataINEndpoint) || !(DataOUTEndpoint))
	{
		/* See if we've found a likely compatible interface, and if there is an endpoint within that interface */
		if (!(HIDInterface) ||
		    USB_GetNextDescriptorComp(&CurrConfigBytesRem, &CurrConfigLocation,
		                              DComp_NextHIDInterfaceDataEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			/* Not all HID devices have an OUT endpoint - if we've reached the end of the HID descriptor
			 * but only found the mandatory IN endpoint, it's safe to continue with the device enumeration */
			if (DataINEndpoint)
			  break;

			/* Get the next HID interface from the configuration descriptor */
			if (USB_GetNextDescriptorComp(&CurrConfigBytesRem, &CurrConfigLocation,
										  DComp_NextHIDInterface) != DESCRIPTOR_SEARCH_COMP_Found)
			{
				/* Descriptor not found, error out */
				return NoCompatibleInterfaceFound;
			}

			/* Save the interface in case we need to refer back to it later */
			HIDInterface = DESCRIPTOR_PCAST(CurrConfigLocation, USB_Descriptor_Interface_t);

			/* Clear any found endpoints */
			DataOUTEndpoint = NULL;

			/* Skip the remainder of the loop as we have not found an endpoint yet */
			continue;
		}

		/* Retrieve the endpoint address from the endpoint descriptor */
		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(CurrConfigLocation, USB_Descriptor_Endpoint_t);

		/* If the endpoint is a IN type endpoint */
		if (EndpointData->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
		  DataINEndpoint = EndpointData;
		else
		  DataOUTEndpoint = EndpointData;
	}

	/* Configure the HID data IN pipe */
	Pipe_ConfigurePipe(HID_DATA_IN_PIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
	                   DataINEndpoint->EndpointAddress, DataINEndpoint->EndpointSize, PIPE_BANK_SINGLE);
	Pipe_SetInterruptPeriod(DataINEndpoint->PollingIntervalMS);

	/* Check if the HID interface contained an optional OUT data endpoint */
	if (DataOUTEndpoint)
	{
		/* Configure the HID data OUT pipe */
		Pipe_ConfigurePipe(HID_DATA_OUT_PIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_OUT,
						   DataOUTEndpoint->EndpointAddress, DataOUTEndpoint->EndpointSize, PIPE_BANK_SINGLE);
	}

	/* Valid data found, return success */
	return SuccessfulConfigRead;
}

/** Descriptor comparator function. This comparator function is can be called while processing an attached USB device's
 *  configuration descriptor, to search for a specific sub descriptor. It can also be used to abort the configuration
 *  descriptor processing if an incompatible descriptor configuration is found.
 *
 *  This comparator searches for the next Interface descriptor of the correct HID Class value.
 *
 *  \return A value from the DSEARCH_Return_ErrorCodes_t enum
 */
uint8_t DComp_NextHIDInterface(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	/* Determine if the current descriptor is an interface descriptor */
	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		/* Check the HID descriptor class, break out if correct class/protocol interface found */
		if (Interface->Class == HID_CSCP_HIDClass)
		{
			/* Indicate that the descriptor being searched for has been found */
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	/* Current descriptor does not match what this comparator is looking for */
	return DESCRIPTOR_SEARCH_NotFound;
}

/** Descriptor comparator function. This comparator function is can be called while processing an attached USB device's
 *  configuration descriptor, to search for a specific sub descriptor. It can also be used to abort the configuration
 *  descriptor processing if an incompatible descriptor configuration is found.
 *
 *  This comparator searches for the next Endpoint descriptor inside the current interface descriptor,
 *  aborting the search if another interface descriptor is found before the required endpoint.
 *
 *  \return A value from the DSEARCH_Return_ErrorCodes_t enum
 */
uint8_t DComp_NextHIDInterfaceDataEndpoint(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	/* Determine the type of the current descriptor */
	if (Header->Type == DTYPE_Endpoint)
	{
		/* Indicate that the descriptor being searched for has been found */
		return DESCRIPTOR_SEARCH_Found;
	}
	else if (Header->Type == DTYPE_Interface)
	{
		/* Indicate that the search has failed prematurely and should be aborted */
		return DESCRIPTOR_SEARCH_Fail;
	}

	/* Current descriptor does not match what this comparator is looking for */
	return DESCRIPTOR_SEARCH_NotFound;
}

