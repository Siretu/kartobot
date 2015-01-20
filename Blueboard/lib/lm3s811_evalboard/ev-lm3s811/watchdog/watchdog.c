//*****************************************************************************
//
// watchdog.c - Watchdog timer example.
//
// Copyright (c) 2005,2006 Luminary Micro, Inc.  All rights reserved.
//
// Software License Agreement
//
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's Stellaris Family of microcontroller products.
//
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  Any use in violation
// of the foregoing restrictions may subject the user to criminal sanctions
// under applicable laws, as well as to civil liability for the breach of the
// terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 935 of the Stellaris Driver Library.
//
//*****************************************************************************

#include "../../hw_ints.h"
#include "../../hw_memmap.h"
#include "../../hw_types.h"
#include "../../src/debug.h"
#include "../../src/gpio.h"
#include "../../src/interrupt.h"
#include "../../src/sysctl.h"
#include "../../src/watchdog.h"
#include "../osram96x16.h"

//*****************************************************************************
//
//! \addtogroup ev_lm3s811_list
//! <h1>Watchdog (watchdog)</h1>
//!
//! This example application demonstrates the use of the watchdog as a simple
//! heartbeat for the system.  If the watchdog is not periodically fed, it will
//! reset the system.  Each time the watchdog is fed, the LED connected to port
//! C5 is inverted so that it is easy to see that it is being fed, which occurs
//! once every second.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The interrupt handler for the watchdog.  This feeds the dog (so that the
// processor does not get reset) and winks the LED connected to GPIO B0.
//
//*****************************************************************************
// mthomas: attribute for stack-aligment (see README_mthomas.txt)
#ifdef __GNUC__
void WatchdogIntHandler(void) __attribute__((__interrupt__)); 
#endif
void
WatchdogIntHandler(void)
{
    //
    // Clear the watchdog interrupt.
    //
    WatchdogIntClear(WATCHDOG_BASE);

    //
    // Invert the GPIO B0 value.
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5,
                 GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5) ^ GPIO_PIN_5);
}

//*****************************************************************************
//
// This example demonstrates the use of the watchdog timer.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_6MHZ);

    //
    // Initialize the OLED display and write status.
    //
    OSRAMInit(false);
    OSRAMStringDraw("Watchdog example", 0, 0);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO C5 as an output.  This drives an LED on the board that will
    // toggle when a watchdog interrupt is processed.
    //
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT);

    //
    // Enable the watchdog interrupt.
    //
    IntEnable(INT_WATCHDOG);

    //
    // Set the period of the watchdog timer.
    //
    WatchdogReloadSet(WATCHDOG_BASE, SysCtlClockGet());

    //
    // Enable reset generation from the watchdog timer.
    //
    WatchdogResetEnable(WATCHDOG_BASE);

    //
    // Enable the watchdog timer.
    //
    WatchdogEnable(WATCHDOG_BASE);

    //
    // Loop forever while the LED winks as watchdog interrupts are handled.
    //
    while(1)
    {
    }
}
