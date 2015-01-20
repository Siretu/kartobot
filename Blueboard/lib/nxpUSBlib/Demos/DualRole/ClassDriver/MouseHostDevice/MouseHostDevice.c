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
 *  Main source file for the MouseHostDevice demo. This file contains the main tasks of
 *  the demo and is responsible for the overall control flow of the demo.
 */

#include "MouseHostDevice.h"

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();

	puts_P(PSTR(ESC_FG_CYAN "Mouse Host/Device Demo running.\r\n" ESC_FG_WHITE));

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	sei();

	for (;;)
	{
		/* Determine which USB mode we are currently in */
		if (USB_CurrentMode == USB_MODE_Host)
		{
			MouseHostTask();
			HID_Host_USBTask(&Mouse_HID_Host_Interface);
		}
		else
		{
			HID_Device_USBTask(&Mouse_HID_Device_Interface);
		}

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
	Serial_Init(9600, false);
	LEDs_Init();
	Joystick_Init();
	Buttons_Init();
	USB_Init(USB_MODE_UID);

	/* Create a stdio stream for the serial port for stdin and stdout */
	Serial_CreateStream(NULL);
}

/** Event handler for the library USB mode change event. */
void EVENT_USB_UIDChange(void)
{
	printf_P(PSTR(ESC_FG_YELLOW "UID Change to %S mode\r\n" ESC_FG_WHITE),
	         (USB_CurrentMode == USB_MODE_Device) ? PSTR("Device") : PSTR("Host"));

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

