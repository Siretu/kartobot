/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Changed string descriptor handling
 *          V1.00 Initial Version
 *---------------------------------------------------------------------------*/
#include "lpc_types.h"
#include "usb.h"
#include "cdc.h"
#include "msc.h"
#include "usbcfg.h"
#include "usbdesc.h"

extern const uint8_t USB_CDC_DeviceDescriptor[];
extern const uint8_t USB_CDC_ConfigDescriptor[];
extern const uint8_t USB_CDC_StringDescriptor[];

/* USB Standard Device Descriptor */
const uint8_t *USB_DeviceDescriptor = USB_CDC_DeviceDescriptor;

/* USB Configuration Descriptor */
const uint8_t *USB_ConfigDescriptor = USB_CDC_ConfigDescriptor;

/* USB String Descriptor (optional) */
const uint8_t *USB_StringDescriptor = USB_CDC_StringDescriptor;
