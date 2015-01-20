/*
 * Xbee.h
 *
 *  Created on: 7 aug 2010
 *      Author: exuvo
 */

#ifndef XBEE_H_
#define XBEE_H_

#include <XBee/XBee.h>
#include <PString/PString.h>

extern XBee xbee;
extern XBeeAddress64 computer;

void XBeeSend(const char*);
void XBeeSend(uint8_t*, const size_t);
void XBeeSend(PString);
void XBeeRead();

#endif /* XBEE_H_ */
