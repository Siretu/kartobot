/*
  twi.h - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef twi_h
#define twi_h

  #include <inttypes.h>

  //#define ATMEGA8

  #ifndef CPU_FREQ
  #define CPU_FREQ 16000000L
  #endif
  
  //TODO calculate how much time this is
  #define TWI_TIMEOUT 2000

  #ifndef TWI_FREQ
//#define TWI_FREQ 100000L
  #define TWI_FREQ 400000L
  #endif

  #ifndef TWI_BUFFER_LENGTH
  #define TWI_BUFFER_LENGTH 64
  #endif

  #ifndef MULTIMASTER_BUSY_CHECKS_AFTER_STOP
  //A value of 0 turns off this feature. Greater values are slower(only affects Masters who use Slave Receive) but more reliable.
  //Fixes a i2c hardware error in AVRs when using Multiple Masters, (might miss a Start after receiving in Slave mode)
  //http://www.robotroom.com/Atmel-AVR-TWI-I2C-Multi-Master-Problem.html
  #define MULTIMASTER_BUSY_CHECKS_AFTER_STOP 5
  #endif

  #define TWI_READY 0
  #define TWI_MRX   1
  #define TWI_MTX   2
  #define TWI_SRX   3
  #define TWI_STX   4
  
  void twi_init(void);
  void twi_setAddress(uint8_t);
  uint8_t twi_readFrom(uint8_t, uint8_t*, uint8_t);
  uint8_t twi_writeTo(uint8_t, const uint8_t*, uint8_t, uint8_t);
  uint8_t twi_transmit(const uint8_t*, uint8_t);
  void twi_attachSlaveRxEvent( void (*)(uint8_t*, uint8_t) );
  void twi_attachSlaveTxEvent( void (*)(void) );
  void twi_reply(uint8_t);
  void twi_stop(void);
  void twi_releaseBus(void);
  bool twi_timeout(uint8_t errorI);
  void twi_timeoutReset();
  bool busyCheck();

#endif

