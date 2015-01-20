/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
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

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>

#include "Print.h"

#define BUFFER_LENGTH 64
//#define CRC

class TwoWire : public Print
{
  private:
    static uint8_t buffer[];
    static uint8_t bufferIndex;
    static uint8_t bufferLength;

    static uint8_t txAddress;

    static void (*user_onRequest)(void);
    static void (*user_onReceive)(uint8_t*, uint8_t);
    static void onRequestService(void);
    static void onReceiveService(uint8_t*, uint8_t);
#ifdef CRC
    static void addCRC();
    static uint8_t checkCRC(const uint8_t* inBytes, const uint8_t numBytes);
#endif

  public:
    TwoWire();
    void begin();
    void begin(uint8_t);
    uint8_t transmit(uint8_t address, const uint8_t* data, uint8_t quantity, bool wait=true);
    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool wait=true);
    uint8_t requestFrom(uint8_t, uint8_t);
    void queue(uint8_t);
    void queue(const uint8_t* data, uint8_t quantity);
    void queue(const char*);
    uint8_t reply(const uint8_t*, uint8_t);
    uint8_t available(void);
    uint8_t receive(void);
    void onReceive( void (*)(uint8_t*, uint8_t) );
    void onRequest( void (*)(void) );

    virtual void write(uint8_t);
    virtual void write(const uint8_t*, uint8_t);
    virtual void write(const char*);
};

extern TwoWire Wire;

#endif

