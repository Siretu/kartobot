/*
  wiring_shift.c - shiftOut() function
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
#include <util/delay.h>

void shiftOut(uint8_t *port, uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, byte val){
	uint8_t state;
	for (uint8_t i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST){
			state = !!(val & (1 << i));
		}else{
			state = !!(val & (1 << (7 - i)));
		}
		if(state){
			*port |= _BV(dataPin);
		}else{
			*port &= ~_BV(dataPin);
		}

		*port |= _BV(clockPin);
		asm("nop");
		*port &= ~_BV(clockPin);
	}
}
