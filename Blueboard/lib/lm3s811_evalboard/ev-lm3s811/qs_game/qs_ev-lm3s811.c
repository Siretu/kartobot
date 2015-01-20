//*****************************************************************************
//
// qs_ev-lm3s811.c - The quick start application for the LM3S811 Evaluation
//                   Board.
//
// Copyright (c) 2006 Luminary Micro, Inc.  All rights reserved.
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
#include "../../src/adc.h"
#include "../../src/debug.h"
#include "../../src/gpio.h"
#include "../../src/interrupt.h"
#include "../../src/sysctl.h"
#include "../../src/timer.h"
#include "../../src/uart.h"
#include "../osram96x16.h"
#include "game.h"
#include "globals.h"
#include "random.h"
#include "screen_saver.h"

//*****************************************************************************
//
//! \addtogroup ev_lm3s811_list
//! <h1>EV-LM3S811 Quickstart Application (qs_ev-lm3s811)</h1>
//!
//! A game in which a ship is navigated through an endless tunnel.  The
//! potentiometer is used to move the ship up and down, and the user push
//! button is used to fire a missile to destroy obstacles in the tunnel.  Score
//! accumulates for survival and for destroying obstacles.  The game lasts for
//! only one ship; the score is displayed on the virtual UART at 115,200, 8-N-1
//! during game play and will be displayed on the screen at the end of the
//! game.
//!
//! Since the OLED display on the evaluation board has burn-in characteristics
//! similar to a CRT, the application also contains a screen saver.  The screen
//! saver will only become active if two minutes have passed without the user
//! push button being pressed while waiting to start the game (i.e. it will
//! never come on during game play).  An implementation of the Game of Life is
//! run with a field of random data as the seed value.
//!
//! After two minutes of running the screen saver, the display will be turned
//! off and the user LED will blink.  Either mode of screen saver (Game of Life
//! or blank display) will be exited by pressing the user push button.  The
//! button will then need to be pressed again to start the game.
//
//*****************************************************************************

//*****************************************************************************
//
// A bitmap for the Luminary Micro logo.  The bitmap is as follows:
//
//     xx...xxxxxxx.........
//     xx...xxxxxxxxx.......
//     xx...xx...xxxxx......
//     xx...xx...xx..xx.....
//     xx...xx...xx..xx.....
//     xx...xx...xx...xx....
//     xx...xx...xx...xx....
//     xx...xx...xx...xx....
//     xx...xx...xx...xx....
//     xx...xx...xx...xx....
//     xx....x...xx...xx....
//     .xx...x...xx...xx....
//     .xx............xx....
//     ..xxx..........xx....
//     ...xxxxxxxxxxxxxx....
//     .....xxxxxxxxxxxx....
//
// Continued...
//
//     xx......xx..xx..xx.....xx..xx..xx...xx...xxxx...xxxxx...xx..xx
//     xx......xx..xx..xxx...xxx..xx..xxx..xx..xxxxxx..xxxxxx..xx..xx
//     xx......xx..xx..xxxx.xxxx..xx..xxxx.xx..xx..xx..xx..xx..xx..xx
//     xx......xx..xx..xx.xxx.xx..xx..xx.xxxx..xxxxxx..xxxxx....xxxx.
//     xx......xx..xx..xx..x..xx..xx..xx..xxx..xxxxxx..xxxxx.....xx..
//     xxxxxx..xxxxxx..xx.....xx..xx..xx...xx..xx..xx..xx..xx....xx..
//     xxxxxx...xxxx...xx.....xx..xx..xx...xx..xx..xx..xx..xx....xx..
//     ..............................................................
//     x.....x..x...xxxx...xxxxx....xxxx.............................
//     xx...xx..x..x....x..x....x..x....x............................
//     x.x.x.x..x..x.......x....x..x....x............................
//     x..x..x..x..x.......xxxxx...x....x............................
//     x.....x..x..x.......x....x..x....x............................
//     x.....x..x..x....x..x....x..x....x............................
//     x.....x..x...xxxx...x....x...xxxx.............................
//     ..............................................................
//
//*****************************************************************************
static const unsigned char g_pucLMILogo[166] =
{
    0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x03, 0x03, 0x03, 0xff, 0xff,
    0x06, 0x06, 0x1c, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x60,
    0x60, 0x60, 0x60, 0x00, 0x00, 0x3f, 0x7f, 0x60, 0x60, 0x7f, 0x3f, 0x00,
    0x00, 0x7f, 0x7f, 0x06, 0x0c, 0x18, 0x0c, 0x06, 0x7f, 0x7f, 0x00, 0x00,
    0x7f, 0x7f, 0x00, 0x00, 0x7f, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x7f, 0x00,
    0x00, 0x7e, 0x7f, 0x1b, 0x1b, 0x7f, 0x7e, 0x00, 0x00, 0x7f, 0x7f, 0x1b,
    0x1b, 0x7f, 0x76, 0x00, 0x00, 0x07, 0x0f, 0x78, 0x78, 0x0f, 0x07,
    0x07, 0x1f, 0x38, 0x60, 0x60, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xcf, 0xcf,
    0xc0, 0xc0, 0xc0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
    0x02, 0x04, 0x08, 0x04, 0x02, 0x7f, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x3e,
    0x41, 0x41, 0x41, 0x22, 0x00, 0x00, 0x7f, 0x09, 0x09, 0x09, 0x09, 0x76,
    0x00, 0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//*****************************************************************************
//
// A bitmap for the Keil/ARM logo.  The bitmap is as follows:
//
//     x..xxxxxxxxxxxxxxx...
//     xx..x............x...
//     x.x..x...........x...
//     x..x..x..........x...
//     x...x..x.........x...
//     x....x..x........x...
//     x.....x..x.......x...
//     x......x..x......x...
//     x.....x..x.......x...
//     x....x..x........x...
//     x...x..x.........x...
//     x..x..x..........x...
//     x.x..x...........x...
//     xx..x............x...
//     x..xxxxxxxxxxxxxxx...
//     .....................
//
// Continued...
//
//     xxx.......xxxx....xxxxxxxxxxxxx....xxx....xxx..........
//     xxx.....xxxx......xxxxxxxxxxxxx....xxx....xxx..........
//     xxx...xxxx........xxx..............xxx....xxx..........
//     xxxxxxxx..........xxxxxxxxxxxxx....xxx....xxx..........
//     xxxxxxxx..........xxxxxxxxxxxxx....xxx....xxx..........
//     xxx...xxxx........xxx..............xxx....xxx..........
//     xxx.....xxxx......xxxxxxxxxxxxx....xxx....xxxxxxxxxxxxx
//     xxx.......xxxx....xxxxxxxxxxxxx....xxx....xxxxxxxxxxxxx
//     .......................................................
//     .......................................................
//     .x.........x..xx..x...x....xx..........................
//     x.x.......x.x.x.x.xx.xx...x............................
//     xxx.xx....xxx.xx..x.x.x...x....x..xx.x..xx...xx.xx..x.x
//     x.x.x.x...x.x.x.x.x...x...x...x.x.x.x.x.x.x.x.x.x.x.x.x
//     x.x.x.x...x.x.x.x.x...x....xx..x..x.x.x.xx...xx.x.x..x.
//     ........................................x...........x..
//
//*****************************************************************************
static const unsigned char g_pucKeilLogo[152] =
{
    0xff, 0x02, 0x04, 0x09, 0x13, 0x25, 0x49, 0x91, 0x21, 0x41, 0x81, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
    0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x81, 0x81, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb,
    0xdb, 0xdb, 0xdb, 0xdb, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,
    0x7f, 0x20, 0x10, 0x48, 0x64, 0x52, 0x49, 0x44, 0x42, 0x41, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x7f, 0x00, 0x00, 0x00, 0x78, 0x14, 0x78,
    0x00, 0x70, 0x10, 0x60, 0x00, 0x00, 0x00, 0x78, 0x14, 0x78, 0x00, 0x7c,
    0x14, 0x68, 0x00, 0x7c, 0x08, 0x10, 0x08, 0x7c, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x44, 0x00, 0x20, 0x50, 0x20, 0x00, 0x70, 0x10, 0x60, 0x10, 0x60,
    0x00, 0xf0, 0x50, 0x20, 0x00, 0x20, 0x50, 0x70, 0x00, 0x70, 0x10, 0x60,
    0x00, 0xb0, 0x40, 0x30
};

//*****************************************************************************
//
// A bitmap for the design context URL.  The bitmap is as follows:
//
//     ........................................................................
//     ........................................................................
//     ....................................xx.......................xx.........
//     ....................................xx.......................xx.........
//     ....................................xx..................................
//     xx......xx.xx......xx.xx......xx....xx......xx...xx.xxxx.xx..xx.xxxxxx..
//     xx..xx..xx.xx..xx..xx.xx..xx..xx....xx......xx...xx.xxxxxxxx.xx.xxxxxxx.
//     xx..xx..xx.xx..xx..xx.xx..xx..xx....xx......xx...xx.xx.xx.xx.xx.xx...xx.
//     xx..xx..xx.xx..xx..xx.xx..xx..xx....xx......xx...xx.xx.xx.xx.xx.xx...xx.
//     xx..xx..xx.xx..xx..xx.xx..xx..xx....xx......xx...xx.xx.xx.xx.xx.xx...xx.
//     .xxxxxxxx...xxxxxxxx...xxxxxxxx.....xx......xx...xx.xx.xx.xx.xx.xx...xx.
//     .xxxxxxxx...xxxxxxxx...xxxxxxxx..xx.xxxxxxx.xxxxxxx.xx.xx.xx.xx.xx...xx.
//     ..xx..xx.....xx..xx.....xx..xx...xx.xxxxxxx..xxxxx..xx.xx.xx.xx.xx...xx.
//     ........................................................................
//     ........................................................................
//     ........................................................................
//
// Continued...
//
//     ........................................................................
//     ........................................................................
//     ........................xx.......xx.xx..................................
//     ........................xxx.....xxx.xx..................................
//     ........................xxxx...xxxx.....................................
//     .xxxxx..xx.xxx..xx...xx.xx.xx.xx.xx.xx..xxxxx..xx.xxx...xxxxx......xxxxx
//     xxxxxxx.xxxxxxx.xx...xx.xx..xxx..xx.xx.xxxxxxx.xxxxxxx.xxxxxxx....xxxxxx
//     xx...xx.xxx..xx.xx...xx.xx...x...xx.xx.xx......xxx..xx.xx...xx....xx....
//     xx...xx.xx......xx...xx.xx.......xx.xx.xx......xx......xx...xx....xx....
//     xx...xx.xx......xx...xx.xx.......xx.xx.xx......xx......xx...xx....xx....
//     xx...xx.xx......xx...xx.xx.......xx.xx.xx......xx......xx...xx....xx....
//     xxxxxxx.xx......xxxxxxx.xx.......xx.xx.xxxxxxx.xx......xxxxxxx.xx.xxxxxx
//     .xxx.xx.xx.......xxx.xx.xx.......xx.xx..xxxxx..xx.......xxxxx..xx..xxxxx
//     .....................xx.................................................
//     ................xxxxxxx.................................................
//     .................xxxxx..................................................
//
// Continued...
//
//     ........................................................................
//     ........................................................................
//     .......................xx.xxxxxx..................xx..................xx
//     .......................xx.xxxxxxx.................xx.................xxx
//     ......................xx..xx...xx....................................xx.
//     ...xxxxx..xxxx.xx.....xx..xx...xx..xxxxx...xxxxx..xx..xxxxx..xxxxxx..xx.
//     x.xxxxxxx.xxxxxxxx...xx...xx...xx.xxxxxxx.xxxxxxx.xx.xxxxxxx.xxxxxxx.xxx
//     ..xx...xx.xx.xx.xx...xx...xx...xx.xx...xx.xx......xx.xx...xx.xx...xx..xx
//     ..xx...xx.xx.xx.xx...xx...xx...xx.xxxxxxx.xxxxxx..xx.xx...xx.xx...xx....
//     ..xx...xx.xx.xx.xx..xx....xx...xx.xxxxxxx..xxxxxx.xx.xx...xx.xx...xx....
//     ..xx...xx.xx.xx.xx..xx....xx...xx.xx...........xx.xx.xx...xx.xx...xx....
//     x.xxxxxxx.xx.xx.xx.xx.....xxxxxxx.xxxxxxx.xxxxxxx.xx.xxxxxxx.xx...xx.xxx
//     ...xxxxx..xx.xx.xx.xx.....xxxxxx...xxxxx...xxxxx..xx..xxx.xx.xx...xx..xx
//     ..........................................................xx............
//     .....................................................xxxxxxx............
//     ......................................................xxxxx.............
//
// Continued...
//
//     ........................................................................
//     ........................................................................
//     xxx.................xx.xx.................xx..........xxxxx...xxxxx...xx
//     xxxx...xx...........xx.xx.................xx.........xxxxxxx.xxxxxxx.xxx
//     .......xx...........xx.xx............................xx...xx.xx...xx.xx.
//     .....xxxxxx..xxxxx..xx.xx..xxxxx..xx.xxx..xx..xxxxx.......xx.xx..xxx.xx.
//     xxx..xxxxxx.xxxxxxx.xx.xx.xxxxxxx.xxxxxxx.xx.xxxxxxx.....xx..xx.xxxx.xx.
//     xxxx...xx...xx...xx.xx.xx.xx...xx.xxx..xx.xx.xx.........xx...xxxx.xx.xxx
//     ..xx...xx...xxxxxxx.xx.xx.xx...xx.xx......xx.xxxxxx....xx....xxx..xx.xxx
//     ..xx...xx...xxxxxxx.xx.xx.xx...xx.xx......xx..xxxxxx..xx.....xx...xx.xx.
//     ..xx...xx...xx......xx.xx.xx...xx.xx......xx......xx.xx......xx...xx.xx.
//     xxxx...xxxx.xxxxxxx.xx.xx.xxxxxxx.xx......xx.xxxxxxx.xxxxxxx.xxxxxxx.xxx
//     xxx.....xx...xxxxx..xx.xx..xxx.xx.xx......xx..xxxxx..xxxxxxx..xxxxx...xx
//     ........................................................................
//     ........................................................................
//     ........................................................................
//
// Continued...
//
//     ............
//     ............
//     xxx...xxxxx.
//     xxxx.xxxxxxx
//     ..xx.xx.....
//     .xxx.xx.....
//     xxxx.xxxxxx.
//     x.xx.xxxxxxx
//     ..xx.xx...xx
//     ..xx.xx...xx
//     ..xx.xx...xx
//     xxxx.xxxxxxx
//     xxx...xxxxx.
//     ............
//     ............
//     ............
//
//*****************************************************************************
static const unsigned char g_pucDesignLogo[600] =
{
    0xe0, 0xe0, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0xe0,
    0xe0, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0xe0, 0xe0,
    0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00,
    0x00, 0xe0, 0xe0, 0x00, 0xe0, 0xe0, 0x60, 0xe0, 0xc0, 0x60, 0xe0, 0xc0,
    0x00, 0xec, 0xec, 0x00, 0xe0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00,
    0xc0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xe0, 0xe0, 0xc0, 0x60,
    0x60, 0xe0, 0xc0, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00,
    0xfc, 0xfc, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0xfc, 0xfc, 0x00,
    0xec, 0xec, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0x60, 0x40, 0x00, 0xe0,
    0xe0, 0xc0, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60,
    0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0x60,
    0x40, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xe0, 0xe0,
    0x60, 0xe0, 0xc0, 0x60, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0x3c,
    0x0c, 0x00, 0xfc, 0xfc, 0x0c, 0x0c, 0x0c, 0xfc, 0xf8, 0x00, 0xc0, 0xe0,
    0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0x60,
    0x40, 0x00, 0xec, 0xec, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0,
    0x00, 0xe0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0x78, 0xfc, 0xcc,
    0xcc, 0xcc, 0xcc, 0x88, 0x00, 0x60, 0x60, 0xf8, 0xf8, 0x60, 0x60, 0x00,
    0xc0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xfc, 0xfc, 0x00, 0xfc,
    0xfc, 0x00, 0xc0, 0xe0, 0x60, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xe0, 0xe0,
    0xc0, 0x60, 0x60, 0xe0, 0xc0, 0x00, 0xec, 0xec, 0x00, 0xc0, 0xe0, 0x60,
    0x60, 0x60, 0x60, 0x40, 0x00, 0x18, 0x1c, 0x0c, 0x8c, 0xcc, 0x7c, 0x38,
    0x00, 0xf8, 0xfc, 0x8c, 0xcc, 0x6c, 0xfc, 0xf8, 0x00, 0xf8, 0xfc, 0x8c,
    0xcc, 0x6c, 0xfc, 0xf8, 0x00, 0xf8, 0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0x88,
    0x03, 0x0f, 0x1c, 0x1c, 0x0f, 0x0f, 0x1c, 0x1c, 0x0f, 0x03, 0x00, 0x03,
    0x0f, 0x1c, 0x1c, 0x0f, 0x0f, 0x1c, 0x1c, 0x0f, 0x03, 0x00, 0x03, 0x0f,
    0x1c, 0x1c, 0x0f, 0x0f, 0x1c, 0x1c, 0x0f, 0x03, 0x00, 0x18, 0x18, 0x00,
    0x1f, 0x1f, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x0f, 0x1f, 0x18, 0x18,
    0x18, 0x1f, 0x0f, 0x00, 0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x00, 0x1f, 0x1f,
    0x00, 0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00,
    0x0f, 0x1f, 0x18, 0x18, 0x08, 0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x4f, 0xdf, 0xd8, 0xd8, 0xc8, 0xff, 0x7f, 0x00,
    0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00,
    0x1f, 0x1f, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x1f,
    0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18,
    0x1f, 0x0f, 0x00, 0x18, 0x18, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18, 0x18,
    0x08, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18, 0x1f, 0x0f, 0x00, 0x1f, 0x1f,
    0x00, 0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x00, 0x18, 0x1e, 0x07, 0x01, 0x00,
    0x00, 0x00, 0x1f, 0x1f, 0x18, 0x18, 0x18, 0x1f, 0x0f, 0x00, 0x0f, 0x1f,
    0x1b, 0x1b, 0x1b, 0x1b, 0x0b, 0x00, 0x09, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f,
    0x0e, 0x00, 0x1f, 0x1f, 0x00, 0x4f, 0xdf, 0xd8, 0xd8, 0xc8, 0xff, 0x7f,
    0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x08, 0x18, 0x18,
    0x18, 0x18, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x18, 0x08, 0x00,
    0x0f, 0x1f, 0x1b, 0x1b, 0x1b, 0x1b, 0x0b, 0x00, 0x1f, 0x1f, 0x00, 0x1f,
    0x1f, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x08, 0x1f, 0x1f, 0x00, 0x1f, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x09, 0x1b, 0x1b,
    0x1b, 0x1b, 0x1f, 0x0e, 0x00, 0x1c, 0x1e, 0x1b, 0x19, 0x18, 0x18, 0x18,
    0x00, 0x0f, 0x1f, 0x19, 0x18, 0x18, 0x1f, 0x0f, 0x00, 0x0f, 0x1f, 0x19,
    0x18, 0x18, 0x1f, 0x0f, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18, 0x1f, 0x0f
};

//*****************************************************************************
//
// A set of flags used to track the state of the application.
//
//*****************************************************************************
unsigned long g_ulFlags;

//*****************************************************************************
//
// The current filtered value of the potentiometer.
//
//*****************************************************************************
unsigned long g_ulWheel;

//*****************************************************************************
//
// Storage for a local frame buffer.
//
//*****************************************************************************
unsigned char g_pucFrame[192];

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
// The interrupt handler for the ADC interrupt.
//
//*****************************************************************************
// mthomas: attribute for stack-aligment (see README_mthomas.txt)
#ifdef __GNUC__
void ADCIntHandler(void) __attribute__((__interrupt__)); 
#endif
void
ADCIntHandler(void)
{
    unsigned long ulData;

    //
    // Clear the ADC interrupt.
    //
    ADCIntClear(ADC_BASE, 0);

    //
    // Read the data from the ADC.
    //
    ADCSequenceDataGet(ADC_BASE, 0, &ulData);

    //
    // Add the ADC data to the random number entropy pool.
    //
    RandomAddEntropy(ulData);

    //
    // Pass the ADC data through the low pass filter (with a coefficient of
    // 0.9) to update the position of the potentiometer.
    //
    g_ulWheel = ((g_ulWheel * 58982) + (ulData * 6554)) / 65536;

    //
    // Read the push button.
    //
    ulData = GPIOPinRead(GPIO_PORTC_BASE, PUSH_BUTTON) ? 1 : 0;

    //
    // See if the push button state doesn't match the debounced push button
    // state.
    //
    if(ulData != HWREGBITW(&g_ulFlags, FLAG_BUTTON))
    {
        //
        // Increment the debounce counter.
        //
        HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_LOW) ^= 1;
        if(!HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_LOW))
        {
            HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_HIGH) = 1;
        }

        //
        // See if the debounce counter has reached three.
        //
        if(HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_LOW) &&
           HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_HIGH))
        {
            //
            // The button has been in the new state for three consecutive
            // samples, so it has been debounced.  Toggle the debounced state
            // of the button.
            //
            HWREGBITW(&g_ulFlags, FLAG_BUTTON) ^= 1;

            //
            // If the button was just pressed, set the flag to indicate that
            // fact.
            //
            if(HWREGBITW(&g_ulFlags, FLAG_BUTTON) == 0)
            {
                HWREGBITW(&g_ulFlags, FLAG_BUTTON_PRESS) = 1;
            }
        }
    }
    else
    {
        //
        // Since the button state matches the debounced state, reset the
        // debounce counter.
        //
        HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_LOW) = 0;
        HWREGBITW(&g_ulFlags, FLAG_DEBOUNCE_HIGH) = 0;
    }

    //
    // Increment the clock count.
    //
    HWREGBITW(&g_ulFlags, FLAG_CLOCK_COUNT_LOW) ^= 1;
    if(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_COUNT_LOW))
    {
        HWREGBITW(&g_ulFlags, FLAG_CLOCK_COUNT_HIGH) ^= 1;
    }

    //
    // If the clock count has wrapped around to zero, then set a flag to
    // indicate that the display needs to be updated.
    //
    if((HWREGBITW(&g_ulFlags, FLAG_CLOCK_COUNT_LOW) == 0) &&
       (HWREGBITW(&g_ulFlags, FLAG_CLOCK_COUNT_HIGH) == 0))
    {
        HWREGBITW(&g_ulFlags, FLAG_UPDATE) = 1;
    }

    //
    // Indicate that a timer interrupt has occurred.
    //
    HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 1;
}

//*****************************************************************************
//
// Displays a logo for a specified amount of time.
//
//*****************************************************************************
static void
DisplayLogo(const unsigned char *pucLogo, unsigned long ulX,
            unsigned long ulWidth, unsigned long ulDelay)
{
    unsigned long ulLoop, ulBit;

    //
    // Loop over the sixteen scan lines of the display, scrolling the logo onto
    // the display from the bottom by one scan line at a time.
    //
    for(ulBit = 1; ulBit <= 16; ulBit++)
    {
        //
        // Clear the local frame buffer.
        //
        for(ulLoop = 0; ulLoop < 192; ulLoop += 4)
        {
            *(unsigned long *)(g_pucFrame + ulLoop) = 0;
        }

        //
        // See if eight scan lines or less should be displayed.
        //
        if(ulBit <= 8)
        {
            //
            // Loop over the columns of the logo.
            //
            for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
            {
                //
                // Copy the specified number of scan lines from the first row
                // of the logo to the second row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop + 96] = pucLogo[ulLoop] << (8 - ulBit);
            }
        }
        else
        {
            //
            // Loop over the columns of the logo.
            //
            for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
            {
                //
                // Copy N - 8 of the scan lines from the first row of the logo
                // to the first row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop] = pucLogo[ulLoop] << (16 - ulBit);

                //
                // Copy the remaining scan lines from the first row of the logo
                // and N - 8 scan lines from the second row of the logo to the
                // second row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop + 96] =
                    ((pucLogo[ulLoop] >> (ulBit - 8)) |
                     (pucLogo[ulLoop + ulWidth] << (16 - ulBit)));
            }
        }

        //
        // Display the local frame buffer on the display.
        //
        OSRAMImageDraw(g_pucFrame, 0, 0, 96, 2);

        //
        // Wait for a twentieth of a second.
        //
        for(ulLoop = 0; ulLoop < (CLOCK_RATE / 20); ulLoop++)
        {
            //
            // Wait until a SysTick interrupt has occurred.
            //
            while(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK))
            {
            }

            //
            // Clear the SysTick interrupt flag.
            //
            HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 0;
        }
    }

    //
    // Delay for the specified time while the logo is displayed.
    //
    for(ulLoop = 0; ulLoop < ulDelay; ulLoop++)
    {
        //
        // Wait until a SysTick interrupt has occurred.
        //
        while(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK))
        {
        }

        //
        // Clear the SysTick interrupt flag.
        //
        HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 0;
    }

    //
    // Loop over the sixteen scan lines of the display, scrolling the logo off
    // the display to the top by one scan line at a time.
    //
    for(ulBit = 1; ulBit <= 16; ulBit++)
    {
        //
        // Clear the local frame buffer.
        //
        for(ulLoop = 0; ulLoop < 192; ulLoop += 4)
        {
            *(unsigned long *)(g_pucFrame + ulLoop) = 0;
        }

        //
        // See if more than eight scan lines should be displayed.
        //
        if(ulBit <= 8)
        {
            //
            // Loop over the columns of the logo.
            //
            for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
            {
                //
                // Copy 8 - N scan lines from the first row of the logo and
                // N scan lines from the second row of the logo to the first
                // row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop] =
                    ((pucLogo[ulLoop] >> ulBit) |
                     (pucLogo[ulLoop + ulWidth] << (8 - ulBit)));

                //
                // Copy 8 - N scan lines from the second row of the logo to the
                // second row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop + 96] = (pucLogo[ulLoop + ulWidth] >>
                                                 ulBit);
            }
        }
        else
        {
            //
            // Loop over the columns of the logo.
            //
            for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
            {
                //
                // Copy 16 - N scan lines of of the second row of the logo to
                // the first row of the local frame buffer.
                //
                g_pucFrame[ulX + ulLoop] = (pucLogo[ulLoop + ulWidth] >>
                                            (ulBit - 8));
            }
        }

        //
        // Display the local frame buffer on the display.
        //
        OSRAMImageDraw(g_pucFrame, 0, 0, 96, 2);

        //
        // Wait for a twentieth of a second.
        //
        for(ulLoop = 0; ulLoop < (CLOCK_RATE / 20); ulLoop++)
        {
            //
            // Wait until a SysTick interrupt has occurred.
            //
            while(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK))
            {
            }

            //
            // Clear the SysTick interrupt flag.
            //
            HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 0;
        }
    }
}

//*****************************************************************************
//
// Scrolls a wide image across the display.
//
//*****************************************************************************
static void
ScrollImage(const unsigned char *pucImage, unsigned long ulWidth)
{
    unsigned long ulLoop, ulIdx;

    //
    // Loop over the columns of the image plus the columns of the display,
    // scrolling the image across the dipslay from right to left one column at
    // a time.
    //
    for(ulIdx = 1; ulIdx <= (ulWidth + 96); ulIdx++)
    {
        //
        // Clear the local frame buffer.
        //
        for(ulLoop = 0; ulLoop < 192; ulLoop += 4)
        {
            *(unsigned long *)(g_pucFrame + ulLoop) = 0;
        }

        //
        // See if the image has reached the left side of the display.
        //
        if(ulIdx <= 96)
        {
            //
            // Copy the first N columns of the image to the right side of the
            // local frame buffer.
            //
            for(ulLoop = 0; ulLoop < ulIdx; ulLoop++)
            {
                g_pucFrame[ulLoop + 96 - ulIdx] = pucImage[ulLoop];
                g_pucFrame[ulLoop + 96 - ulIdx + 96] =
                    pucImage[ulLoop + ulWidth];
            }
        }

        //
        // See if the right side of the image has reached the right side of the
        // display.
        //
        else if(ulIdx < ulWidth)
        {
            //
            // Copy 96 columns from the middle of the image to the local frame
            // buffer.
            //
            for(ulLoop = 0; ulLoop < 96; ulLoop++)
            {
                g_pucFrame[ulLoop] = pucImage[ulLoop + ulIdx - 96];
                g_pucFrame[ulLoop + 96] =
                    pucImage[ulLoop + ulIdx - 96 + ulWidth];
            }
        }

        //
        // Otherwise, the right side of the image has already reached the right
        // side of the display.
        //
        else
        {
            //
            // Copy the right N columns of the image to the left side of the
            // local frame buffer.
            //
            for(ulLoop = 0; ulLoop < (ulWidth + 96 - ulIdx); ulLoop++)
            {
                g_pucFrame[ulLoop] = pucImage[ulLoop + ulIdx - 96];
                g_pucFrame[ulLoop + 96] =
                    pucImage[ulLoop + ulIdx - 96 + ulWidth];
            }
        }

        //
        // Display the local frame buffer on the display.
        //
        OSRAMImageDraw(g_pucFrame, 0, 0, 96, 2);

        //
        // Wait for a thirtieth of a second.
        //
        for(ulLoop = 0; ulLoop < (CLOCK_RATE / 30); ulLoop++)
        {
            //
            // Wait until a SysTick interrupt has occurred.
            //
            while(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK))
            {
            }

            //
            // Clear the SysTick interrupt flag.
            //
            HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 0;
        }
    }
}


#ifdef __GNUC__
//*****************************************************************************
// 
// Show that this code has been compiled with the GNU tools (mthomas)
//
//*****************************************************************************
static void GCCInfo(void)
{
	unsigned long ulLoop;
	
	OSRAMStringDraw("Compiled with", 0, 0);
	OSRAMStringDraw("GNU-Tools", 0, 45);
	// wait 3 sec
	for(ulLoop = 0; ulLoop < (CLOCK_RATE*3); ulLoop++) {
		// Wait until a SysTick interrupt has occurred.
		while(!HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK)) { ; }
		// Clear the SysTick interrupt flag.
		HWREGBITW(&g_ulFlags, FLAG_CLOCK_TICK) = 0;
	}
}
#endif

//*****************************************************************************
//
// The main code for the application.  It sets up the peripherals, displays the
// splash screens, and then manages the interaction between the game and the
// screen saver.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run at 20MHz from the PLL.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_6MHZ);

    //
    // Enable the peripherals used by the application.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure the ADC to sample the potentiometer when the timer expires.
    // After sampling, the ADC will interrupt the processor; this is used as
    // the heartbeat for the game.
    //
    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                             ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC_BASE, 0);
    ADCIntEnable(ADC_BASE, 0);
    IntEnable(INT_ADC0);

    //
    // Configure the first timer to generate a 10 kHz PWM signal for driving
    // the user LED.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_B_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_B, (SysCtlClockGet() / 10000) - 1);
    TimerMatchSet(TIMER0_BASE, TIMER_B, 0);
    TimerControlLevel(TIMER0_BASE, TIMER_B, true);
    TimerEnable(TIMER0_BASE, TIMER_B);

    //
    // Configure the second timer to generate triggers to the ADC to sample the
    // potentiometer.
    //
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 120);
    TimerControlStall(TIMER1_BASE, TIMER_A, true);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
    TimerEnable(TIMER1_BASE, TIMER_A);

    //
    // Configure the LED, push button, and UART GPIOs as required.
    //
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW);
    GPIODirModeSet(GPIO_PORTC_BASE, PUSH_BUTTON, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_PORTC_BASE, USER_LED, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);

    //
    // Configure the first UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSet(UART0_BASE, 115200,
                  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                   UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);

    //
    // Send a welcome message to the UART.
    //
    UARTCharPut(UART0_BASE, 'L');
    UARTCharPut(UART0_BASE, 'M');
    UARTCharPut(UART0_BASE, '3');
    UARTCharPut(UART0_BASE, 'S');
    UARTCharPut(UART0_BASE, '8');
    UARTCharPut(UART0_BASE, '1');
    UARTCharPut(UART0_BASE, '1');
    UARTCharPut(UART0_BASE, '\r');
    UARTCharPut(UART0_BASE, '\n');

    //
    // Initialize the OSRAM OLED display.
    //
    OSRAMInit(true);

    //
    // Display the Luminary Micro logo for five seconds.
    //
    DisplayLogo(g_pucLMILogo, 6, 83, 5 * CLOCK_RATE);

    //
    // Display the Keil/ARM logo for five seconds.
    //
    DisplayLogo(g_pucKeilLogo, 10, 76, 5 * CLOCK_RATE);

    //
    // Scroll the design contest image.
    //
    ScrollImage(g_pucDesignLogo, 300);

#ifdef __GNUC__
	// 
	// Show "compiled with GCC" (mthomas)
	//
	GCCInfo();
#endif

    //
    // Throw away any button presses that may have occurred while the splash
    // screens were being displayed.
    //
    HWREGBITW(&g_ulFlags, FLAG_BUTTON_PRESS) = 0;

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Display the main screen.
        //
        if(MainScreen())
        {
            //
            // The button was pressed, so start the game.
            //
            PlayGame();
        }
        else
        {
            //
            // The button was not pressed during the timeout period, so start
            // the screen saver.
            //
            ScreenSaver();
        }
    }
}
