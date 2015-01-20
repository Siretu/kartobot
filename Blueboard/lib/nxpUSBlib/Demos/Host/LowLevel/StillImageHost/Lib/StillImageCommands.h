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
 *  Header file for StillImageCommands.c.
 */

#ifndef _STILL_IMAGE_COMMANDS_H_
#define _STILL_IMAGE_COMMANDS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include "PIMACodes.h"
		#include "../StillImageHost.h"

	/* Macros: */
		/** Timeout period between the issuing of a command to a device, and the reception of the first packet. */
		#define COMMAND_DATA_TIMEOUT_MS        10000

	/* External Variables: */
		extern PIMA_Container_t PIMA_SendBlock;
		extern PIMA_Container_t PIMA_ReceivedBlock;
		extern PIMA_Container_t PIMA_EventBlock;

	/* Function Prototypes: */
		void    SImage_SendBlockHeader(void);
		uint8_t SImage_ReceiveBlockHeader(void);
		uint8_t SImage_ReceiveEventHeader(void);
		uint8_t SImage_SendData(void* const Buffer,
		                        const uint16_t Bytes);
		uint8_t SImage_ReadData(void* const Buffer,
		                        const uint16_t Bytes);
		bool    SImage_IsEventReceived(void);

#endif

