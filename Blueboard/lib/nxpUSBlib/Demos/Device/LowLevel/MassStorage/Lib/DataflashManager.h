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
 *  Header file for DataflashManager.c.
 */

#ifndef _DATAFLASH_MANAGER_H_
#define _DATAFLASH_MANAGER_H_

	/* Includes: */
		#include <avr/io.h>

		#include "MassStorage.h"
		#include "Descriptors.h"

		#include <LUFA/Common/Common.h>
		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Drivers/Board/Dataflash.h>

	/* Preprocessor Checks: */
		#if (DATAFLASH_PAGE_SIZE % 16)
			#error Dataflash page size must be a multiple of 16 bytes.
		#endif

	/* Defines: */
		/** Total number of bytes of the storage medium, comprised of one or more Dataflash ICs. */
		#define VIRTUAL_MEMORY_BYTES                ((uint32_t)DATAFLASH_PAGES * DATAFLASH_PAGE_SIZE * DATAFLASH_TOTALCHIPS)

		/** Block size of the device. This is kept at 512 to remain compatible with the OS despite the underlying
		 *  storage media (Dataflash) using a different native block size. Do not change this value.
		 */
		#define VIRTUAL_MEMORY_BLOCK_SIZE           512

		/** Total number of blocks of the virtual memory for reporting to the host as the device's total capacity. Do not
		 *  change this value; change VIRTUAL_MEMORY_BYTES instead to alter the media size.
		 */
		#define VIRTUAL_MEMORY_BLOCKS               (VIRTUAL_MEMORY_BYTES / VIRTUAL_MEMORY_BLOCK_SIZE)

		/** Total number of Logical Units (drives) in the device. The total device capacity is shared equally between
		 *  each drive - this can be set to any positive non-zero amount.
		 */
		#define TOTAL_LUNS                           1

		/** Blocks in each LUN, calculated from the total capacity divided by the total number of Logical Units in the device. */
		#define LUN_MEDIA_BLOCKS                    (VIRTUAL_MEMORY_BLOCKS / TOTAL_LUNS)

	/* Function Prototypes: */
		void DataflashManager_WriteBlocks(const uint32_t BlockAddress,
		                                  uint16_t TotalBlocks);
		void DataflashManager_ReadBlocks(const uint32_t BlockAddress,
		                                 uint16_t TotalBlocks);
		void DataflashManager_WriteBlocks_RAM(const uint32_t BlockAddress,
		                                      uint16_t TotalBlocks,
		                                      uint8_t* BufferPtr) ATTR_NON_NULL_PTR_ARG(3);
		void DataflashManager_ReadBlocks_RAM(const uint32_t BlockAddress,
		                                     uint16_t TotalBlocks,
		                                     uint8_t* BufferPtr) ATTR_NON_NULL_PTR_ARG(3);
		void DataflashManager_ResetDataflashProtections(void);
		bool DataflashManager_CheckDataflashOperation(void);

#endif

