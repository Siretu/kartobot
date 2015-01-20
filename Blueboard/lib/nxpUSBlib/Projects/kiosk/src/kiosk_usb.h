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
 *  Header file for MassStorage.c.
 */

#ifndef _MASS_STORAGE_HOST_H_
#define _MASS_STORAGE_HOST_H_

	/* Includes: */
#if ARCH==ARCH_LPC
		#include "configure.h"
		#include "USB.h"
#else
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/pgmspace.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>

		#include <LUFA/Version.h>
		#include <LUFA/Drivers/Misc/TerminalCodes.h>
		#include <LUFA/Drivers/Peripheral/Serial.h>
		#include <LUFA/Drivers/Board/LEDs.h>
		#include <LUFA/Drivers/USB/USB.h>
#endif
		#include <ctype.h>
		#include <stdio.h>

		#include "lcd_type.h"
		#include "fatfs/ff.h"

	/* Macros: */
		/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
		#define LEDMASK_USB_NOTREADY      LEDS_LED1

		/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
		#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

		/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
		#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

		/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
		#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

		/** LED mask for the library LED driver, to indicate that the USB interface is busy. */
		#define LEDMASK_USB_BUSY          LEDS_LED2

#define KIOSK_SLIDE_MAX 10

	/* Function Prototypes: */
		void SetupHardware(void);
		void MassStorageHost_Task(void);
		
		void EVENT_USB_Host_HostError(const uint8_t ErrorCode);
		void EVENT_USB_Host_DeviceAttached(void);
		void EVENT_USB_Host_DeviceUnattached(void);
		void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
		                                            const uint8_t SubErrorCode);
		void EVENT_USB_Host_DeviceEnumerationComplete(void);


		void kiosk_usb_setup();
		void kiosk_usb_task(void);
		void kiosk_usb_load_slides(void);
		uint32_t kiosk_get_slide_addr(int slideNum);
		int kiosk_get_num_slides(void);
		int kiosk_usb_ready(void);
		void kiosk_usb_run_slides(void);
		void kiosk_usb_stop_slides(void);


		extern USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface;
		extern int kioskUsbDeviceAttached;

#endif

