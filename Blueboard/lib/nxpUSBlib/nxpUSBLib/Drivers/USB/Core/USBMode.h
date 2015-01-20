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
 *  \brief USB mode and feature support definitions.
 *  \copydetails Group_USBMode
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in LUFA/Drivers/USB/USB.h.
 */

/** \ingroup Group_USB
 *  \defgroup Group_USBMode USB Mode Tokens
 *  \brief USB mode and feature support definitions.
 *
 *  This file defines macros indicating the type of USB controller the library is being compiled for, and its
 *  capabilities. These macros may then be referenced in the user application to selectively enable or disable
 *  code sections depending on if they are defined or not.
 *
 *  After the inclusion of the master USB driver header, one or more of the following tokens may be defined, to
 *  allow the user code to conditionally enable or disable code based on the USB controller family and allowable
 *  USB modes. These tokens may be tested against to eliminate code relating to a USB mode which is not enabled for
 *  the given compilation.
 *
 *  @{
 */

#ifndef __USBMODE_H__
#define __USBMODE_H__

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include LUFA/Drivers/USB/USB.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
	#if defined(__DOXYGEN__)
		/** Indicates that the target AVR microcontroller belongs to the Series 2 AVR8 USB controller
		 *  (i.e. AT90USBxxx2 or ATMEGAxxU2) when defined.
		 */
		#define USB_SERIES_2_AVR

		/** Indicates that the target AVR microcontroller belongs to the Series 4 AVR8 USB controller
		 *  (i.e. ATMEGAxxU4) when defined.
		 */
		#define USB_SERIES_4_AVR

		/** Indicates that the target AVR microcontroller belongs to the Series 6 AVR8 USB controller
		 *  (i.e. AT90USBxxx6) when defined.
		 */
		#define USB_SERIES_6_AVR

		/** Indicates that the target AVR microcontroller belongs to the Series 7 AVR8 USB controller
		 *  (i.e. AT90USBxxx7) when defined.
		 */
		#define USB_SERIES_7_AVR

		/** Indicates that the target AVR microcontroller belongs to the AVR32 UC3A0 Series USB controller
		 *  (i.e. AT32UC3A0*) when defined.
		 */
		#define USB_SERIES_UC3A0_AVR
		
		/** Indicates that the target AVR microcontroller belongs to the AVR32 UC3A1 Series USB controller
		 *  (i.e. AT32UC3A1*) when defined.
		 */
		#define USB_SERIES_UC3A1_AVR

		/** Indicates that the target AVR microcontroller belongs to the AVR32 UC3A3 Series USB controller
		 *  (i.e. AT32UC3A3*) when defined.
		 */
		#define USB_SERIES_UC3A3_AVR

		/** Indicates that the target AVR microcontroller belongs to the AVR32 UC3B0 Series USB controller
		 *  (i.e. AT32UC3B0*) when defined.
		 */
		#define USB_SERIES_UC3B0_AVR

		/** Indicates that the target AVR microcontroller belongs to the AVR32 UC3B1 Series USB controller
		 *  (i.e. AT32UC3B1*) when defined.
		 */
		#define USB_SERIES_UC3B1_AVR

		/** Indicates that the target microcontroller and compilation settings allow for the
		 *  target to be configured in USB Device mode when defined.
		 */
		#define USB_CAN_BE_DEVICE

		/** Indicates that the target microcontroller and compilation settings allow for the
		 *  target to be configured in USB Host mode when defined.
		 */
		#define USB_CAN_BE_HOST

		/** Indicates that the target microcontroller and compilation settings allow for the
		 *  target to be configured in either USB Device or Host mode when defined.
		 */
		#define USB_CAN_BE_BOTH
	#else
		/* Macros: */
			#if (defined(__AVR_AT90USB162__) || defined(__AVR_AT90USB82__)  || \
			     defined(__AVR_ATmega32U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega8U2__))
				#define USB_SERIES_2_AVR
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__))
				#define USB_SERIES_4_AVR
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_ATmega32U6__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__))
				#define USB_SERIES_6_AVR
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_AT90USB647__) || defined(__AVR_AT90USB1287__))
				#define USB_SERIES_7_AVR
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3A0512__) || defined(__AVR32_UC3A0256__) || \
			       defined(__AVR32_UC3A0128__) || defined(__AVR32_UC3A064__))
				#define USB_SERIES_UC3A0_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3A1512__) || defined(__AVR32_UC3A1256__) || \
			       defined(__AVR32_UC3A1128__) || defined(__AVR32_UC3A164__))
				#define USB_SERIES_UC3A1_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3A3256__) || defined(__AVR32_UC3A3256S__) || \
			       defined(__AVR32_UC3A3128__) || defined(__AVR32_UC3A3128S__) || \
			       defined(__AVR32_UC3A364__)  || defined(__AVR32_UC3A364S__))
				#define USB_SERIES_UC3A3_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3A4256__) || defined(__AVR32_UC3A4256S__) || \
			       defined(__AVR32_UC3A4128__) || defined(__AVR32_UC3A4128S__) || \
			       defined(__AVR32_UC3A464__)  || defined(__AVR32_UC3A464S__))
				#define USB_SERIES_UC3A4_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3B0512__) || defined(__AVR32_UC3B0256__) || \
			       defined(__AVR32_UC3B0128__) || defined(__AVR32_UC3B064__))
				#define USB_SERIES_UC3B0_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR32_UC3B1512__) || defined(__AVR32_UC3B1256__) || \
			       defined(__AVR32_UC3B1128__) || defined(__AVR32_UC3B164__))
				#define USB_SERIES_UC3B1_AVR32
				#define USB_CAN_BE_DEVICE
				#define USB_CAN_BE_HOST
			#elif (defined(__AVR_ATxmega128A1U__))
				#define USB_SERIES_A1U_XMEGA
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_ATxmega16A4U__) || defined(__AVR_ATxmega32A4U__))
				#define USB_SERIES_A4U_XMEGA
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_ATxmega64A3U__) || defined(__AVR_ATxmega128A3U__) || \
			       defined(__AVR_ATxmega192A3U__) || defined(__AVR_ATxmega256A3U__))
				#define USB_SERIES_A3U_XMEGA
				#define USB_CAN_BE_DEVICE
			#elif (defined(__AVR_ATxmega256A3BU__))
				#define USB_SERIES_A3BU_XMEGA
				#define USB_CAN_BE_DEVICE
			#elif (defined(__LPC17XX__))
				#define USB_CAN_BE_HOST
				#define __LPC_OHCI__
				#define USB_CAN_BE_DEVICE
			#elif (defined (__LPC18XX__))
				#define USB_CAN_BE_HOST
				#define __LPC_EHCI__
				#define USB_CAN_BE_DEVICE
			#elif (defined(__LPC11UXX__))
				#define USB_CAN_BE_DEVICE
			#endif
			
			#if (defined(USB_CAN_BE_DEVICE) && defined(USB_CAN_BE_HOST))
				#define USB_CAN_BE_BOTH
			#endif

			#if defined(USB_HOST_ONLY)
				#if !defined(USB_CAN_BE_HOST)
					#error USB_HOST_ONLY is not available for the currently selected microcontroller model.
				#else
					#undef USB_CAN_BE_DEVICE
					#undef USB_CAN_BE_BOTH
				#endif
			#endif

			#if defined(USB_DEVICE_ONLY)
				#if !defined(USB_CAN_BE_DEVICE)
					#error USB_DEVICE_ONLY is not available for the currently selected microcontroller model.
				#else
					#undef USB_CAN_BE_HOST
					#undef USB_CAN_BE_BOTH
				#endif
			#endif
			
			#if (defined(USB_HOST_ONLY) && defined(USB_DEVICE_ONLY))
				#error USB_HOST_ONLY and USB_DEVICE_ONLY are mutually exclusive.
			#endif

			#if (!defined(USB_CAN_BE_DEVICE) && !defined(USB_CAN_BE_HOST))
				#error The currently selected device or architecture is not supported under the USB component of the library.
			#endif
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */