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

#include "HCD.h"

#ifdef USB_CAN_BE_HOST

#if defined(__LPC_OHCI__)
	#define HCD_FUNCTION(x) O##x
#elif defined(__LPC_EHCI__)
	#define HCD_FUNCTION(x) E##x
#else
	#error "Either __LPC_OHCI__ or __LPC_EHCI__ must be defined"
#endif

HCD_STATUS HCD_FUNCTION(HcdInitDriver)(uint8_t HostID);
HCD_STATUS HCD_FUNCTION(HcdDeInitDriver)(uint8_t HostID);

HCD_STATUS HCD_FUNCTION(HcdRhPortEnable)(uint8_t HostID, uint8_t PortNum); 
HCD_STATUS HCD_FUNCTION(HcdRhPortDisable)(uint8_t HostID, uint8_t PortNum);
HCD_STATUS HCD_FUNCTION(HcdRhPortReset)(uint8_t HostID, uint8_t uPortNumber);
HCD_USB_SPEED  HCD_FUNCTION(HcdGetDeviceSpeed)(uint8_t HostID, uint8_t PortNum, HCD_USB_SPEED* DeviceSpeed);

HCD_STATUS HCD_FUNCTION(HcdOpenPipe)(uint8_t HostID,
						uint8_t DeviceAddr,
						HCD_USB_SPEED DeviceSpeed,
						uint8_t EndpointNo,
						HCD_TRANSFER_TYPE TransferType,
						HCD_TRANSFER_DIR TransferDir,
						uint16_t MaxPacketSize,
						uint8_t Interval,
#if defined(__LPC_EHCI__)
						uint8_t Mult,
						uint8_t HSHubDevAddr,
						uint8_t HSHubPortNum,
#endif
						uint32_t* const PipeHandle);

HCD_STATUS HCD_FUNCTION(HcdClosePipe)(uint32_t PipeHandle);
HCD_STATUS HCD_FUNCTION(HcdCancelTransfer)(uint32_t PipeHandle);
HCD_STATUS HCD_FUNCTION(HcdClearEndpointHalt)(uint32_t PipeHandle);
uint8_t HCD_FUNCTION(HcdGetBoundEndpointAddress) (uint32_t PipeHandle);

HCD_STATUS HCD_FUNCTION(HcdControlTransfer)(uint32_t PipeHandle, const uint8_t* const pDeviceRequest, uint8_t* const buffer);
HCD_STATUS HCD_FUNCTION(HcdDataTransfer)( uint32_t PipeHandle, uint8_t* const buffer, uint32_t const length, uint32_t* const pActualTransferred );
uint32_t   HCD_FUNCTION(HcdGetFrameNumber)(uint8_t HostID);
void	   HCD_FUNCTION(HcdIrqHandler) (uint8_t HostID);

/************************************************************************/
/* Driver Initialization                                                                     */
/************************************************************************/
HCD_STATUS HcdInitDriver (uint8_t HostID)
{
	return HCD_FUNCTION(HcdInitDriver) (HostID);
}
HCD_STATUS HcdDeInitDriver(uint8_t HostID)
{
	return HCD_FUNCTION(HcdDeInitDriver) (HostID);
}

void HcdIrqHandler(uint8_t HostID)
{
	HCD_FUNCTION(HcdIrqHandler) (HostID);
}

uint32_t HcdGetFrameNumber(uint8_t HostID)
{
	return HCD_FUNCTION(HcdGetFrameNumber)(HostID);
}

/************************************************************************/
/* Port API                                                                     */
/************************************************************************/
HCD_STATUS HcdRhPortReset(uint8_t HostID, uint8_t PortNum) 
{
	return HCD_FUNCTION(HcdRhPortReset) (HostID, PortNum);
}
HCD_STATUS HcdRhPortEnable(uint8_t HostID, uint8_t PortNum) 
{
	return HCD_FUNCTION(HcdRhPortEnable) (HostID, PortNum);
}
HCD_STATUS HcdRhPortDisable(uint8_t HostID, uint8_t PortNum)
{
	return HCD_FUNCTION(HcdRhPortDisable) (HostID, PortNum);
}
HCD_STATUS HcdGetDeviceSpeed(uint8_t HostID, uint8_t PortNum, HCD_USB_SPEED* DeviceSpeed)
{
	return HCD_FUNCTION(HcdGetDeviceSpeed)(HostID, PortNum,DeviceSpeed);
}

/************************************************************************/
/* Pipe API                                                                     */
/************************************************************************/
HCD_STATUS HcdOpenPipe(uint8_t HostID,
					   uint8_t DeviceAddr,
					   HCD_USB_SPEED DeviceSpeed,
					   uint8_t EndpointNo,
					   HCD_TRANSFER_TYPE TransferType,
					   HCD_TRANSFER_DIR TransferDir,
					   uint16_t MaxPacketSize,
					   uint8_t Interval,
					   uint8_t Mult,
					   uint8_t HSHubDevAddr,
					   uint8_t HSHubPortNum,
					   uint32_t* const PipeHandle)
{
	return HCD_FUNCTION(HcdOpenPipe) (	HostID, 
										DeviceAddr, 
										DeviceSpeed, 
										EndpointNo, 
										TransferType, 
										TransferDir, 
										MaxPacketSize, 
										Interval,
							#if defined(__LPC_EHCI__)
										Mult, HSHubDevAddr, HSHubPortNum, 
							#endif			
										PipeHandle);
}

HCD_STATUS HcdClosePipe(uint32_t PipeHandle)
{
	return HCD_FUNCTION(HcdClosePipe) (PipeHandle);
}
HCD_STATUS HcdCancelTransfer(uint32_t PipeHandle)
{
	return HCD_FUNCTION(HcdCancelTransfer)(PipeHandle);
}
HCD_STATUS 	HcdClearEndpointHalt(uint32_t PipeHandle)
{
	return HCD_FUNCTION(HcdClearEndpointHalt) (PipeHandle);
}

uint8_t HcdGetBoundEndpointAddress (uint32_t PipeHandle)
{
	return HCD_FUNCTION(HcdGetBoundEndpointAddress) (PipeHandle);
}
/************************************************************************/
/* Transfer API                                                                     */
/************************************************************************/
HCD_STATUS HcdControlTransfer(uint32_t PipeHandle, const uint8_t* const pDeviceRequest, uint8_t* const buffer)
{
	return HCD_FUNCTION(HcdControlTransfer) (PipeHandle, pDeviceRequest, buffer);
}

HCD_STATUS HcdDataTransfer( uint32_t PipeHandle, uint8_t* const buffer, uint32_t const length, uint32_t* const pActualTransferred )
{
	return HCD_FUNCTION(HcdDataTransfer) (PipeHandle, buffer, length, pActualTransferred);
}

/*==========================================================================*/
/* Private Functions to OHCI EHCI                        											*/
/*==========================================================================*/

#ifdef NXPUSBLIB_DEBUG
void assert_status_ok_message(HCD_STATUS status, char const * mess, char const * func, char const * file, uint32_t const line )
{
	if (HCD_STATUS_OK != status)
	{
		hcd_printf("%s\r\n", func);
		hcd_printf("\t%s: %d\r\n", file, line);
		hcd_printf("\tEvaluated HCD_STATUS = %d\r\n", (uint32_t)status );
		if (mess != NULL){
			hcd_printf("\t%s\r\n", mess);
		}
	}
}
#endif

void  HcdDelayUS (uint32_t  delay)
{
	volatile  uint32_t  i;

	for (i = 0; i < (4 * delay); i++) {    /* This logic was tested. It gives app. 1 micro sec delay        */
		;
	}
}
void  HcdDelayMS (uint32_t  delay)
{
	volatile  uint32_t  i;

	for (i = 0; i < delay; i++) {
		HcdDelayUS(1000);
	}
}

HCD_STATUS OpenPipe_VerifyParameters( uint8_t HostID, uint8_t DeviceAddr, HCD_USB_SPEED DeviceSpeed, uint8_t EndpointNumber, HCD_TRANSFER_TYPE TransferType, HCD_TRANSFER_DIR TransferDir, uint16_t MaxPacketSize, uint8_t Interval, uint8_t Mult )
{
	if	(HostID >= MAX_HOST_CONTROLLER ||
		DeviceAddr > 127 ||
		DeviceSpeed > HIGH_SPEED ||
		(EndpointNumber & 0x70) ||
		TransferType > INTERRUPT_TRANSFER ||
		TransferDir > OUT_TRANSFER )
	{
		ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
	}

	/* Lowspeed with control size > 8 cannot be mounted, should we relax the restriction */
	if (DeviceSpeed == LOW_SPEED && (MaxPacketSize > 8 || TransferType == BULK_TRANSFER || TransferType == ISOCHRONOUS_TRANSFER ) )
	{
		ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
	}

	switch(TransferType)
	{
	case CONTROL_TRANSFER:
		if (MaxPacketSize > 64)
		{
			ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
		}
		break;

	case BULK_TRANSFER:
		if ((DeviceSpeed == FULL_SPEED && MaxPacketSize > 64) ||
			(DeviceSpeed == HIGH_SPEED && MaxPacketSize > 512) )
		{
			ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
		}
		break;

	case INTERRUPT_TRANSFER:
		if ( Interval==0 ||
			(DeviceSpeed == FULL_SPEED && MaxPacketSize > 64) ||
			(DeviceSpeed == HIGH_SPEED && (MaxPacketSize > 1024 || Interval > 16 || Mult == 0)) )
		{
			ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
		}
		break;

	case ISOCHRONOUS_TRANSFER:
		if ( Interval == 0 || Interval > 16 ||
			(DeviceSpeed == FULL_SPEED && MaxPacketSize > 1023) ||
			(DeviceSpeed == HIGH_SPEED && (MaxPacketSize > 1024 || Mult == 0)) )
		{
			ASSERT_STATUS_OK(HCD_STATUS_PARAMETER_INVALID);
		}
		break;
	}
	return HCD_STATUS_OK;
}

#endif
