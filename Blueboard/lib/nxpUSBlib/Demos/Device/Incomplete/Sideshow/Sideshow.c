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


/*
	SideShow Class demonstration application. This give a reference
	for implementing Microsoft SideShow compatible devices in an
	embedded environment. SideShow allows for gadget data displayed
	on a Windows Vista or later machine to also be displayed on an
	externally connected interactive display. Upon enumeration, this will
	appear as a new SideShow device which can have gadgets loaded onto
	it.

	Note that while the incoming content is buffered in packet struct
	form, the data is not actually displayed. It is left to the user to
	write sufficient code to read out the packed data for display to a
	screen.

	Installed gadgets can be accessed through the InstalledApplications
	array, with entries that have their InUse flag set being active. As
	only the active content is displayed on the device due to memory
	constraints, new content can be requested as needed.
*/

#include "Sideshow.h"

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	sei();

	for (;;)
	{
		SideShow_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	LEDs_Init();
	USB_Init();
	Serial_Init(9600, false);

	/* Create a stdio stream for the serial port for stdin and stdout */
	Serial_CreateStream(NULL);
}

void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup Sideshow Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(SIDESHOW_IN_EPNUM,  EP_TYPE_BULK, ENDPOINT_DIR_IN,
	                                            SIDESHOW_IO_EPSIZE, ENDPOINT_BANK_SINGLE);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(SIDESHOW_OUT_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_OUT,
	                                            SIDESHOW_IO_EPSIZE, ENDPOINT_BANK_SINGLE);

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

void EVENT_USB_Device_ControlRequest(void)
{
	switch (USB_ControlRequest.bRequest)
	{
		case REQ_GetOSFeatureDescriptor:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE))
			{
				const void* DescriptorPointer;
				uint16_t    DescriptorSize = USB_GetOSFeatureDescriptor(USB_ControlRequest.wValue,
				                                                        USB_ControlRequest.wIndex,
				                                                        &DescriptorPointer);

				if (DescriptorSize == NO_DESCRIPTOR)
				  return;

				Endpoint_ClearSETUP();

				Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
				Endpoint_ClearOUT();
			}

			break;
	}
}

void SideShow_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Select the SideShow data out endpoint */
	Endpoint_SelectEndpoint(SIDESHOW_OUT_EPNUM);

	/* Check to see if a new SideShow message has been received */
	if (Endpoint_IsReadWriteAllowed())
	{
		/* Process the received SideShow message */
		Sideshow_ProcessCommandPacket();
	}
}

