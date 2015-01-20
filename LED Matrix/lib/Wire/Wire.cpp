/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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

extern "C" {
	#include <stdlib.h>
	#include <string.h>
	#include <inttypes.h>
	#include "utility/twi.h"
	#include <util/crc16.h>
}

#include "Wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

//Transmit and request buffer
uint8_t TwoWire::buffer[BUFFER_LENGTH];
uint8_t TwoWire::bufferIndex = 0;
uint8_t TwoWire::bufferLength = 0;

uint8_t TwoWire::txAddress = 0;

void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(uint8_t*, uint8_t);

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire(){}

// Public Methods //////////////////////////////////////////////////////////////

//Master only
void TwoWire::begin(void){
	twi_init();
}

void TwoWire::begin(uint8_t address){
	twi_setAddress(address);
	twi_attachSlaveTxEvent(onRequestService);
	twi_attachSlaveRxEvent(onReceiveService);
	begin();
}

//Returns amount of bytes read
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity){
	// clamp to buffer length
	if(quantity > BUFFER_LENGTH){
		quantity = BUFFER_LENGTH;
	}
	// perform blocking read into buffer
	uint8_t read = twi_readFrom(address, buffer, quantity);
	//often fails on first try, retry
	if(read == 0 && quantity > 0) read = twi_readFrom(address, buffer, quantity);

	// set rx buffer iterator vars
	bufferIndex = 0;
#ifdef CRC
	if(address == QUAD1 || address == QUAD2){
		if(!checkCRC(buffer, read)){
			bufferLength = 0;
			return 0;
		}
		read--;
	}
#endif
	bufferLength = read;

	return read;
}

uint8_t TwoWire::transmit(uint8_t address, const uint8_t* data, uint8_t quantity, bool wait){
#ifdef CRC
	if(address == QUAD1 || address == QUAD2){
		bufferIndex = 0;
		bufferLength = 0;
		queue(data, quantity);
		addCRC();
		return twi_writeTo(address, buffer, bufferLength, wait);
	}else{
#endif
		return twi_writeTo(address, data, quantity, wait);
#ifdef CRC
	}
#endif
}

//Start buffering data for transmission
void TwoWire::beginTransmission(uint8_t address){
	// set address of targeted slave
	txAddress = address;
	// reset buffer iterator vars
	bufferIndex = 0;
	bufferLength = 0;
}

//Return value is not correct if wait is false
uint8_t TwoWire::endTransmission(bool wait){
#ifdef CRC
	if(txAddress == QUAD1 || txAddress == QUAD2){
		addCRC();
	}
#endif

	// transmit buffer (blocking if wait)
	uint8_t ret = twi_writeTo(txAddress, buffer, bufferLength, wait);
	if(ret != 0){//Retry
		ret = twi_writeTo(txAddress, buffer, bufferLength, wait);
	}
	return ret;
}

// must be called after beginTransmission(address)
void TwoWire::queue(uint8_t data){
    // don't bother if buffer is full
    if(bufferLength >= BUFFER_LENGTH){
      return;
    }
    // put byte in tx buffer
    buffer[bufferIndex++] = data;
    // update amount in buffer   
    bufferLength = bufferIndex;
}

// must be called after beginTransmission(address)
void TwoWire::queue(const uint8_t* data, uint8_t quantity){
	for(uint8_t i = 0; i < quantity; ++i){
	  queue(data[i]);
	}
}

//only to be called in onReceiveService callback
uint8_t TwoWire::reply(const uint8_t* data, uint8_t quantity){
	//reply to master
	return twi_transmit(data, quantity);
}

// must be called in after beginTransmission(address)
void TwoWire::queue(const char* data){
	queue((uint8_t*)data, strlen(data));
}

// must be called in:
// slave rx event callback or after requestFrom(address, numBytes)
uint8_t TwoWire::available(void){
	return bufferLength - bufferIndex;
}

// must be called after requestFrom(address, numBytes)
uint8_t TwoWire::receive(void){
	uint8_t value = 0;

	// get each successive byte on each call
	if(bufferIndex < bufferLength){
		value = buffer[bufferIndex++];
	}

	return value;
}

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(uint8_t* inBytes, uint8_t numBytes){
#ifdef CRC
	//Assume other quad
	//if(address == QUAD1 || address == QUAD2){
		if(!checkCRC(inBytes, numBytes)){
			return;
		}
		numBytes--;
	//}
#endif

	// don't bother if user hasn't registered a callback
	if(user_onReceive != NULL){
		user_onReceive(inBytes, numBytes);
	}
}

// behind the scenes function that is called when data is requested
void TwoWire::onRequestService(void){
	// don't bother if user hasn't registered a callback
	if(user_onRequest != NULL){
		user_onRequest();
	}

#ifdef CRC
	//Assume other quad
	//if(address == QUAD1 || address == QUAD2){
		addCRC();
	//}
#endif
}

// sets function called on slave write
void TwoWire::onReceive( void (*function)(uint8_t*, uint8_t) ){
	user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void) ){
	user_onRequest = function;
}

void inline TwoWire::write(uint8_t data){
	queue(data);
}

void inline TwoWire::write(const uint8_t* data, uint8_t quantity){
	queue(data,quantity);
}

void inline TwoWire::write(const char *str){
	queue(str);
}

#ifdef CRC
void TwoWire::addCRC(){
		uint8_t crc = 0, i;
		for (i = 0; i < bufferLength; i++){
			crc = _crc_ibutton_update(crc, buffer[i]);
		}
		Wire.queue(crc);
}

uint8_t TwoWire::checkCRC(const uint8_t* inBytes, const uint8_t numBytes){
		uint8_t crc = 0, i;
		for (i = 0; i < numBytes-1; i++){
			crc = _crc_ibutton_update(crc, inBytes[i]);
		}
		if(crc != inBytes[numBytes-1]){
			warn(9);
			return 0;
		}
		return 1;
}
#endif

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();

