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
 *  \brief Master include file for the library USB MIDI Class driver.
 *
 *  Master include file for the library USB MIDI Class driver, for both host and device modes, where available.
 *
 *  This file should be included in all user projects making use of this optional class driver, instead of
 *  including any headers in the USB/ClassDriver/Device, USB/ClassDriver/Host or USB/ClassDriver/Common subdirectories.
 */

/** \ingroup Group_USBClassDrivers
 *  \defgroup Group_USBClassMIDI MIDI Class Driver
 *
 *  \section Sec_Dependencies Module Source Dependencies
 *  The following files must be built with any user project that uses this module:
 *    - LUFA/Drivers/USB/Class/Device/MIDI.c <i>(Makefile source module name: LUFA_SRC_USBCLASS)</i>
 *    - LUFA/Drivers/USB/Class/Host/MIDI.c <i>(Makefile source module name: LUFA_SRC_USBCLASS)</i>
 *
 *  \section Sec_ModDescription Module Description
 *  MIDI Class Driver module. This module contains an internal implementation of the USB MIDI Class, for both Device
 *  and Host USB modes. User applications can use this class driver instead of implementing the MIDI class manually
 *  via the low-level LUFA APIs.
 *
 *  This module is designed to simplify the user code by exposing only the required interface needed to interface with
 *  Hosts or Devices using the USB MIDI Class.
 *
 *  \note The USB MIDI class is actually a special case of the regular Audio class, thus this module depends on
 *        structure definitions from the \ref Group_USBClassAudioDevice class driver module.
 *
 *  @{
 */

#ifndef _MIDI_CLASS_H_
#define _MIDI_CLASS_H_

	/* Macros: */
		#define __INCLUDE_FROM_USB_DRIVER
		#define __INCLUDE_FROM_MIDI_DRIVER

	/* Includes: */
		#include "../Core/USBMode.h"

		#if defined(USB_CAN_BE_DEVICE)
			#include "Device/MIDI.h"
		#endif

		#if defined(USB_CAN_BE_HOST)
			#include "Host/MIDI.h"
		#endif

#endif

/** @} */

