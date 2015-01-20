//*****************************************************************************
//
// uart_out.c - Example for writing data to the UART in an interrupt driven
//              fashion.
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
#include "../../src/uart.h"
#include "../osram96x16.h"

//*****************************************************************************
//
//! \addtogroup ev_lm3s811_list
//! <h1>UART (uart_out)</h1>
//!
//! This example application utilizes the UART to display text.  The first UART
//! (connected to the FTDI virtual serial port on the Stellaris LM3S811
//! Evaluation Board) will be configured in 115,200 baud, 8-n-1 mode and
//! continuously display text.  The text is transferred by servicing the
//! interrupt from the UART; since it takes about 1 ms to drain half of the
//! UART FIFO (causing an interrupt), this leaves plenty of time for other
//! processing to occur during the transfer (though that time is not utilized
//! by this example).
//
//*****************************************************************************

//*****************************************************************************
//
// The string that is to be written to the UART.  This happens to be the
// preamble to the Magna Carta.
//
//*****************************************************************************
static const unsigned char g_pucString[] =
"John, by the grace of God, king of England, lord of Ireland, duke of\r\n"
"Normandy and Aquitaine, and count of Anjou, to the archbishop, bishops,\r\n"
"abbots, earls, barons, justiciaries, foresters, sheriffs, stewards,\r\n"
"servants, and to all his bailiffs and liege subjects, greetings. Know\r\n"
"that, having regard to God and for the salvation of our soul, and those\r\n"
"of all our ancestors and heirs, and unto the honor of God and the\r\n"
"advancement of his holy Church and for the rectifying of our realm, we\r\n"
"have granted as underwritten by advice of our venerable fathers, Stephen,\r\n"
"archbishop of Canterbury, primate of all England and cardinal of the holy\r\n"
"Roman Church, Henry, archbishop of Dublin, William of London, Peter of\r\n"
"Winchester, Jocelyn of Bath and Glastonbury, Hugh of Lincoln, Walter of\r\n"
"Worcester, William of Coventry, Benedict of Rochester, bishops; of Master\r\n"
"Pandulf, subdeacon and member of the household of our lord the Pope, of\r\n"
"brother Aymeric (master of the Knights of the Temple in England), and of\r\n"
"the illustrious men William Marshal, earl of Pembroke, William, earl of\r\n"
"Salisbury, William, earl of Warenne, William, earl of Arundel, Alan of\r\n"
"Galloway (constable of Scotland), Waren Fitz Gerold, Peter Fitz Herbert,\r\n"
"Hubert De Burgh (seneschal of Poitou), Hugh de Neville, Matthew Fitz\r\n"
"Herbert, Thomas Basset, Alan Basset, Philip d'Aubigny, Robert of\r\n"
"Roppesley, John Marshal, John Fitz Hugh, and others, our liegemen.\r\n\r\n";

//*****************************************************************************
//
// The variables that track the data to be transmitted.
//
//*****************************************************************************
static volatile const unsigned char *g_pucBuffer = 0;
static volatile unsigned long g_ulCount = 0;

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
// The transmit interrupt handler for the UART.
//
//*****************************************************************************
// mthomas: added static
static void
UARTTxIntHandler(void)
{
    //
    // Loop while there are more characters to send and space available in the
    // hardware FIFO.
    //
    while(g_ulCount && UARTSpaceAvail(UART0_BASE))
    {
        //
        // Send the next character.
        //
        UARTCharNonBlockingPut(UART0_BASE, *g_pucBuffer++);

        //
        // Decrement the count of characters to send.
        //
        g_ulCount--;
    }
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
// mthomas: attribute for stack-aligment (see README_mthomas.txt)
#ifdef __GNUC__
void UARTIntHandler(void) __attribute__((__interrupt__));
#endif
void
UARTIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // See if the transmit interrupt was asserted.
    //
    if(ulStatus & UART_INT_TX)
    {
        //
        // Handle the transmit interrupt.
        //
        UARTTxIntHandler();
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
// mthomas: added static
static void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Wait until any previous string has been sent.
    //
    while(g_ulCount)
    {
    }

    //
    // Save the data buffer to be transmitted.
    //
    g_pucBuffer = pucBuffer;
    g_ulCount = ulCount;

    //
    // Prime the UART FIFO.  This is done with interrupts disabled to avoid
    // problems if a transmit interrupt occurs mid way through the priming.
    //
    UARTIntDisable(UART0_BASE, UART_INT_TX);
    UARTTxIntHandler();
    UARTIntEnable(UART0_BASE, UART_INT_TX);
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
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
    OSRAMStringDraw("UART out on SER0", 0, 0);
    OSRAMStringDraw("115,200, 8-N-1", 6, 1);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSet(UART0_BASE, 115200, (UART_CONFIG_WLEN_8 |
                                       UART_CONFIG_STOP_ONE |
                                       UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_TX);

    //
    // Loop forever sending data out the UART.
    //
    while(1)
    {
        //
        // Send the string to the UART.
        //
        UARTSend(g_pucString, sizeof(g_pucString) - 1);
    }
}
