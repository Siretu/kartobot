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
 *  Header file for MassStoreCommands.c.
 */

#ifndef _MASS_STORE_COMMANDS_H_
#define _MASS_STORE_COMMANDS_H_

	/* Includes: */
		#include <avr/io.h>

		#include "MassStorageHost.h"

		#include <LUFA/Drivers/USB/USB.h>

	/* Macros: */
		/** Timeout period between the issuing of a CBW to a device, and the reception of the first packet. */
		#define COMMAND_DATA_TIMEOUT_MS             10000

		/** Additional error code for Mass Storage functions when a device returns a logical command failure. */
		#define MASS_STORE_SCSI_COMMAND_FAILED      0xC0

	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_MASSSTORE_COMMANDS_C)
			static uint8_t MassStore_SendCommand(MS_CommandBlockWrapper_t* const SCSICommandBlock,
			                                     void* BufferPtr);
			static uint8_t MassStore_WaitForDataReceived(void);
			static uint8_t MassStore_SendReceiveData(MS_CommandBlockWrapper_t* const SCSICommandBlock,
			                                         void* BufferPtr) ATTR_NON_NULL_PTR_ARG(1);
			static uint8_t MassStore_GetReturnedStatus(MS_CommandStatusWrapper_t* const SCSICommandStatus) ATTR_NON_NULL_PTR_ARG(1);
		#endif

		uint8_t MassStore_MassStorageReset(void);
		uint8_t MassStore_GetMaxLUN(uint8_t* const MaxLUNIndex);
		uint8_t MassStore_RequestSense(const uint8_t LUNIndex,
		                               SCSI_Request_Sense_Response_t* const SensePtr) ATTR_NON_NULL_PTR_ARG(2);
		uint8_t MassStore_Inquiry(const uint8_t LUNIndex,
		                          SCSI_Inquiry_Response_t* const InquiryPtr) ATTR_NON_NULL_PTR_ARG(2);
		uint8_t MassStore_ReadDeviceBlock(const uint8_t LUNIndex,
		                                  const uint32_t BlockAddress,
		                                  const uint8_t Blocks,
		                                  const uint16_t BlockSize,
		                                  void* BufferPtr) ATTR_NON_NULL_PTR_ARG(5);
		uint8_t MassStore_WriteDeviceBlock(const uint8_t LUNIndex,
		                                   const uint32_t BlockAddress,
                                           const uint8_t Blocks,
		                                   const uint16_t BlockSize,
		                                   void* BufferPtr) ATTR_NON_NULL_PTR_ARG(5);
		uint8_t MassStore_ReadCapacity(const uint8_t LUNIndex,
		                               SCSI_Capacity_t* const CapacityPtr) ATTR_NON_NULL_PTR_ARG(2);
		uint8_t MassStore_TestUnitReady(const uint8_t LUNIndex);
		uint8_t MassStore_PreventAllowMediumRemoval(const uint8_t LUNIndex,
		                                            const bool PreventRemoval);

#endif

