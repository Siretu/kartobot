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

/*=======================================================================*/
/*        I N C L U D E S                                                */
/*=======================================================================*/
#define  __INCLUDE_FROM_USB_DRIVER
#include "../../../USBMode.h"

#if (defined(USB_CAN_BE_HOST) && defined(__LPC_OHCI__))

#define __LPC_OHCI_C__
#include "OHCI.h"

OHCI_HOST_DATA_Type ohci_data[MAX_HOST_CONTROLLER] __DATA(USBRAM_SECTION);

/*=======================================================================*/
/*  G L O B A L   S Y M B O L   D E C L A R A T I O N S                  */
/*=======================================================================*/
void USB_Host_Enumerate (uint8_t HostID);
void USB_Host_DeEnumerate(uint8_t HostID);

/*********************************************************************
*								IMPLEMENTATION						
**********************************************************************/
HCD_STATUS OHcdGetDeviceSpeed(uint8_t HostID, uint8_t PortNum, HCD_USB_SPEED* DeviceSpeed)
{
	if ( OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_CurrentConnectStatus) /* If device is connected */
	{
		*DeviceSpeed =  ( OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_LowSpeedDeviceAttached) ? LOW_SPEED : FULL_SPEED;
		return HCD_STATUS_OK;
	}else
	{
		return HCD_STATUS_DEVICE_DISCONNECTED;
	}
}

uint32_t OHcdGetFrameNumber(uint8_t HostID)
{
	return ohci_data[HostID].hcca.HccaFrameNumber;
}

HCD_STATUS OHcdRhPortReset(uint8_t HostID, uint8_t uPortNumber)
{
	HcdDelayMS(400);
	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortResetStatus; /* SetPortReset */
	/* should have time-out */
	while ( OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_PortResetStatus) {}

	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortResetStatusChange; /* Clear Port Reset Status Change */
	
	HcdDelayMS(400);
	return HCD_STATUS_OK;
}

HCD_STATUS OHcdRhPortEnable(uint8_t HostID, uint8_t uPortNumber)
{
	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PowerEnableStatus; /* SetPortEnable */

	return HCD_STATUS_OK;
}

HCD_STATUS OHcdRhPortDisable(uint8_t HostID, uint8_t uPortNumber)
{
	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_CurrentConnectStatus; /* ClearPortEnable */

	return HCD_STATUS_OK;
}

HCD_STATUS OHcdInitDriver(uint8_t HostID)
{
	OHCI_REG(HostID)->OTGClkCtrl = 0x00000019;			/* enable Host clock, OTG clock and AHB clock */
	while((OHCI_REG(HostID)->OTGClkSt & 0x00000019)!= 0x00000019);
	OHCI_REG(HostID)->OTGStCtrl = 0x3;					/* ??? */

	OHciHostReset(HostID);	/* Software Reset */
	return OHciHostInit(HostID);
}

HCD_STATUS OHcdDeInitDriver(uint8_t HostID)
{
	return HCD_STATUS_OK;
}

/*********************************************************************//**
 * @brief		Open communication pipe for an endpoint
 * @param[in]	HostID			select Host Controller, start from 0
 * @param[in]	DeviceAddr		Address of destinate device 
 * @param[in]	EnpointNumber	Endpoint Number to communicate
 * @param[in]	TransferType	One of four transfer CONTROL_TRANSFER, ISOCHRONOUS_TRANSFER, BULK_TRANSFER, INTERRUPT_TRANSFER
 * @param[in]	TransferDir		Pipe Direction either IN_TRANSFER or OUT_TRANSFER. For control this parameter is ignored
 * @param[in]	MaxPacketSize	Endpoint max packet size for each usb transaction
 * @param[in]	Interval		Polling frame (or micro frame), valid only for Interrupt and Isochronous
 *				- Interrupt		: range 1-255
 *				- Isochronous	: value is used as the exponent for a 2^(Interval-1). E.g: 4 means a period of 8=2^(4-1)
 * @param[out]	PipeHandle		: Handler for further communication on this pipe
 *				
 * @return 		HCD_STATUS
 *				- HCD_STATUS_OK : Pipe is opened successfully
 *				- Others		: Error occurs, PipeHandle is not valid
 * Note: This function must be invoked before any communication between device endpoint and host. 
 **********************************************************************/
HCD_STATUS OHcdOpenPipe(uint8_t HostID,
					   uint8_t DeviceAddr,
					   HCD_USB_SPEED DeviceSpeed,
					   uint8_t EndpointNumber,
					   HCD_TRANSFER_TYPE TransferType,
					   HCD_TRANSFER_DIR TransferDir,
					   uint16_t MaxPacketSize,
					   uint8_t Interval,
					   uint32_t* const PipeHandle)
{
	uint32_t EdIdx;
	uint8_t ListIdx;

#if !ISO_LIST_ENABLE
	if ( TransferType == ISOCHRONOUS_TRANSFER )
	{
		ASSERT_STATUS_OK_MESSAGE(HCD_STATUS_TRANSFER_TYPE_NOT_SUPPORTED, "Please set ISO_LIST_ENABLE to YES");
	}
#endif

#if !INTERRUPT_LIST_ENABLE
	if ( TransferType == INTERRUPT_TRANSFER )
	{
		ASSERT_STATUS_OK_MESSAGE(HCD_STATUS_TRANSFER_TYPE_NOT_SUPPORTED, "Please set INTERRUPT_LIST_ENABLE to YES");
	}
#endif

	/********************************* Parameters Verify *********************************/
	ASSERT_STATUS_OK( OpenPipe_VerifyParameters(HostID, DeviceAddr, DeviceSpeed, EndpointNumber, TransferType, TransferDir, MaxPacketSize, Interval, 0) );

	EndpointNumber &= 0xF;	/* Endpoint number is in range 0-15 */
	MaxPacketSize &= 0x3FF;	/* Max Packet Size is in range 0-1024 */


	/* Add new ED to the EDs List */
	switch (TransferType)
	{
		case CONTROL_TRANSFER:
			ListIdx = CONTROL_LIST_HEAD;
		break;

		case BULK_TRANSFER:
			ListIdx = BULK_LIST_HEAD;
		break;

		case INTERRUPT_TRANSFER:
			//ListIdx = FindInterruptTransferListIndex(Interval);
			ListIdx = INTERRUPT_1ms_LIST_HEAD;
		break;

		case ISOCHRONOUS_TRANSFER:
			ListIdx = ISO_LIST_HEAD;
		break;
	}

	ASSERT_STATUS_OK ( AllocEd(DeviceAddr, DeviceSpeed, EndpointNumber, TransferType, TransferDir, MaxPacketSize, Interval, &EdIdx) ) ;
	HcdED(EdIdx)->ListIndex  = ListIdx;
	InsertEndpoint(HostID, EdIdx, ListIdx);

	PipehandleCreate(PipeHandle, HostID, EdIdx);
	return HCD_STATUS_OK;
}

/*********************************************************************//**
 * @brief		Cancel all transfer queued/processing on the selected pipe
 * @param[in]	PipeHandle	Handler of target pipe
 * @return 		HCD_STATUS
 *				- HCD_STATUS_OK : function performs successfully
 *				- Others		: Error occurs
 * Note: 
 **********************************************************************/
HCD_STATUS OHcdCancelTransfer(uint32_t PipeHandle)
{
	uint8_t HostID, EdIdx;

	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );

	HcdED(EdIdx)->hcED.Skip = 1;
	HcdDelayMS(2); /* delay 1 ms until the next frame to make sure HC not processing this ED */

	/* ISO TD & General TD have the same offset for nextTD, we can use GTD as pointer to travel on TD list */
	while ( Align16( HcdED(EdIdx)->hcED.HeadP.HeadTD ) != Align16( HcdED(EdIdx)->hcED.TailP ) )
	{
		uint32_t HeadTD = Align16( HcdED(EdIdx)->hcED.HeadP.HeadTD );
		if ( IsIsoEndpoint(EdIdx) ) 
		{
			HcdED(EdIdx)->hcED.HeadP.HeadTD = ((PHCD_IsoTransferDescriptor) HeadTD)->NextTD;
			FreeItd( (PHCD_IsoTransferDescriptor) HeadTD);
		}	
		else
		{
			HcdED(EdIdx)->hcED.HeadP.HeadTD = ((PHCD_GeneralTransferDescriptor) HeadTD)->hcGTD.NextTD;
			FreeGtd((PHCD_GeneralTransferDescriptor) HeadTD);
		}
	}
	HcdED(EdIdx)->hcED.HeadP.HeadTD = Align16( HcdED(EdIdx)->hcED.TailP ); /*-- Toggle Carry is also set to 0 --*/
	HcdED(EdIdx)->hcED.HeadP.ToggleCarry = 0;

	HcdED(EdIdx)->hcED.Skip = 0;
	return HCD_STATUS_OK;
}

/*********************************************************************//**
 * @brief		No more communication on this pipe, all queued/processing transfers are cancelled (if any)
 * @param[in]	PipeHandle	Handler of target pipe
 * @return 		HCD_STATUS
 *				- HCD_STATUS_OK	: function performs successfully
 *				- Others		: Error occurs
 * Note: 
 **********************************************************************/
HCD_STATUS OHcdClosePipe(uint32_t PipeHandle)
{
	uint8_t HostID, EdIdx;

	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );

	ASSERT_STATUS_OK ( OHcdCancelTransfer(PipeHandle) );

	HcdED(EdIdx)->hcED.Skip = 1; /* no need for delay, it is already delayed in cancel transfer */
	RemoveEndpoint(HostID, EdIdx);

	FreeED(EdIdx);

	return HCD_STATUS_OK;
}
HCD_STATUS OHcdClearEndpointHalt(uint32_t PipeHandle)
{
	uint8_t HostID, EdIdx;
	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );
	
	HcdED(EdIdx)->hcED.HeadP.Halted = 0;
	HcdED(EdIdx)->hcED.HeadP.ToggleCarry = 0;

	return HCD_STATUS_OK;
}

uint8_t OHcdGetBoundEndpointAddress(uint32_t PipeHandle)
{
	uint8_t HostID, EdIdx;
	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );

	return HcdED(EdIdx)->hcED.FunctionAddr | (HcdED(EdIdx)->hcED.Direction == 2 ? 0x80 : 0x00);
}

/*********************************************************************//**
 * @brief		Issue Transfer on the control pipe
 * @param[in]	PipeHandle		Handler of target pipe
 * @param[in]	pDeviceRequest	8-byte device request
 * @param[in]	buffer
 * @param[out]	
 *				
 * @return 		HCD_STATUS
 *				- HCD_STATUS_OK	: function performs successfully
 *				- Others		: Error occurs
 * Note: 
 **********************************************************************/
HCD_STATUS OHcdControlTransfer(uint32_t PipeHandle, const uint8_t* const pDeviceRequest, uint8_t* const buffer)
{
	uint8_t HostID, EdIdx;

	struct HCD_DeviceRequest{
		uint8_t	 bmRequestType;
		uint8_t	 bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;
	} *stDeviceRequest;

	if (pDeviceRequest == NULL || buffer == NULL)
	{
		ASSERT_STATUS_OK_MESSAGE(HCD_STATUS_PARAMETER_INVALID, "Device Request or Data Buffer is NULL");
	}

	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );

	stDeviceRequest = (struct HCD_DeviceRequest *) pDeviceRequest;
	/************************************************************************/
	/* Setup Stage                                                          */
	/************************************************************************/
	ASSERT_STATUS_OK ( QueueOneGTD(EdIdx, (uint8_t* const) pDeviceRequest, 8, 0, 2) );		/* Setup TD: DirectionPID=00 - DataToggle=10b (always DATA0) */
	
	/************************************************************************/
	/* Data Stage                                                           */
	/************************************************************************/
	ASSERT_STATUS_OK ( QueueGTDs(EdIdx, buffer, stDeviceRequest->wLength, (stDeviceRequest->bmRequestType & 0x80) ? 2 : 1) );

	/************************************************************************/
	/* Status Stage                                                                     */
	/************************************************************************/
	ASSERT_STATUS_OK ( QueueOneGTD(EdIdx, NULL, 0, (stDeviceRequest->bmRequestType & 0x80) ? 1 : 2, 3) );	/* Status TD: Direction=opposite of data direction - DataToggle=11b (always DATA1) */

	/* set control list filled */
	OHCI_REG(HostID)->HcCommandStatus |= HC_COMMAND_STATUS_ControlListFilled;

	/* wait for semaphore compete TDs */
	ASSERT_STATUS_OK ( WaitForTransferComplete(EdIdx) );

	return HCD_STATUS_OK;
}

static HCD_STATUS QueueOneITD(uint32_t EdIdx, uint8_t* dataBuff, uint32_t TDLen, uint16_t StartingFrame)
{
	uint32_t i;
	Bool isCrossPage;
	PHCD_IsoTransferDescriptor pItd = (PHCD_IsoTransferDescriptor) Align16( HcdED(EdIdx)->hcED.TailP );
	
	pItd->StartingFrame = StartingFrame;
	pItd->FrameCount = (TDLen / HcdED(EdIdx)->hcED.MaxPackageSize) + (TDLen%HcdED(EdIdx)->hcED.MaxPackageSize ? 1 : 0) - 1;
	pItd->BufferPage0 = Align4k( (uint32_t) dataBuff );
	pItd->BufferEnd = (uint32_t) (dataBuff + TDLen - 1);
	isCrossPage = ( Align4k(pItd->BufferEnd) !=  Align4k(pItd->BufferPage0) );

	for (i=0; TDLen>0 && i < 8; i++)
	{
		uint32_t XactLen = MIN(TDLen, HcdED(EdIdx)->hcED.MaxPackageSize);

		pItd->OffsetPSW[i] = 0xE000 | ( ((uint32_t) dataBuff) & 0x1FFF ); /*-- take into cross page account later 15-12: ConditionCode, 11-0: offset --*/
		
		TDLen -= XactLen;
		dataBuff += XactLen;
	}

	/* Create a new place holder TD & link setup TD to the new place holder */
	ASSERT_STATUS_OK ( AllocItdForEd(EdIdx) );
	HcdED(EdIdx)->semaphore++;

	return HCD_STATUS_OK;
}

static HCD_STATUS QueueITDs(uint32_t EdIdx, uint8_t* dataBuff, uint32_t xferLen)
{
	uint32_t FrameIdx = OHcdGetFrameNumber(0)+1;
	uint32_t MaxTDLen	= ITD_MAX_XFER_LENGTH - Offset4k((uint32_t)dataBuff);
	uint32_t MaxDataSize;

	MaxDataSize = HcdED(EdIdx)->MaxXactPerITD * HcdED(EdIdx)->hcED.MaxPackageSize;
	MaxTDLen = MIN(MaxDataSize, MaxTDLen);

	while (xferLen > 0)
	{
		uint16_t TdLen;

		TdLen = MIN(xferLen, MaxTDLen);
		xferLen -= TdLen;

		/*---------- Fill data to Place hodler TD ----------*/
		ASSERT_STATUS_OK ( QueueOneITD(EdIdx, dataBuff, TdLen, FrameIdx) );
		
		FrameIdx = (FrameIdx + HcdED(EdIdx)->FramePeriod) % (1<<16);
		dataBuff += TdLen;
	}
	return HCD_STATUS_OK;
}

HCD_STATUS OHcdDataTransfer( uint32_t PipeHandle, uint8_t* const buffer, uint32_t const length, uint32_t* const pActualTransferred )
{
	uint8_t HostID, EdIdx;
	uint32_t ExpectedLength;

	if (buffer == NULL || length == 0 )
	{
		ASSERT_STATUS_OK_MESSAGE(HCD_STATUS_PARAMETER_INVALID, "Data Buffer is NULL or Transfer Length is 0");
	}
	
	ASSERT_STATUS_OK ( PipehandleParse(PipeHandle, &HostID, &EdIdx) );

	ExpectedLength = (length != HCD_ENDPOINT_MAXPACKET_XFER_LEN) ? length : HcdED(EdIdx)->hcED.MaxPackageSize; /* LUFA adaption, receive only 1 data transaction */

	if ( IsIsoEndpoint(EdIdx) ) /* Iso Transfer */
	{
		ASSERT_STATUS_OK( QueueITDs(EdIdx, buffer, ExpectedLength) );
	}else
	{
		ASSERT_STATUS_OK( QueueGTDs(EdIdx, buffer, ExpectedLength, 0) );

		if (HcdED(EdIdx)->ListIndex == BULK_LIST_HEAD)
		{
			/* set bulk list filled */
			OHCI_REG(HostID)->HcCommandStatus |= HC_COMMAND_STATUS_BulkListFilled;
		}
	}

	/* wait for semaphore compete TDs */
	ASSERT_STATUS_OK ( WaitForTransferComplete(EdIdx) );
	
	if (pActualTransferred)
	{
		*pActualTransferred = HcdED(EdIdx)->TotalTransferCount; /* Actual length transfer */
	}
	HcdED(EdIdx)->TotalTransferCount = 0; /* reset transfer count */

	return HCD_STATUS_OK;
}

/*=======================================================================*/
/* OHCD INTERRUPT HANDLERS                     */
/*=======================================================================*/

/*********************************************************************//**
 * @brief		
 * @param[in]	HostID		Host Controller Number
 * @param[in]	deviceConnect	Connection Status
 *				0		: Device is connected
 *				others	: Device is disconnected
 *
 * @return 		HCD_STATUS
 *				- HCD_STATUS_OK	: function performs successfully
 *				- Others		: Error occurs
 * Note: 
 **********************************************************************/
static HCD_STATUS OHciRhStatusChangeIsr(uint8_t HostID, uint32_t deviceConnect)
{
	if (deviceConnect)	/* Device Attached */
	{
		USB_Host_Enumerate(HostID);
	}else /* Device detached */
	{
		USB_Host_DeEnumerate(HostID);
	}

	return HCD_STATUS_OK;
}

/*********************************************************************//**
 * @brief		Process Done Queue List and free complete TDs
 * @param[in]	HostID		Host Controller Number
 * @param[in]	donehead	Done Head of retired TDs
 * @return 		None
 * Note: 
 **********************************************************************/
static void ProcessDoneQueue(uint8_t HostID, uint32_t donehead)
{
	PHC_GTD pCurTD = (PHC_GTD) donehead;
	PHC_GTD pTDList = NULL;

	/* do nothing if done queue is empty */
	if (!donehead)
		return;

	/* reverse done queue order */	
	do 
	{
		uint32_t nextTD = pCurTD->NextTD;
		pCurTD->NextTD = (uint32_t) pTDList;
		pTDList = pCurTD;
		pCurTD = (PHC_GTD) nextTD;
	} while (pCurTD);

	while(pTDList != NULL)
	{
		pCurTD	= pTDList;
		pTDList = (PHC_GTD) pTDList->NextTD;

		/* Cannot determine EdIdx because GTD and ITD have different offsets for EdIdx  */
		if ( ((uint32_t) pCurTD) <=  ((uint32_t) HcdITD(MAX_ITD-1)) )	/* ISO TD address range */
		{
			PHCD_IsoTransferDescriptor pItd = (PHCD_IsoTransferDescriptor) pCurTD;
			uint32_t EdIdx = pItd->EdIdx;

			if(pItd->ConditionCode == 0)
			{
				FreeItd(pItd);

				/* remove completed TD from usb request list, if request list is now empty complete usb request */
				/* Post Semaphore to signal TDs are transfer */
				if( HcdED(EdIdx)->semaphore )
					HcdED(EdIdx)->semaphore--;
				else
					HcdED(EdIdx)->status = (uint32_t)HCD_STATUS_TRANSFER_TooLessQueuedTD;
			}else /* TD complete with error */
			{
				HcdED(EdIdx)->status = pItd->ConditionCode;
			}
		}else /* GTD */
		{
			PHCD_GeneralTransferDescriptor pGtd = (PHCD_GeneralTransferDescriptor) pCurTD;
			uint32_t EdIdx = pGtd->EdIdx;

			if (pGtd->hcGTD.CurrentBufferPointer) 
			{
				pGtd->TransferCount -= ( ( (((uint32_t)pGtd->hcGTD.BufferEnd) ^ ((uint32_t)pGtd->hcGTD.CurrentBufferPointer)) & 0xFFFFF000 ) ? 0x00001000 : 0 ) + 
										( ((uint32_t)pGtd->hcGTD.BufferEnd) & 0x00000FFF) - ( ((uint32_t)pGtd->hcGTD.CurrentBufferPointer) & 0x00000FFF) + 1;
			}
			
			/* increase usb request transfer count */
			HcdED(EdIdx)->TotalTransferCount += pGtd->TransferCount;

			/* check completion code */
			if (pGtd->hcGTD.ConditionCode == 0 )
			{
				/* free TD */
				FreeGtd(pGtd);

				/* remove completed TD from usb request list, if request list is now empty complete usb request */
				/* Post Semaphore to signal TDs are transfer */
				if( HcdED(EdIdx)->semaphore )
					HcdED(EdIdx)->semaphore--;
				else
					HcdED(EdIdx)->status = (uint32_t)HCD_STATUS_TRANSFER_TooLessQueuedTD;

			}else /* TD complete with error */
			{
				HcdED(EdIdx)->status = pGtd->hcGTD.ConditionCode;
			}
		}
	}
}
#if SCHEDULING_OVRERRUN_INTERRUPT
static HCD_STATUS OHciSchedulingOverrunIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

#if SOF_INTERRUPT
static HCD_STATUS OHciStartofFrameIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

#if RESUME_DETECT_INTERRUPT
static HCD_STATUS OHciResumeDetectedIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

#if UNRECOVERABLE_ERROR_INTERRUPT
static HCD_STATUS OHciUnrecoverableErrorIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

#if FRAMENUMBER_OVERFLOW_INTERRUPT
static HCD_STATUS OHciFramenumberOverflowIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

#if OWNERSHIP_CHANGE_INTERRUPT
static HCD_STATUS OHciOwnershipChangeIsr(uint8_t HostID)
{
	return HCD_STATUS_OK;
}
#endif

/*********************************************************************//**
 * @brief		USB Hardware Interrupt Handler
 * @param[in]	HostID	The Host Controller Which the interrupt occurred
 * @return 		None
 * Note: 
 **********************************************************************/
void OHcdIrqHandler(uint8_t HostID)
{
	uint32_t IntStatus;
	
	IntStatus = OHCI_REG(HostID)->HcInterruptStatus & OHCI_REG(HostID)->HcInterruptEnable ;

	if (IntStatus == 0)
		return;

	/* disable all interrupt for processing */
	OHCI_REG(HostID)->HcInterruptDisable = HC_INTERRUPT_MasterInterruptEnable;

	/* Process RootHub Status Change */
	if (IntStatus & HC_INTERRUPT_RootHubStatusChange)
	{
		/* only 1 port/host --> skip to get the number of ports */
		if (OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_ConnectStatusChange)
		{
			if (OHCI_REG(HostID)->HcRhStatus & HC_RH_STATUS_DeviceRemoteWakeupEnable)	/* means a remote wakeup event */
			{

			}else{

			}
			/* clear CSC bit */
			OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_ConnectStatusChange;
			
			OHciRhStatusChangeIsr(HostID, OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_CurrentConnectStatus);
		}

		if (OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_PortEnableStatusChange)
		{
			/* clear PESC */
			OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortEnableStatusChange;
		}

		if (OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_PortSuspendStatusChange)
		{
			/* clear PSSC */
			OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortSuspendStatusChange;
		}

		if (OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_OverCurrentIndicatorChange)
		{
			/* Should have a over current handler to avoid physical damage */
			/* clear OCIC */
			OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_OverCurrentIndicatorChange;
		}

		if (OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_PortResetStatusChange)
		{
			/* clear PRSC */
			OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortResetStatusChange;
		}
	}

	if (IntStatus & HC_INTERRUPT_WritebackDoneHead)
	{
		ProcessDoneQueue(HostID, Align16( ohci_data[HostID].hcca.HccaDoneHead ) );
	}

#if SCHEDULING_OVRERRUN_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_SchedulingOverrun)
	{
		OHciSchedulingOverrunIsr(HostID);
	}
#endif

#if SOF_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_StartofFrame)
	{
		OHciStartofFrameIsr(HostID);
	}
#endif

#if RESUME_DETECT_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_ResumeDetected)
	{
		OHciResumeDetectedIsr(HostID);
	}
#endif

#if UNRECOVERABLE_ERROR_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_UnrecoverableError)
	{
		OHciUnrecoverableErrorIsr(HostID);
	}
#endif

#if FRAMENUMBER_OVERFLOW_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_FrameNumberOverflow)
	{
		OHciFramenumberOverflowIsr(HostID);
	}
#endif

#if OWNERSHIP_CHANGE_INTERRUPT
	if (OHCI_REG(HostID)->HcInterruptStatus & HC_INTERRUPT_OwnershipChange)
	{
		OHciOwnershipChangeIsr(HostID);
	}
#endif

	OHCI_REG(HostID)->HcInterruptStatus = IntStatus; /* Clear HcInterruptStatus */
	OHCI_REG(HostID)->HcInterruptEnable = HC_INTERRUPT_MasterInterruptEnable;

}

/*==========================================================================*/
/* HELPER FUNCTIONS                         											*/
/*==========================================================================*/
/** Direction, DataToggle parameter only has meaning for control transfer, for other transfer use 0 for these paras */
static HCD_STATUS QueueOneGTD (uint32_t EdIdx, uint8_t* const CurrentBufferPointer, uint32_t xferLen, uint8_t DirectionPID, uint8_t DataToggle)
{
	PHCD_GeneralTransferDescriptor TailP;

	TailP = ( (PHCD_GeneralTransferDescriptor) HcdED(EdIdx)->hcED.TailP ) ;
	TailP->hcGTD.DirectionPID = DirectionPID;
	TailP->hcGTD.DataToggle = DataToggle;
	TailP->hcGTD.CurrentBufferPointer = CurrentBufferPointer;
	TailP->hcGTD.BufferEnd = (xferLen) ? (CurrentBufferPointer+xferLen-1) : NULL;
	TailP->TransferCount = xferLen;

	/* Create a new place holder TD & link setup TD to the new place holder */
	ASSERT_STATUS_OK ( AllocGtdForEd(EdIdx) );
	HcdED(EdIdx)->semaphore++;

	return HCD_STATUS_OK;
}

static HCD_STATUS QueueGTDs (uint32_t EdIdx, uint8_t* dataBuff, uint32_t xferLen, uint8_t Direction)
{
	while (xferLen > 0)
	{
		uint16_t TdLen;

		TdLen = (xferLen < 0x1000) ? xferLen : 0x1000;
		xferLen -= TdLen;

		ASSERT_STATUS_OK ( QueueOneGTD(EdIdx, dataBuff, TdLen, Direction, 0) );
		dataBuff += TdLen;
	}
	return HCD_STATUS_OK;
}

static HCD_STATUS WaitForTransferComplete( uint8_t EdIdx ) 
{
#ifndef __TEST__
	while ( HcdED(EdIdx)->semaphore && HcdED(EdIdx)->status == 0){
	}
	return (HCD_STATUS) HcdED(EdIdx)->status ;
#else
	return HCD_STATUS_OK;
#endif
}

static __INLINE HCD_STATUS InsertEndpoint( uint8_t HostID, uint32_t EdIdx, uint8_t ListIndex )
{
	PHC_ED list_head;
	list_head = &(ohci_data[HostID].staticEDs[ListIndex]);
	
	HcdED(EdIdx)->hcED.NextED = list_head->NextED;	
	list_head->NextED = (uint32_t) HcdED(EdIdx);

// 	if ( IsInterruptEndpoint(EdIdx) )
// 	{
// 		OHCI_HOST_DATA->staticEDs[ListIndex].TailP += HcdED(EdIdx)->hcED.MaxPackageSize;	/* increase the bandwidth for the found list */
// 	}

	return HCD_STATUS_OK;
}

/* Remove ED from its list */
static __INLINE HCD_STATUS RemoveEndpoint( uint8_t HostID, uint32_t EdIdx )
{
	PHCD_EndpointDescriptor prevED;

	prevED = (PHCD_EndpointDescriptor) &(ohci_data[HostID].staticEDs[HcdED(EdIdx)->ListIndex]); 
	while(prevED->hcED.NextED != (uint32_t) HcdED(EdIdx) )
	{
		prevED = (PHCD_EndpointDescriptor) (prevED->hcED.NextED) ;
	}

// 	if ( IsInterruptEndpoint(EdIdx) )
// 	{
// 		OHCI_HOST_DATA->staticEDs[HcdED(EdIdx)->ListIndex].TailP -= HcdED(EdIdx)->hcED.MaxPackageSize;	/* decrease the bandwidth for the removed list */
// 	}
	prevED->hcED.NextED = HcdED(EdIdx)->hcED.NextED;

	return HCD_STATUS_OK;
}

__INLINE uint8_t FindInterruptTransferListIndex( uint8_t HostID, uint8_t Interval )
{
	uint8_t ListLeastBandwidth;
	uint8_t ListEnd;
	uint8_t ListIdx = INTERRUPT_32ms_LIST_HEAD;

	/* Find the correct interval list with right power of 2, i.e: 1,2,4,8,16,32 ms */
	while ( (ListIdx >= Interval) && (ListIdx >>= 1) ) {}
	ListEnd = ListIdx << 1;

	/* Find the least bandwidth in the same interval */
	/* Note: For Interrupt Static ED (0 to 62), TailP is used to store the accumulated bandwidth of the list */
	for (ListLeastBandwidth=ListIdx; ListIdx <= ListEnd; ListIdx++ )
	{
		if ( ohci_data[HostID].staticEDs[ListIdx].TailP < ohci_data[HostID].staticEDs[ListLeastBandwidth].TailP )
		{
			ListLeastBandwidth = ListIdx;
		}
	}
	return ListLeastBandwidth;
}

/* build static EDs tree for periodic transfer */
static __INLINE void BuildPeriodicStaticEdTree( uint8_t HostID )
{
#if 0	/* We dont need to manage bandwidth this hard */
#if INTERRUPT_LIST_ENABLE
	/* Build full binary tree for interrupt list */
	uint32_t idx, count;
	uint32_t Balance[16] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};

	/* build static tree for 1 -> 16 ms */
	OHCI_HOST_DATA->staticEDs[0].NextED = 0;
	for (idx=1; idx < INTERRUPT_32ms_LIST_HEAD; idx++)
	{
		OHCI_HOST_DATA->staticEDs[idx].NextED = (uint32_t) &(OHCI_HOST_DATA->staticEDs[(idx-1)/2]);
	}
	/* create 32ms EDs which will be assigned to HccaInterruptTable */
	for (count=0, idx=INTERRUPT_32ms_LIST_HEAD; count < 32; count++, idx++)
	{
		OHCI_HOST_DATA->staticEDs[idx].NextED = (uint32_t) &(OHCI_HOST_DATA->staticEDs[ Balance[count & 0xF] + INTERRUPT_16ms_LIST_HEAD ]);
	}
	/* Hook to HCCA interrupt Table */
	for (idx = 0; idx < 32; idx++)
	{
		OHCI_HOST_DATA->hcca.HccaIntTable[idx] = (uint32_t) &(OHCI_HOST_DATA->staticEDs[idx+INTERRUPT_32ms_LIST_HEAD]) ;
	}
	OHCI_HOST_DATA->staticEDs[INTERRUPT_1ms_LIST_HEAD].NextED = (uint32_t) &(OHCI_HOST_DATA->staticEDs[ISO_LIST_HEAD]);
#elif ISO_LIST_ENABLE
	for (idx = 0; idx < 32; idx++)
	{
		OHCI_HOST_DATA->hcca.HccaIntTable[idx] = (uint32_t) &(OHCI_HOST_DATA->staticEDs[ISO_LIST_HEAD]) ;
	}
#endif
#else
	/* Treat all interrupt interval as 1ms (maximum rate) */
	uint32_t idx;
	for (idx = 0; idx < 32; idx++)
	{
		ohci_data[HostID].hcca.HccaIntTable[idx] = (uint32_t) &(ohci_data[HostID].staticEDs[INTERRUPT_1ms_LIST_HEAD]) ;
	}
	/* ISO_LIST_HEAD is an alias for INTERRUPT_1ms_LIST_HEAD */
#endif

}

static __INLINE uint32_t Align16 (uint32_t Value)
{
	return (Value & 0xFFFFFFF0UL); /* Bit 31 .. 4 */
}

static __INLINE PHCD_EndpointDescriptor HcdED(uint8_t idx)
{
	return &(ohci_data[0/*HostID*/].EDs[idx]);
}

static __INLINE PHCD_GeneralTransferDescriptor HcdGTD(uint8_t idx)
{
	return &(ohci_data[0/*HostID*/].gTDs[idx]);
}

static __INLINE PHCD_IsoTransferDescriptor HcdITD(uint8_t idx)
{
#if ISO_LIST_ENABLE
	return &(ohci_data[0/*HostID*/].iTDs[idx]);
#else
	return 0;
#endif
}

static __INLINE Bool IsIsoEndpoint( uint8_t EdIdx )
{
	return HcdED(EdIdx)->hcED.Format ;
}

static __INLINE Bool IsInterruptEndpoint (uint8_t EdIdx)
{
	return ( (HcdED(EdIdx)->ListIndex < CONTROL_LIST_HEAD) && !IsIsoEndpoint(EdIdx) );
}

static void PipehandleCreate(uint32_t* pPipeHandle, uint8_t HostID, uint8_t EdIdx)
{
	*pPipeHandle = ((uint32_t)(HostID << 8)) + EdIdx;
}
static HCD_STATUS PipehandleParse(uint32_t Pipehandle, uint8_t* HostID, uint8_t* EdIdx)
{
	*HostID = Pipehandle >> 8;
	*EdIdx = Pipehandle & 0xFF;
	if (*HostID >= MAX_HOST_CONTROLLER || *EdIdx >= MAX_ED || HcdED(*EdIdx)->inUse == 0)
		return HCD_STATUS_PIPEHANDLE_INVALID;
	else
		return HCD_STATUS_OK;
}

static __INLINE HCD_STATUS AllocEd( uint8_t DeviceAddr, HCD_USB_SPEED DeviceSpeed, uint8_t EndpointNumber, HCD_TRANSFER_TYPE TransferType, HCD_TRANSFER_DIR TransferDir, uint16_t MaxPacketSize, uint8_t Interval, uint32_t* pEdIdx )
{
	/* Looking for free EDs */
	for ((*pEdIdx) = 0; ((*pEdIdx) < MAX_ED) && HcdED((*pEdIdx))->inUse; (*pEdIdx)++) 
	{

	}
	if ((*pEdIdx) >= MAX_ED)
		return HCD_STATUS_NOT_ENOUGH_ENDPOINT;

	/* Init Data for new ED */
	memset( HcdED(*pEdIdx), 0, sizeof(HCD_EndpointDescriptor) );

	HcdED((*pEdIdx))->inUse = 1;

	HcdED((*pEdIdx))->hcED.FunctionAddr = DeviceAddr;
	HcdED((*pEdIdx))->hcED.EndpointNumber = EndpointNumber; /* Endpoint number only has 4 bits */
	HcdED((*pEdIdx))->hcED.Direction = (TransferType == CONTROL_TRANSFER) ? 0 : ((TransferDir == OUT_TRANSFER) ? 1 : 2 ) ;
	HcdED((*pEdIdx))->hcED.Speed = (DeviceSpeed == FULL_SPEED) ? 0 : 1;
	HcdED((*pEdIdx))->hcED.Skip = 0;
	HcdED((*pEdIdx))->hcED.Format = (TransferType == ISOCHRONOUS_TRANSFER) ? 1 : 0;
	HcdED((*pEdIdx))->hcED.MaxPackageSize = MaxPacketSize;
	
	/* Allocate Place Holder TD as suggested by OHCI 5.2.8 */
	if (TransferType != ISOCHRONOUS_TRANSFER)
	{
		ASSERT_STATUS_OK ( AllocGtdForEd(*pEdIdx) );
	}
	else
	{
		if (Interval < 4) /*-- Period < 8 --*/
		{
			HcdED((*pEdIdx))->MaxXactPerITD = 1 << ( 4 - Interval ) ;	/*-- Interval 1 => 8, 2 => 4, 3 => 2 --*/
			HcdED((*pEdIdx))->FramePeriod = 1;
		}else
		{
			HcdED((*pEdIdx))->MaxXactPerITD = 1;
			HcdED((*pEdIdx))->FramePeriod = 1 << ( Interval - 4 );	/*-- Frame step 4 => 1, 5 => 2, 6 => 3 --*/
		}

		ASSERT_STATUS_OK ( AllocItdForEd(*pEdIdx) );
	}

	return HCD_STATUS_OK;
}

static HCD_STATUS AllocGtdForEd(uint8_t EdIdx)
{
	uint32_t GtdIdx;

	/* Allocate new GTD */
	for (GtdIdx = 0; (GtdIdx < MAX_GTD) && HcdGTD(GtdIdx)->inUse; GtdIdx++) {}

	if (GtdIdx < MAX_GTD)
	{
		HcdGTD(GtdIdx)->inUse = 1;
		HcdGTD(GtdIdx)->EdIdx = EdIdx;
		HcdGTD(GtdIdx)->TransferCount = 0;

		/***************    Control (word 0) ****************/
		/* Buffer rounding:    R = 1b (yes)                 */
		/* Direction/PID:      DP = 00b (SETUP)             */
		/* Delay Interrupt:    DI = 000b (interrupt)		*/
		/* Data Toggle:        DT = 00b (from ED)		    */
		/* Error Count:        EC = 00b                     */
		/* Condition Code:     CC = 1110b (not accessed)    */
		/****************************************************/
		HcdGTD(GtdIdx)->hcGTD.BufferRounding = 1;
		HcdGTD(GtdIdx)->hcGTD.DirectionPID = 0;
		HcdGTD(GtdIdx)->hcGTD.DelayInterrupt = 0;
		HcdGTD(GtdIdx)->hcGTD.DataToggle = 0;
		HcdGTD(GtdIdx)->hcGTD.ErrorCount = 0;
		HcdGTD(GtdIdx)->hcGTD.ConditionCode = 0xe;

		HcdGTD(GtdIdx)->hcGTD.CurrentBufferPointer = HcdGTD(GtdIdx)->hcGTD.BufferEnd = NULL;
		HcdGTD(GtdIdx)->hcGTD.NextTD = 0;

		/* link new GTD to the Endpoint */
		if (HcdED(EdIdx)->hcED.TailP) /* already have place holder */
		{
			( (PHCD_GeneralTransferDescriptor) HcdED(EdIdx)->hcED.TailP )->hcGTD.NextTD = (uint32_t) HcdGTD(GtdIdx);
		}else /* have no dummy TD attached to the ED */
		{
			HcdED(EdIdx)->hcED.HeadP.HeadTD = ((uint32_t) HcdGTD(GtdIdx)) ;
		}
		HcdED(EdIdx)->hcED.TailP = (uint32_t) HcdGTD(GtdIdx);

		return HCD_STATUS_OK;
	}
	else
	{
		return HCD_STATUS_NOT_ENOUGH_GTD;
	}

}
static HCD_STATUS AllocItdForEd(uint8_t EdIdx)
{
	uint32_t ItdIdx;

	for (ItdIdx = 0; (ItdIdx < MAX_ITD) && HcdITD(ItdIdx)->inUse; ItdIdx++) {}
	
	if (ItdIdx < MAX_ITD)
	{
		memset( HcdITD(ItdIdx), 0, sizeof(HCD_IsoTransferDescriptor) );
		HcdITD(ItdIdx)->inUse = 1;
		HcdITD(ItdIdx)->EdIdx = EdIdx;
		HcdITD(ItdIdx)->ConditionCode = (uint32_t) HCD_STATUS_TRANSFER_NotAccessed;

		/* link new ITD to the Endpoint */
		if (HcdED(EdIdx)->hcED.TailP) /* already have place holder */
		{
			( (PHCD_IsoTransferDescriptor) HcdED(EdIdx)->hcED.TailP )->NextTD = (uint32_t) HcdITD(ItdIdx);
		}else /* have no dummy TD attached to the ED */
		{
			HcdED(EdIdx)->hcED.HeadP.HeadTD = ((uint32_t) HcdITD(ItdIdx)) ;
		}
		HcdED(EdIdx)->hcED.TailP = (uint32_t) HcdITD(ItdIdx);

		return HCD_STATUS_OK;
	}else	
		return HCD_STATUS_NOT_ENOUGH_ITD;
}


static __INLINE HCD_STATUS FreeED( uint8_t EdIdx ) 
{
	/* Remove Place holder TD */
	if ( IsIsoEndpoint(EdIdx) )
	{
		FreeItd( (PHCD_IsoTransferDescriptor) HcdED(EdIdx)->hcED.TailP );
	}else
	{
		FreeGtd( (PHCD_GeneralTransferDescriptor) HcdED(EdIdx)->hcED.TailP );
	}

	HcdED(EdIdx)->status = HCD_STATUS_TRANSFER_NotAccessed;
	HcdED(EdIdx)->inUse = 0;

	return HCD_STATUS_OK;
}

static __INLINE HCD_STATUS FreeGtd(PHCD_GeneralTransferDescriptor pGtd)
{
	pGtd->inUse = 0;
	return HCD_STATUS_OK;
}

static __INLINE HCD_STATUS FreeItd(PHCD_IsoTransferDescriptor pItd)
{
	pItd->inUse = 0;
	return HCD_STATUS_OK;
}

/*==========================================================================*/
/* HOST API                        											*/
/*==========================================================================*/
static __INLINE HCD_STATUS OHciHostInit(uint8_t HostID)
{
	uint32_t idx;

	if( sizeof(OHCI_HOST_DATA_Type) > 0x4000 )	/* Host data exceed 16 KB */
	{
		ASSERT_STATUS_OK( HCD_STATUS_NOT_ENOUGH_MEMORY );
	}

	memset(&ohci_data[HostID], 0, sizeof(OHCI_HOST_DATA_Type));
	/* Skip writing 1s to HcHCCA, assume it is 256 aligned */

	/* set skip bit for all static EDs */
	for (idx=0; idx < MAX_STATIC_ED; idx++)
	{
		ohci_data[HostID].staticEDs[idx].Skip = 1;
	}

	/* Periodic List Initialization */
	BuildPeriodicStaticEdTree(HostID);

	/* Initialize OHCI registers */
	OHCI_REG(HostID)->HcControl = 0;
	OHciHostOperational(HostID); /* have to turn HC to operational mode before setting up below registers*/	

	/* HcFmInterval Register Init */
	OHCI_REG(HostID)->HcFmInterval = HC_FMINTERVAL_DEFAULT;

	/* HcPeriodicStart Register Init */
	OHCI_REG(HostID)->HcPeriodicStart = PERIODIC_START;

	/* Static Control List Head */
	OHCI_REG(HostID)->HcControlHeadED = (uint32_t) &(ohci_data[HostID].staticEDs[CONTROL_LIST_HEAD]);

	/* Static Bulk List Head */
	OHCI_REG(HostID)->HcBulkHeadED = (uint32_t) &(ohci_data[HostID].staticEDs[BULK_LIST_HEAD]);

	/* Hook Hcca */
	OHCI_REG(HostID)->HcHCCA = (uint32_t) &(ohci_data[HostID].hcca);

	/* Set up HcControl */
	OHCI_REG(HostID)->HcControl |= CONTROL_BULK_SERVICE_RATIO |
		(INTERRUPT_ROUTING ? HC_CONTROL_InterruptRouting : 0) |
		(REMOTE_WAKEUP_CONNECTED ? HC_CONTROL_RemoteWakeupConnected : 0) |
		(REMOTE_WAKEUP_ENABLE ? HC_CONTROL_RemoteWakeupEnable : 0) |
		HC_CONTROL_ControlListEnable | HC_CONTROL_BulkListEnable |
		(ISO_LIST_ENABLE ? (HC_CONTROL_PeriodListEnable|HC_CONTROL_IsochronousEnable) : 
		(INTERRUPT_LIST_ENABLE ? HC_CONTROL_PeriodListEnable : 0));

	/* Set Global Power */
	OHCI_REG(HostID)->HcRhStatus = HC_RH_STATUS_LocalPowerStatusChange;

	// HcInterrupt Registers Init
	/* Clear Interrupt Status */
	OHCI_REG(HostID)->HcInterruptStatus |= OHCI_REG(HostID)->HcInterruptStatus;
	/* Disable all interrupts */
	OHCI_REG(HostID)->HcInterruptDisable = HC_INTERRUPT_ALL;
	/* Enable necessary Interrupts */
	OHCI_REG(HostID)->HcInterruptEnable = HC_INTERRUPT_MasterInterruptEnable | HC_INTERRUPT_WritebackDoneHead | HC_INTERRUPT_RootHubStatusChange |
		(SCHEDULING_OVRERRUN_INTERRUPT ? HC_INTERRUPT_SchedulingOverrun : 0 ) |
		(SOF_INTERRUPT ? HC_INTERRUPT_StartofFrame : 0) |
		(RESUME_DETECT_INTERRUPT ? HC_INTERRUPT_ResumeDetected : 0) |
		(UNRECOVERABLE_ERROR_INTERRUPT ? HC_INTERRUPT_UnrecoverableError : 0) |
		(FRAMENUMBER_OVERFLOW_INTERRUPT ? HC_INTERRUPT_FrameNumberOverflow : 0) |
		(OWNERSHIP_CHANGE_INTERRUPT ? HC_INTERRUPT_OwnershipChange : 0 ) ;

	return HCD_STATUS_OK;
}

static __INLINE HCD_STATUS OHciHostReset(uint8_t HostID)
{
	OHCI_REG(HostID)->HcCommandStatus = HC_COMMAND_STATUS_HostControllerReset;

	/* Wait indefinitely (may need a time-out here) */
	while( OHCI_REG(HostID)->HcCommandStatus & HC_COMMAND_STATUS_HostControllerReset){}

	return HCD_STATUS_OK;
}

static __INLINE HCD_STATUS OHciHostSuspend(uint8_t HostID)
{
	OHCI_REG(HostID)->HcControl = (OHCI_REG(HostID)->HcControl & (~HC_CONTROL_HostControllerFunctionalState)) | (HC_HOST_SUSPEND << 6) ;
	return HCD_STATUS_OK;
}
static __INLINE HCD_STATUS OHciHostOperational(uint8_t HostID)
{
	OHCI_REG(HostID)->HcControl = (OHCI_REG(HostID)->HcControl & (~HC_CONTROL_HostControllerFunctionalState)) | (HC_HOST_OPERATIONAL << 6) ;
	return HCD_STATUS_OK;
}

/*==========================================================================*/
/* PORT API                        											*/
/*==========================================================================*/
static __INLINE HCD_STATUS OHciRhPortPowerOn(uint8_t HostID, uint8_t uPortNumber)
{
	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortPowerStatus; /* SetPortPower */
	/* need to delay here POTPGT */
	HcdDelayMS( 2*( (OHCI_REG(HostID)->HcRhDescriptorA & HC_RH_DESCRIPTORA_PowerOnToPowerGoodTime) >> 24 ) );

	return HCD_STATUS_OK;
}
static __INLINE HCD_STATUS OHciRhPortPowerOff(uint8_t HostID, uint8_t uPortNumber)
{
	OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_LowSpeedDeviceAttached; /* ClearPortPower */
	return HCD_STATUS_OK;
}
static __INLINE HCD_STATUS OHciRhPortSuspend(uint8_t HostID, uint8_t uPortNumber)
{
	if ( OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_CurrentConnectStatus) /* If device is connected */
	{
		OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortSuspendStatus; /* SetPortSuspend */
	}
	HcdDelayMS(3); /* 3ms for device to suspend */

	return HCD_STATUS_OK;
}
static __INLINE HCD_STATUS OHciRhPortResume(uint8_t HostID, uint8_t uPortNumber)
{
	if ( OHCI_REG(HostID)->HcRhPortStatus1 & HC_RH_PORT_STATUS_CurrentConnectStatus) /* If port is currently suspended */
	{
		OHCI_REG(HostID)->HcRhPortStatus1 = HC_RH_PORT_STATUS_PortOverCurrentIndicator; /* ClearSuspendStatus */
	}
	HcdDelayMS(20); /* 20ms for device to resume */

	return HCD_STATUS_OK;
}
#endif
