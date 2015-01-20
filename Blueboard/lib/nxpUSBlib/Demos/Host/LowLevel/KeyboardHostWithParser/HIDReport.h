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
 *  Header file for HIDReport.c.
 */

#ifndef _HID_REPORT_H_
#define _HID_REPORT_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include "KeyboardHostWithParser.h"

	/* Macros: */
		/** HID Report Descriptor Usage Page value for a desktop keyboard. */
		#define USAGE_PAGE_KEYBOARD             0x07

	/* Enums: */
		/** Enum for the possible return codes of the \ref GetHIDReportData() function. */
		enum KeyboardHostWithParser_GetHIDReportDataCodes_t
		{
			ParseSuccessful         = 0, /**< HID report descriptor parsed successfully */
			ParseError              = 1, /**< Failed to fully process the HID report descriptor */
			ParseControlError       = 2, /**< Control error occurred while trying to read the device HID descriptor */
		};

	/* External Variables: */
		extern uint16_t         HIDReportSize;
		extern HID_ReportInfo_t HIDReportInfo;

	/* Function Prototypes: */
		uint8_t GetHIDReportData(void);

		bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t* const CurrentItem);

#endif

