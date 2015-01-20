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
 *  Main source file for the Joystick demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "Joystick.h"

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	sei();

	for (;;)
	{
		HID_Task();
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
	Joystick_Init();
	LEDs_Init();
	Buttons_Init();
	USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the USB management and joystick reporting tasks.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the joystick reporting task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoint */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
	                                            JOYSTICK_EPSIZE, ENDPOINT_BANK_SINGLE);

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_JoystickReport_Data_t JoystickReportData;

				/* Create the next HID report to send to the host */
				GetNextReport(&JoystickReportData);

				Endpoint_ClearSETUP();

				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&JoystickReportData, sizeof(JoystickReportData));
				Endpoint_ClearOUT();
			}

			break;
	}
}

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 *
 *  \return Boolean true if the new report differs from the last report, false otherwise
 */
bool GetNextReport(USB_JoystickReport_Data_t* const ReportData)
{
	static uint8_t PrevJoyStatus    = 0;
	static uint8_t PrevButtonStatus = 0;
	uint8_t        JoyStatus_LCL    = Joystick_GetStatus();
	uint8_t        ButtonStatus_LCL = Buttons_GetStatus();
	bool           InputChanged     = false;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_JoystickReport_Data_t));

	if (JoyStatus_LCL & JOY_UP)
	  ReportData->Y = -100;
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportData->Y =  100;

	if (JoyStatus_LCL & JOY_LEFT)
	  ReportData->X = -100;
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportData->X =  100;

	if (JoyStatus_LCL & JOY_PRESS)
	  ReportData->Button |= (1 << 1);

	if (ButtonStatus_LCL & BUTTONS_BUTTON1)
	  ReportData->Button |= (1 << 0);

	/* Check if the new report is different to the previous report */
	InputChanged = (uint8_t)(PrevJoyStatus ^ JoyStatus_LCL) | (uint8_t)(PrevButtonStatus ^ ButtonStatus_LCL);

	/* Save the current joystick status for later comparison */
	PrevJoyStatus    = JoyStatus_LCL;
	PrevButtonStatus = ButtonStatus_LCL;

	/* Return whether the new report is different to the previous report or not */
	return InputChanged;
}

/** Function to manage HID report generation and transmission to the host. */
void HID_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Select the Joystick Report Endpoint */
	Endpoint_SelectEndpoint(JOYSTICK_EPNUM);

	/* Check to see if the host is ready for another packet */
	if (Endpoint_IsINReady())
	{
		USB_JoystickReport_Data_t JoystickReportData;

		/* Create the next HID report to send to the host */
		GetNextReport(&JoystickReportData);

		/* Write Joystick Report Data */
		Endpoint_Write_Stream_LE(&JoystickReportData, sizeof(JoystickReportData), NULL);

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();

		/* Clear the report data afterwards */
		memset(&JoystickReportData, 0, sizeof(JoystickReportData));
	}
}

