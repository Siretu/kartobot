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
 *  \brief Common definitions and declarations for the library USB Printer Class driver.
 *
 *  Common definitions and declarations for the library USB Printer Class driver.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB module driver
 *        dispatch header located in LUFA/Drivers/USB.h.
 */

/** \ingroup Group_USBClassPrinter
 *  \defgroup Group_USBClassPrinterCommon  Common Class Definitions
 *
 *  \section Sec_ModDescription Module Description
 *  Constants, Types and Enum definitions that are common to both Device and Host modes for the USB
 *  Printer Class.
 *
 *  @{
 */

#ifndef _PRINTER_CLASS_COMMON_H_
#define _PRINTER_CLASS_COMMON_H_

	/* Includes: */
		#include "../../Core/StdDescriptors.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_PRINTER_DRIVER)
			#error Do not include this file directly. Include LUFA/Drivers/USB.h instead.
		#endif

	/* Macros: */
		/** \name Virtual Printer Status Line Masks */
		//@{
		/** Port status mask for a printer device, indicating that an error has *not* occurred. */
		#define PRNT_PORTSTATUS_NOTERROR    (1 << 3)

		/** Port status mask for a printer device, indicating that the device is currently selected. */
		#define PRNT_PORTSTATUS_SELECT      (1 << 4)

		/** Port status mask for a printer device, indicating that the device is currently out of paper. */
		#define PRNT_PORTSTATUS_PAPEREMPTY  (1 << 5)
		//@}

	/* Enums: */
		/** Enum for possible Class, Subclass and Protocol values of device and interface descriptors relating to the Printer
		 *  device class.
		 */
		enum PRNT_Descriptor_ClassSubclassProtocol_t
		{
			PRNT_CSCP_PrinterClass          = 0x07, /**< Descriptor Class value indicating that the device or interface
			                                         *   belongs to the Printer class.
			                                         */
			PRNT_CSCP_PrinterSubclass       = 0x01, /**< Descriptor Subclass value indicating that the device or interface
			                                         *   belongs to the Printer subclass.
			                                         */
			PRNT_CSCP_BidirectionalProtocol = 0x02, /**< Descriptor Protocol value indicating that the device or interface
			                                         *   belongs to the Bidirectional protocol of the Printer class.
			                                         */
		};
	
		/** Enum for the Printer class specific control requests that can be issued by the USB bus host. */
		enum PRNT_ClassRequests_t
		{
			PRNT_REQ_GetDeviceID            = 0x00, /**< Printer class-specific request to retrieve the Unicode ID
			                                         *   string of the device, containing the device's name, manufacturer
			                                         *   and supported printer languages.
			                                         */
			PRNT_REQ_GetPortStatus          = 0x01, /**< Printer class-specific request to get the current status of the
			                                         *   virtual printer port, for device selection and ready states.
			                                         */
			PRNT_REQ_SoftReset              = 0x02, /**< Printer class-specific request to reset the device, ready for new
			                                         *   printer commands.
			                                         */
		};		

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

