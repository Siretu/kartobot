/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbuser.c
 * Purpose: USB Custom User Module
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"
#include "cdcuser.h"
#include "mscuser.h"
#include "../USB.h"
#include "main.h"
#include "dbgD.h"
#include "utils.h"
#include "usbdesc.h"

/*
 *  USB Power Event Callback
 *   Called automatically on USB Power Event
 *    Parameter:       power: On(TRUE)/Off(FALSE)
 */

#if USB_POWER_EVENT
void USB_Power_Event (uint32_t  power) {
	if(power == 0){
		USB_Connected = 0;
		xprintfD("USB Disconnected");
	}else{
		log_info("USB Power Event: %lu  ReConnected?", power);
	}
}
#endif


/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */

#if USB_RESET_EVENT
void USB_Reset_Event (void) {
	xprintfD("USB Reset Event");
  USB_ResetCore();
  USB_ResetBuffers();
  USB_Connected = 0;
}
#endif


/*
 *  USB Suspend Event Callback
 *   Called automatically on USB Suspend Event
 */

#if USB_SUSPEND_EVENT
void USB_Suspend_Event (void) {
	xprintfD("USB Suspend Event");
}
#endif


/*
 *  USB Resume Event Callback
 *   Called automatically on USB Resume Event
 */

#if USB_RESUME_EVENT
void USB_Resume_Event (void) {
	xprintfD("USB Resume Event");
}
#endif


/*
 *  USB Remote Wakeup Event Callback
 *   Called automatically on USB Remote Wakeup Event
 */

#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) {
	xprintfD("USB WakeUp Event");
}
#endif


/*
 *  USB Start of Frame Event Callback
 *   Called automatically on USB Start of Frame Event
 */

#if USB_SOF_EVENT
void USB_SOF_Event (void) {
	xprintfD("USB SOF Event");
}
#endif


/*
 *  USB Error Event Callback
 *   Called automatically on USB Error Event
 *    Parameter:       error: Error Code
 */

#if USB_ERROR_EVENT
void USB_Error_Event (uint32_t error) {
//	error &= 0xFF;
//	char b[8];
//
//	if(error & _BV(0))sprintf (b, "PID_ERR");//PID encoding error or Unknown PID or Token CRC.
//	if(error & _BV(1))return;//sprintf (b, "UEPKT");//Unexpected Packet - any packet sequence violation from the specification.
//	if(error & _BV(2))sprintf (b, "DCRC");//Data CRC error.
//	if(error & _BV(3))sprintf (b, "TIMEOUT");//Time out error.
//	if(error & _BV(4))sprintf (b, "EOP");//End of packet error.
//	if(error & _BV(5))sprintf (b, "B_OVRN");//Buffer Overrun.
//	if(error & _BV(6))sprintf (b, "BTSTF");//Bit stuff error.
//	if(error & _BV(7))sprintf (b, "TGL_ERR");//Wrong toggle bit in data PID, ignored data.
//
//	xprintfD("USB Error Event: %s", b);
}
#endif


/*
 *  USB Set Configuration Event Callback
 *   Called automatically on USB Set Configuration Request
 */

#if USB_CONFIGURE_EVENT
void USB_Configure_Event (void) {
  if (USB_Configuration) {                  /* Check if USB is configured */
  	USB_Connected = 1;
  	xTaskResumeFromISR(tasks.usb);

  	extern const uint8_t USB_CDC_DeviceDescriptor[];
  	if(USB_DeviceDescriptor == USB_CDC_DeviceDescriptor){
  		CDC_NotificationIn();
  	}

  	xprintfD("USB Connected");
  }else{
  	xprintfD("USB failed to connect");
  }
}
#endif


/*
 *  USB Set Interface Event Callback
 *   Called automatically on USB Set Interface Request
 */

#if USB_INTERFACE_EVENT
void USB_Interface_Event (void) {
	xprintfD("USB Interface Event");
}
#endif


/*
 *  USB Set/Clear Feature Event Callback
 *   Called automatically on USB Set/Clear Feature Request
 */

#if USB_FEATURE_EVENT
void USB_Feature_Event (void) {
	xprintfD("USB Feature Event");
}
#endif


#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)

/* USB Endpoint Events Callback Pointers */
void (* const USB_P_EP[16]) (uint32_t event) = {
  P_EP(0),
  P_EP(1),
  P_EP(2),
  P_EP(3),
  P_EP(4),
  P_EP(5),
  P_EP(6),
  P_EP(7),
  P_EP(8),
  P_EP(9),
  P_EP(10),
  P_EP(11),
  P_EP(12),
  P_EP(13),
  P_EP(14),
  P_EP(15),
};


/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */

void USB_EndPoint1 (uint32_t event) {
  uint16_t temp;
  static uint16_t serialState;
//  xprintfD("USB_EndPoint1: %lu", event);

  switch (event) {
    case USB_EVT_IN:
      temp = CDC_GetSerialState();
      if (serialState != temp) {
         serialState = temp;
         CDC_NotificationIn();            /* send SERIAL_STATE notification */
      }
      break;
  }
}


/*
 *  USB Endpoint 2 Event Callback
 *   Called automatically on USB Endpoint 2 Event
 *    Parameter:       event
 */

void USB_EndPoint2 (uint32_t event) {
//	xprintfD("USB_EndPoint2: %lu", event);
  switch (event) {
    case USB_EVT_OUT:
      CDC_BulkOut ();                /* data received from Host */
      break;
    case USB_EVT_IN:
      CDC_BulkIn ();                 /* data expected by Host */
      break;
  }
}


/*
 *  USB Endpoint 3 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */

void USB_EndPoint3 (uint32_t event) {
	debug("USB_EndPoint3: %lu", event);
}


/*
 *  USB Endpoint 4 Event Callback
 *   Called automatically on USB Endpoint 4 Event
 *    Parameter:       event
 */

void USB_EndPoint4 (uint32_t event) {
	debug("USB_EndPoint4: %lu", event);
}


/*
 *  USB Endpoint 5 Event Callback
 *   Called automatically on USB Endpoint 5 Event
 *    Parameter:       event
 */

void USB_EndPoint5 (uint32_t event) {
//	xprintfD("USB_EndPoint5: %lu", event);
#if USB_MSC
	switch (event) {
	  case USB_EVT_OUT:
	    MSC_BulkOut();
	    break;
	  case USB_EVT_IN:
	    MSC_BulkIn();
	    break;
  }
#endif
}


/*
 *  USB Endpoint 6 Event Callback
 *   Called automatically on USB Endpoint 6 Event
 *    Parameter:       event
 */

void USB_EndPoint6 (uint32_t event) {
	debug("USB_EndPoint6: %lu", event);
}


/*
 *  USB Endpoint 7 Event Callback
 *   Called automatically on USB Endpoint 7 Event
 *    Parameter:       event
 */

void USB_EndPoint7 (uint32_t event) {
	debug("USB_EndPoint7: %lu", event);
}


/*
 *  USB Endpoint 8 Event Callback
 *   Called automatically on USB Endpoint 8 Event
 *    Parameter:       event
 */

void USB_EndPoint8 (uint32_t event) {
	debug("USB_EndPoint8: %lu", event);
}


/*
 *  USB Endpoint 9 Event Callback
 *   Called automatically on USB Endpoint 9 Event
 *    Parameter:       event
 */

void USB_EndPoint9 (uint32_t event) {
	debug("USB_EndPoint9: %lu", event);
}


/*
 *  USB Endpoint 10 Event Callback
 *   Called automatically on USB Endpoint 10 Event
 *    Parameter:       event
 */

void USB_EndPoint10 (uint32_t event) {
	debug("USB_EndPoint10: %lu", event);
}


/*
 *  USB Endpoint 11 Event Callback
 *   Called automatically on USB Endpoint 11 Event
 *    Parameter:       event
 */

void USB_EndPoint11 (uint32_t event) {
	debug("USB_EndPoint11: %lu", event);
}


/*
 *  USB Endpoint 12 Event Callback
 *   Called automatically on USB Endpoint 12 Event
 *    Parameter:       event
 */

void USB_EndPoint12 (uint32_t event) {
	debug("USB_EndPoint12: %lu", event);
}


/*
 *  USB Endpoint 13 Event Callback
 *   Called automatically on USB Endpoint 13 Event
 *    Parameter:       event
 */

void USB_EndPoint13 (uint32_t event) {
	debug("USB_EndPoint13: %lu", event);
}


/*
 *  USB Endpoint 14 Event Callback
 *   Called automatically on USB Endpoint 14 Event
 *    Parameter:       event
 */

void USB_EndPoint14 (uint32_t event) {
	debug("USB_EndPoint14: %lu", event);
}


/*
 *  USB Endpoint 15 Event Callback
 *   Called automatically on USB Endpoint 15 Event
 *    Parameter:       event
 */

void USB_EndPoint15 (uint32_t event) {
	debug("USB_EndPoint15: %lu", event);
}
