//*****************************************************************************
//
// bitband.c - Bit-band manipulation example.
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
// This is part of revision 920 of the Stellaris Driver Library.
//
//*****************************************************************************

#include "../../hw_types.h"
#include "../../src/debug.h"
#include "../../src/sysctl.h"
#include "../../src/systick.h"
#include "../../utils/diag.h"
#include "../osram96x16.h"

//*****************************************************************************
//
//! \addtogroup ev_lm3s811_list
//! <h1>Bit-Banding (bitband)</h1>
//!
//! This example application demonstrates the use of the bit-banding
//! capabilities of the Cortex-M3 microprocessor.  All of SRAM and all of the
//! peripherals reside within bit-band regions, meaning that bit-banding
//! operations can be applied to any of them.  In this example, a variable in
//! SRAM is set to a particular value one bit at a time using bit-banding
//! operations (it would be more efficient to do a single non-bit-banded write;
//! this simply demonstrates the operation of bit-banding).
//
//*****************************************************************************

//*****************************************************************************
//
// A map of hex nibbles to ASCII characters.
//
//*****************************************************************************
static const char *pcHex = "0123456789ABCDEF";

//*****************************************************************************
//
// The value that is to be modified via bit-banding.
//
//*****************************************************************************
static volatile unsigned long g_ulValue;

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
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, along with the remainder of the current second).
//
//*****************************************************************************
// mthomas: added static
static void
Delay(unsigned long ulSeconds)
{
    //
    // Loop while there are more seconds to wait.
    //
    while(ulSeconds--)
    {
        //
        // Wait until the SysTick value is less than 1000.
        //
        while(SysTickValueGet() > 1000)
        {
        }

        //
        // Wait until the SysTick value is greater than 1000.
        //
        while(SysTickValueGet() < 1000)
        {
        }
    }
}

//*****************************************************************************
//
// Print the given value as a hexadecimal string on the LCD.
//
//*****************************************************************************
// mthomas: added static
static void
PrintValue(unsigned long ulValue)
{
    char pcBuffer[9];

    pcBuffer[0] = pcHex[(ulValue >> 28) & 15];
    pcBuffer[1] = pcHex[(ulValue >> 24) & 15];
    pcBuffer[2] = pcHex[(ulValue >> 20) & 15];
    pcBuffer[3] = pcHex[(ulValue >> 16) & 15];
    pcBuffer[4] = pcHex[(ulValue >> 12) & 15];
    pcBuffer[5] = pcHex[(ulValue >> 8) & 15];
    pcBuffer[6] = pcHex[(ulValue >> 4) & 15];
    pcBuffer[7] = pcHex[(ulValue >> 0) & 15];
    pcBuffer[8] = '\0';
    OSRAMStringDraw(pcBuffer, 24, 1);
}

//*****************************************************************************
//
// This example demonstrates the use of bit-banding to set individual bits
// within a word of SRAM.
//
//*****************************************************************************
int
main(void)
{
    unsigned long ulErrors, ulIdx;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_6MHZ);

    //
    // Initialize the OLED display and write status.
    //
    OSRAMInit(false);
    OSRAMStringDraw("Bit banding...", 6, 0);

    //
    // Set up and enable the SysTick timer.  It will be used as a reference
    // for delay loops.  The SysTick timer period will be set up for one
    // second.
    //
    SysTickPeriodSet(SysCtlClockGet());
    SysTickEnable();

    //
    // Set the value and error count to zero.
    //
    g_ulValue = 0;
    ulErrors = 0;

    //
    // Print the initial value to the LCD.
    //
    PrintValue(g_ulValue);

    //
    // Delay for 1 second.
    //
    Delay(1);

    //
    // Set the value to 0xdecafbad using bit band accesses to each individual
    // bit.
    //
    for(ulIdx = 0; ulIdx < 32; ulIdx++)
    {
        //
        // Set this bit.
        //
        HWREGBITW(&g_ulValue, 31 - ulIdx) = (0xdecafbad >> (31 - ulIdx)) & 1;

        //
        // Print the current value to the LCD.
        //
        PrintValue(g_ulValue);

        //
        // Delay for 1 second.
        //
        Delay(1);
    }

    //
    // Make sure that the value is 0xdecafbad.
    //
    if(g_ulValue != 0xdecafbad)
    {
        ulErrors++;
    }

    //
    // Make sure that the individual bits read back correctly.
    //
    for(ulIdx = 0; ulIdx < 32; ulIdx++)
    {
        if(HWREGBITW(&g_ulValue, ulIdx) != ((0xdecafbad >> ulIdx) & 1))
        {
            ulErrors++;
        }
    }

    //
    // Delay for 2 seconds.
    //
    Delay(2);

    //
    // Print out the result.
    //
    if(ulErrors)
    {
        OSRAMStringDraw("Errors!    ", 6, 1);
    }
    else
    {
        OSRAMStringDraw("Success.   ", 6, 1);
    }

    //
    // Exit.
    //
    DiagExit(0);
}
