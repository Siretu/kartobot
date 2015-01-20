/*
 * i2c.cpp
 *
 *  Created on: 7 aug 2010
 *      Author: exuvo
 */

#include "main.h"
#include <PString/PString.h>
#include <Wire/Wire.h>
#include <Wire/utility/twi.c>
#include "bar.h"
#include "matrix.h"

uint8_t request = 0;
uint32_t lastReceived = millis();

void comm(uint8_t* data, uint8_t length){
	uint8_t next = 0;
	lastReceived = millis();

	while(next < length){
		char d = data[next++];
		if(d == 'b'){//LED Bargraph
			uint16_t v = data[next++] << 8; v |= data[next++];
			BarUpdate(v);
		}else if(d == 'm'){//Matrix
			for(uint8_t y=0; y<8; y++){
				uint16_t v = data[next++] << 8; v |= data[next++];
				matrixUpdate(y, v);
			}
		}
	}
}

void commRequest(){
	char buffer[32];
	PString str(buffer,sizeof(buffer));
	str = "";

	if(request == 'a'){//a
	}

	request = 0;
	Wire.reply((uint8_t*)&*str,str.length());
}
