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

#ifndef _DATARAM_H_
#define _DATARAM_H_
#if ARCH==ARCH_LPC
		#include "../MassStorage.h"
		#include "../Descriptors.h"
		#include "USB.h"
#else
	/* Includes: */
		#include <avr/io.h>
		#include <LUFA/Drivers/Board/Dataflash.h>
	/* Preprocessor Checks: */
		#if (DATAFLASH_PAGE_SIZE % 16)
			#error Dataflash page size must be a multiple of 16 bytes.
		#endif
		#include "MassStorage.h"
		#include "Descriptors.h"

		#include <LUFA/Common/Common.h>
		#include <LUFA/Drivers/USB/USB.h>
#endif

	/* Defines: */
#if ARCH==ARCH_LPC
		/** Start address and size of RAM area which used for disk image */
		#if defined(__LPC18XX__)
			#define DATA_RAM_START_ADDRESS			0x20008000
			#define DATA_RAM_SIZE					0x8000
		#endif
		#if defined(__LPC17XX__)
			#define DATA_RAM_START_ADDRESS			0x20080000
			#define DATA_RAM_SIZE					0x4000
		#endif
		#if defined(__LPC11UXX__)
			#define DATA_RAM_START_ADDRESS			0x20080000
			#define DATA_RAM_SIZE					0x2000 /* fake capacity to treat windows */
			#define DATA_11U_ACTUAL_RAM_SIZE		0xC00
		#endif
		/** Total number of bytes of the storage medium, comprised of one or more Dataflash ICs. */
		#define VIRTUAL_MEMORY_BYTES                DATA_RAM_SIZE
#else
		/** Total number of bytes of the storage medium, comprised of one or more Dataflash ICs. */
		#define VIRTUAL_MEMORY_BYTES                ((uint32_t)DATAFLASH_PAGES * DATAFLASH_PAGE_SIZE * DATAFLASH_TOTALCHIPS)
#endif
		/** Block size of the device. This is kept at 512 to remain compatible with the OS despite the underlying
		 *  storage media (Dataflash) using a different native block size. Do not change this value.
		 */
		#define VIRTUAL_MEMORY_BLOCK_SIZE           512

		/** Total number of blocks of the virtual memory for reporting to the host as the device's total capacity. Do not
		 *  change this value; change VIRTUAL_MEMORY_BYTES instead to alter the media size.
		 */
		#define VIRTUAL_MEMORY_BLOCKS               (VIRTUAL_MEMORY_BYTES / VIRTUAL_MEMORY_BLOCK_SIZE)

	/* Function Prototypes: */
		void DataRam_WriteBlocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
		                                  const uint32_t BlockAddress,
		                                  uint16_t TotalBlocks);
		void DataRam_ReadBlocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
		                                 const uint32_t BlockAddress,
		                                 uint16_t TotalBlocks);
#endif

