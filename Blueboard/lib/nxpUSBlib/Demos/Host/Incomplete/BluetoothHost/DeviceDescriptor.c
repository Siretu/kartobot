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
 *  USB Device Descriptor processing routines, to determine the overall device parameters. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine information about
 *  the attached device.
 */

#include "DeviceDescriptor.h"

/** Reads and processes an attached device's Device Descriptor, to determine compatibility
 *
 *  This routine checks to ensure that the attached device's class codes match those for Bluetooth devices.
 *
 *  \return An error code from the \ref BluetoothHost_GetDeviceDescriptorDataCodes_t enum.
 */
uint8_t ProcessDeviceDescriptor(void)
{
	USB_Descriptor_Device_t DeviceDescriptor;

	/* Send the request to retrieve the device descriptor */
	if (USB_Host_GetDeviceDescriptor(&DeviceDescriptor) != HOST_SENDCONTROL_Successful)
	  return DevControlError;

	/* Validate returned data - ensure the returned data is a device descriptor */
	if (DeviceDescriptor.Header.Type != DTYPE_Device)
	  return InvalidDeviceDataReturned;

	/* Validate returned device Class, SubClass and Protocol values against the Bluetooth spec values */
	if ((DeviceDescriptor.Class    != BLUETOOTH_DEVICE_CLASS)    ||
	    (DeviceDescriptor.SubClass != BLUETOOTH_DEVICE_SUBCLASS) ||
	    (DeviceDescriptor.Protocol != BLUETOOTH_DEVICE_PROTOCOL))
	{
		return IncorrectBTDevice;
	}

	return SuccessfulDeviceRead;
}

