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
 *  Header file for PrinterCommands.c.
 */

#ifndef _PRINTER_COMMANDS_H_
#define _PRINTER_COMMANDS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include <LUFA/Drivers/USB/USB.h>

		#include "../PrinterHost.h"

	/* Function Prototypes: */
		uint8_t Printer_SendData(const void* const PrinterCommands,
		                         const uint16_t CommandSize);
		uint8_t Printer_GetDeviceID(char* DeviceIDString,
		                            const uint16_t BufferSize);
		uint8_t Printer_GetPortStatus(uint8_t* const PortStatus);
		uint8_t Printer_SoftReset(void);

#endif

