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
 *  Host Mode USB Mouse functionality for the MouseHostDevice demo. This file contains the Host mode
 *  USB Mouse related code of the demo and is responsible for all the Host mode Mouse functionality.
 */

#include "HostFunctions.h"

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Host_t Mouse_HID_Host_Interface =
	{
		.Config =
			{
				.DataINPipeNumber       = 1,
				.DataOUTPipeNumber      = 2,

				.HIDInterfaceProtocol   = HID_CSCP_MouseBootProtocol,
			},
	};


/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(void)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(void)
{
	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_READY);
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t ErrorCode)
{
	USB_Disable();

	printf_P(PSTR(ESC_FG_RED "Host Mode Error\r\n"
	                         " -- Error Code %d\r\n" ESC_FG_WHITE), ErrorCode);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	printf_P(PSTR(ESC_FG_RED "Dev Enum Error\r\n"
	                         " -- Error Code %d\r\n"
	                         " -- Sub Error Code %d\r\n"
	                         " -- In State %d\r\n" ESC_FG_WHITE), ErrorCode, SubErrorCode, USB_HostState);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
}

/** Host state machine task. This task handles the enumeration and control of USB Mice while in USB Host mode,
 *  setting up the appropriate data pipes and processing reports from the attached device.
 */
void MouseHostTask(void)
{
	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

			uint16_t ConfigDescriptorSize;
			uint8_t  ConfigDescriptorData[512];

			if (USB_Host_GetDeviceConfigDescriptor(1, &ConfigDescriptorSize, ConfigDescriptorData,
			                                       sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful)
			{
				printf("Error Retrieving Configuration Descriptor.\r\n");
				LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
				USB_HostState = HOST_STATE_WaitForDeviceRemoval;
				break;
			}

			if (HID_Host_ConfigurePipes(&Mouse_HID_Host_Interface,
			                            ConfigDescriptorSize, ConfigDescriptorData) != HID_ENUMERROR_NoError)
			{
				printf("Attached Device Not a Valid Mouse.\r\n");
				LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
				USB_HostState = HOST_STATE_WaitForDeviceRemoval;
				break;
			}

			if (USB_Host_SetDeviceConfiguration(1) != HOST_SENDCONTROL_Successful)
			{
				printf("Error Setting Device Configuration.\r\n");
				LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
				USB_HostState = HOST_STATE_WaitForDeviceRemoval;
				break;
			}

			if (HID_Host_SetBootProtocol(&Mouse_HID_Host_Interface) != HOST_SENDCONTROL_Successful)
			{
				printf("Could not Set Boot Protocol Mode.\r\n");
				LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
				USB_HostState = HOST_STATE_WaitForDeviceRemoval;
				break;
			}

			printf("Mouse Enumerated.\r\n");
			USB_HostState = HOST_STATE_Configured;
			break;
		case HOST_STATE_Configured:
			if (HID_Host_IsReportReceived(&Mouse_HID_Host_Interface))
			{
				uint8_t LEDMask  = LEDS_NO_LEDS;

				USB_MouseReport_Data_t MouseReport;
				HID_Host_ReceiveReport(&Mouse_HID_Host_Interface, &MouseReport);

				printf_P(PSTR("dX:%2d dY:%2d Button:%d\r\n"), MouseReport.X,
															  MouseReport.Y,
															  MouseReport.Button);
				if (MouseReport.X > 0)
				  LEDMask |= LEDS_LED1;
				else if (MouseReport.X < 0)
				  LEDMask |= LEDS_LED2;

				if (MouseReport.Y > 0)
				  LEDMask |= LEDS_LED3;
				else if (MouseReport.Y < 0)
				  LEDMask |= LEDS_LED4;

				if (MouseReport.Button)
				  LEDMask  = LEDS_ALL_LEDS;

				LEDs_SetAllLEDs(LEDMask);
			}

			break;
	}
}

