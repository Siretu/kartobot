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
 *  \brief Pipe data stream transmission and reception management.
 *  \copydetails Group_PipeStreamRW
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in LUFA/Drivers/USB/USB.h.
 */

/** \ingroup Group_PipeRW  
 *  \defgroup Group_PipeStreamRW Read/Write of Multi-Byte Streams
 *  \brief Pipe data stream transmission and reception management.
 *
 *  Functions, macros, variables, enums and types related to data reading and writing of data streams from
 *  and to pipes.
 *
 *  @{
 */

#ifndef __PIPE_STREAM_H__
#define __PIPE_STREAM_H__

	/* Includes: */
		#include "../../../Common/Common.h"
		#include "USBMode.h"
		
	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include LUFA/Drivers/USB/USB.h instead.
		#endif
		
	/* Public Interface - May be used in end-application: */
		/* Enums: */
			/** Enum for the possible error return codes of the Pipe_*_Stream_* functions. */
			enum Pipe_Stream_RW_ErrorCodes_t
			{
				PIPE_RWSTREAM_NoError            = 0, /**< Command completed successfully, no error. */
				PIPE_RWSTREAM_PipeStalled        = 1, /**< The device stalled the pipe during the transfer. */		
				PIPE_RWSTREAM_DeviceDisconnected = 2, /**< Device was disconnected from the host during
			                                           *   the transfer.
			                                           */		
				PIPE_RWSTREAM_Timeout            = 3, /**< The device failed to accept or send the next packet
				                                       *   within the software timeout period set by the
				                                       *   \ref USB_STREAM_TIMEOUT_MS macro.
				                                       */
				PIPE_RWSTREAM_IncompleteTransfer = 4, /**< Indicates that the pipe bank became full/empty before the
				                                       *   complete contents of the stream could be transferred.
				                                       */
			};

	/* Architecture Includes: */
		#if (ARCH == ARCH_AVR8)
			#include "AVR8/PipeStream_AVR8.h"
		#elif (ARCH == ARCH_UC3)
			#include "UC3/PipeStream_UC3.h"
		#elif (ARCH == ARCH_LPC)
			#include "LPC/PipeStream_LPC.h"
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
	
#endif

/** @} */

