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
 *  Printer Device commands, to send/receive data to and from an attached USB
 *  printer, and to send and receive Printer Class control requests.
 */

#include "PrinterCommands.h"

/** Sends the given data directly to the printer via the data endpoints, for the sending of print commands in printer
 *  languages accepted by the attached printer (e.g. PCL).
 *
 *  \param[in] PrinterCommands  Pointer to the data to send to the attached printer
 *  \param[in] CommandSize  Size of the data to send to the attached printer
 *
 *  \return A value from the Pipe_Stream_RW_ErrorCodes_t enum
 */
uint8_t Printer_SendData(const void* const PrinterCommands,
                         const uint16_t CommandSize)
{
	uint8_t ErrorCode;

	Pipe_SelectPipe(PRINTER_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	if ((ErrorCode = Pipe_Write_Stream_LE(PrinterCommands, CommandSize, NULL)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	Pipe_ClearOUT();

	Pipe_WaitUntilReady();

	Pipe_Freeze();

	return PIPE_RWSTREAM_NoError;
}

/** Issues a Printer class Get Device ID command to the attached device, to retrieve the device ID string (which indicates
 *  the accepted printer languages, the printer's model and other pertinent information).
 *
 *  \param[out] DeviceIDString Pointer to the destination where the returned string should be stored
 *  \param[in] BufferSize  Size in bytes of the allocated buffer for the returned Device ID string
 *
 *  \return A value from the USB_Host_SendControlErrorCodes_t enum
 */
uint8_t Printer_GetDeviceID(char* DeviceIDString,
                            const uint16_t BufferSize)
{
	uint8_t  ErrorCode = HOST_SENDCONTROL_Successful;
	uint16_t DeviceIDStringLength = 0;

	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = PRNT_REQ_GetDeviceID,
			.wValue        = 0,
			.wIndex        = PrinterInterfaceNumber,
			.wLength       = sizeof(DeviceIDStringLength),
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	if ((ErrorCode = USB_Host_SendControlRequest(&DeviceIDStringLength)) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	if (!(DeviceIDStringLength))
	{
		DeviceIDString[0] = 0x00;
		return HOST_SENDCONTROL_Successful;
	}

	DeviceIDStringLength = SwapEndian_16(DeviceIDStringLength);

	if (DeviceIDStringLength > BufferSize)
	  DeviceIDStringLength = BufferSize;

	USB_ControlRequest.wLength = DeviceIDStringLength;

	if ((ErrorCode = USB_Host_SendControlRequest(DeviceIDString)) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	/* Move string back two characters to remove the string length value from the start of the array */
	memmove(&DeviceIDString[0], &DeviceIDString[2], DeviceIDStringLength - 2);

	DeviceIDString[DeviceIDStringLength - 2] = 0x00;

	return HOST_SENDCONTROL_Successful;
}

/** Issues a Printer class Get Port Status command to the attached device, to retrieve the current status flags of the
 *  printer.
 *
 *  \param[out] PortStatus  Pointer to the destination where the printer's status flag values should be stored
 *
 *  \return A value from the USB_Host_SendControlErrorCodes_t enum
 */
uint8_t Printer_GetPortStatus(uint8_t* const PortStatus)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = PRNT_REQ_GetPortStatus,
			.wValue        = 0,
			.wIndex        = PrinterInterfaceNumber,
			.wLength       = sizeof(uint8_t),
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(PortStatus);
}

/** Issues a Printer class Soft Reset command to the attached device, to reset the printer ready for new input without
 *  physically cycling the printer's power.
 *
 *  \return A value from the USB_Host_SendControlErrorCodes_t enum
 */
uint8_t Printer_SoftReset(void)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = PRNT_REQ_SoftReset,
			.wValue        = 0,
			.wIndex        = PrinterInterfaceNumber,
			.wLength       = 0,
		};

	Pipe_SelectPipe(PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(NULL);
}

