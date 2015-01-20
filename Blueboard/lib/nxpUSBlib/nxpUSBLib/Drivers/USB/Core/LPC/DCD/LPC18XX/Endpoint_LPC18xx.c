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

#define  __INCLUDE_FROM_USB_DRIVER
#include "../../../USBMode.h"

#if defined(__LPC18XX__) && defined(USB_CAN_BE_DEVICE)
#include "../../../Endpoint.h"
#include <string.h>

DeviceQueueHead dQueueHead[USED_PHYSICAL_ENDPOINTS] ATTR_ALIGNED(2048) __DATA(USBRAM_SECTION);
DeviceTransferDescriptor dTransferDescriptor[USED_PHYSICAL_ENDPOINTS] ATTR_ALIGNED(32) __DATA(USBRAM_SECTION);

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void HAL18XX_Reset (void)
{
  uint32_t i;

  /* disable all EPs */
  LPC_USB0->ENDPTCTRL0 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);
  LPC_USB0->ENDPTCTRL1 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);
  LPC_USB0->ENDPTCTRL2 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);
  LPC_USB0->ENDPTCTRL3 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);
  LPC_USB0->ENDPTCTRL4 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);
  LPC_USB0->ENDPTCTRL5 &= ~(ENDPTCTRL_RxEnable | ENDPTCTRL_TxEnable);

  /* Clear all pending interrupts */
  LPC_USB0->ENDPTNAK		= 0xFFFFFFFF;
  LPC_USB0->ENDPTNAKEN		= 0;
  LPC_USB0->USBSTS_D		= 0xFFFFFFFF;
  LPC_USB0->ENDPTSETUPSTAT	= LPC_USB0->ENDPTSETUPSTAT;
  LPC_USB0->ENDPTCOMPLETE	= LPC_USB0->ENDPTCOMPLETE;
  while (LPC_USB0->ENDPTPRIME);                  /* Wait until all bits are 0 */
  LPC_USB0->ENDPTFLUSH = 0xFFFFFFFF;
  while (LPC_USB0->ENDPTFLUSH); /* Wait until all bits are 0 */

  /* Set the interrupt Threshold control interval to 0 */
  LPC_USB0->USBCMD_D &= ~0x00FF0000;

  /* Configure the Endpoint List Address */
  /* make sure it in on 64 byte boundary !!! */
  /* init list address */
  LPC_USB0->ENDPOINTLISTADDR = (uint32_t)dQueueHead;
  
  /* Enable interrupts: USB interrupt, error, port change, reset, suspend, NAK interrupt */
  LPC_USB0->USBINTR_D =  USBINTR_D_UsbIntEnable | USBINTR_D_UsbErrorIntEnable | 
						USBINTR_D_PortChangeIntEnable | USBINTR_D_UsbResetEnable |
						USBINTR_D_SuspendEnable | USBINTR_D_NAKEnable;

  USB_Device_SetDeviceAddress(0);
  
  endpointselected = 0;
  for(i=0;i<ENDPOINT_TOTAL_ENDPOINTS;i++)
	  endpointhandle[i] = 0;

  return;
}

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
bool Endpoint_ConfigureEndpoint(const uint8_t Number, const uint8_t Type,
							   const uint8_t Direction, const uint16_t Size, const uint8_t Banks)
{
	uint32_t PhyEP = 2*Number + (Direction == ENDPOINT_DIR_OUT ? 0 : 1);
	uint32_t EndPtCtrl = ENDPTCTRL_REG(Number);

	memset(&dQueueHead[PhyEP], 0, sizeof(DeviceQueueHead) );
	
	dQueueHead[PhyEP].MaxPacketSize = Size & 0x3ff;
	dQueueHead[PhyEP].IntOnSetup = 1;
	dQueueHead[PhyEP].ZeroLengthTermination = 1;
	dQueueHead[PhyEP].overlay.NextTD = LINK_TERMINATE;
		
	if (Direction == ENDPOINT_DIR_OUT)
	{
		EndPtCtrl &= ~0x0000FFFF;
		EndPtCtrl |= ((Type << 2) & ENDPTCTRL_RxType) | ENDPTCTRL_RxEnable | ENDPTCTRL_RxToggleReset;
		LPC_USB0->ENDPTNAKEN |=  (1 << EP_Physical2BitPosition(PhyEP));
	}else /* ENDPOINT_DIR_IN */
	{
		EndPtCtrl &= ~0xFFFF0000;
		EndPtCtrl |= ((Type << 18) & ENDPTCTRL_TxType)| ENDPTCTRL_TxEnable| ENDPTCTRL_TxToggleReset;
	}
	ENDPTCTRL_REG(Number) = EndPtCtrl;

	endpointhandle[Number] = (Number==ENDPOINT_CONTROLEP) ? ENDPOINT_CONTROLEP : PhyEP;
	return true;
}

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void DcdDataTransfer(uint8_t PhyEP, uint8_t *pData, uint32_t length)
{
	DeviceTransferDescriptor*  pDTD = (DeviceTransferDescriptor*) &dTransferDescriptor[ PhyEP ];

	while ( LPC_USB0->ENDPTSTAT & _BIT( EP_Physical2BitPosition(PhyEP) ) )	/* Endpoint is already primed */
	{
	}
	

	/* Zero out the device transfer descriptors */
	memset((void*)pDTD, 0, sizeof(DeviceTransferDescriptor));

	pDTD->NextTD = LINK_TERMINATE ; /* The next DTD pointer is INVALID */
	pDTD->TotalBytes = length;
	pDTD->IntOnComplete = 1;
	pDTD->Active = 1;

	pDTD->BufferPage[0] = (uint32_t) pData;
	pDTD->BufferPage[1] = ((uint32_t) pData + 0x1000) & 0xfffff000;
	pDTD->BufferPage[2] = ((uint32_t) pData + 0x2000) & 0xfffff000;
	pDTD->BufferPage[3] = ((uint32_t) pData + 0x3000) & 0xfffff000;
	pDTD->BufferPage[4] = ((uint32_t) pData + 0x4000) & 0xfffff000;

	dQueueHead[PhyEP].overlay.Halted = 0; /* this should be in USBInt */
	dQueueHead[PhyEP].overlay.Active = 0; /* this should be in USBInt */
	dQueueHead[PhyEP].overlay.NextTD = (uint32_t) &dTransferDescriptor[ PhyEP ];
	dQueueHead[PhyEP].TransferCount = length;
	
	/* prime the endpoint for transmit */
	LPC_USB0->ENDPTPRIME |= _BIT( EP_Physical2BitPosition(PhyEP) ) ;
}

void TransferCompleteISR(void)
{
	uint32_t ENDPTCOMPLETE = LPC_USB0->ENDPTCOMPLETE;
	LPC_USB0->ENDPTCOMPLETE = ENDPTCOMPLETE;
	
	if (ENDPTCOMPLETE)
	{
		uint8_t n;
		for (n = 0; n < USED_PHYSICAL_ENDPOINTS/2; n++) /* LOGICAL */
		{
			if ( ENDPTCOMPLETE & _BIT(n) ) /* OUT */
			{
				dQueueHead[2*n].TransferCount -= dQueueHead[2*n].overlay.TotalBytes;
				dQueueHead[2*n].IsOutReceived = 1;
			}
			if ( ENDPTCOMPLETE & _BIT( (n+16) ) ) /* IN */
			{
			}
		}
	}
}


void Endpoint_GetSetupPackage(uint8_t* pData)
{
	memcpy(pData, dQueueHead[0].SetupPackage, 8);
}

/********************************************************************//**
 * @brief
 * @param
 * @return
 *********************************************************************/
void DcdIrqHandler (uint8_t HostID)
{
	uint32_t USBSTS_D;

	USBSTS_D = LPC_USB0->USBSTS_D & LPC_USB0->USBINTR_D;                      /* Device Interrupt Status */
	if (USBSTS_D == 0)	/* avoid to clear disabled interrupt source */
		return;

	LPC_USB0->USBSTS_D = USBSTS_D; /* Acknowledge Interrupt */
	
	/* Process Interrupt Sources */	
	if (USBSTS_D & USBSTS_D_UsbInt)
	{		
		if (LPC_USB0->ENDPTSETUPSTAT)
		{
//			memcpy(SetupPackage, dQueueHead[0].SetupPackage, 8);
			/* Will be cleared by Endpoint_ClearSETUP */
		}
		
		if (LPC_USB0->ENDPTCOMPLETE)
		{
			TransferCompleteISR();
		}
	}
	
	if (USBSTS_D & USBSTS_D_NAK)					/* NAK */
	{
		uint32_t ENDPTNAK = LPC_USB0->ENDPTNAK & LPC_USB0->ENDPTNAKEN;
		LPC_USB0->ENDPTNAK = ENDPTNAK;

	    if (ENDPTNAK)  /* handle NAK interrupts */
	    {
			uint8_t LogicalEP;
			for (LogicalEP = 0; LogicalEP < USED_PHYSICAL_ENDPOINTS / 2; LogicalEP++)
			{
				if (ENDPTNAK & _BIT(LogicalEP)) /* Only OUT Endpoint is NAK enable */
				{
					uint8_t PhyEP = 2*LogicalEP;
					if ( ! (LPC_USB0->ENDPTSTAT & _BIT(LogicalEP)) ) /* Is In ready */
					{
						DcdDataTransfer(PhyEP, usb_data_buffer, 512);
					}
				}
			}
	    }
	}
	
	if (USBSTS_D & USBSTS_D_SofReceived)					/* Start of Frame Interrupt */
	{

	}

	if (USBSTS_D & USBSTS_D_ResetReceived)                      /* Reset */
	{
		HAL18XX_Reset();
		USB_DeviceState = DEVICE_STATE_Default;
		Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL, ENDPOINT_DIR_OUT, USB_Device_ControlEndpointSize,0);
		Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL, ENDPOINT_DIR_IN, USB_Device_ControlEndpointSize, 0);
	}

	if (USBSTS_D & USBSTS_D_SuspendInt)                   /* Suspend */
	{

	}

	if (USBSTS_D & USBSTS_D_PortChangeDetect)                  /* Resume */
	{

	}

	if (USBSTS_D & USBSTS_D_UsbErrorInt)					/* Error Interrupt */
	{
		//while(1){}
	}
}

#endif /*__LPC18XX__*/
