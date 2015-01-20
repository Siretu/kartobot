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
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Macros: */
		/** Endpoint number of the Audio isochronous streaming data endpoint. */
		#define AUDIO_STREAM_EPNUM           1

		/** Endpoint size in bytes of the Audio isochronous streaming data endpoint. The Windows audio stack requires
		 *  at least 192 bytes for correct output, thus the smaller 128 byte maximum endpoint size on some of the smaller
		 *  USB AVR models will result in unavoidable distorted output.
		 */
		#define AUDIO_STREAM_EPSIZE          ENDPOINT_MAX_SIZE(AUDIO_STREAM_EPNUM)

		/** Sample frequency of the data being transmitted through the streaming endpoint. */
		#define AUDIO_SAMPLE_FREQUENCY       48000

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t     Config;
			USB_Descriptor_Interface_t                Audio_ControlInterface;
			USB_Audio_Descriptor_Interface_AC_t       Audio_ControlInterface_SPC;
			USB_Audio_Descriptor_InputTerminal_t      Audio_InputTerminal;
			USB_Audio_Descriptor_OutputTerminal_t     Audio_OutputTerminal;
			USB_Descriptor_Interface_t                Audio_StreamInterface_Alt0;
			USB_Descriptor_Interface_t                Audio_StreamInterface_Alt1;
			USB_Audio_Descriptor_Interface_AS_t       Audio_StreamInterface_SPC;
			USB_Audio_Descriptor_Format_t             Audio_AudioFormat;
			USB_Audio_Descriptor_StreamEndpoint_Std_t Audio_StreamEndpoint;
			USB_Audio_Descriptor_StreamEndpoint_Spc_t Audio_StreamEndpoint_SPC;
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
