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
 *  Main module for the Bluetooth stack. This module contains the overall Bluetooth
 *  stack state variables and the main Bluetooth stack management functions.
 */

#include "BluetoothStack.h"

/** Bluetooth device connection information structure. Once connected to a remote device, this structure tracks the
 *  connection state of the individual L2CAP channels.
 */
Bluetooth_Connection_t Bluetooth_Connection = { IsConnected: false };

/** Bluetooth device state information structure. This structure contains details on the current Bluetooth stack
 *  state.
 */
Bluetooth_Stack_State_t Bluetooth_State     = { IsInitialized: false };

/** Bluetooth stack initialization function. This function must be called once to initialize the Bluetooth stack,
 *  ready for connection to remote devices.
 *
 *  \note This function only begins the initialization process; the stack is initialized as the main Bluetooth stack
 *        management task is repeatedly called. The initialization process ends when the IsInitialized element of the
 *        \ref Bluetooth_State structure becomes true and the \ref Bluetooth_StackInitialized() callback fires.
 */
void Bluetooth_Stack_Init(void)
{
	/* Reset the HCI state machine - this will reset the adapter and stack when the Bluetooth stack task is called */
	Bluetooth_State.CurrentHCIState = Bluetooth_Init;
	Bluetooth_State.NextHCIState    = Bluetooth_Init;
}

/** Bluetooth stack management task. This task must be repeatedly called to maintain the Bluetooth stack and any connection
 *  to remote Bluetooth devices, including both the HCI control layer and the ACL channel layer.
 */
void Bluetooth_Stack_USBTask(void)
{
	if (USB_HostState != HOST_STATE_Configured)
	  return;

	Bluetooth_HCITask();
	Bluetooth_ACLTask();
}

