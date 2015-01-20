/*
 * Xbee.h
 *
 *  Created on: 7 aug 2010
 *      Author: exuvo
 */

#ifndef XBEE_H_
#define XBEE_H_

#include "lpc_types.h"
#include <stdlib.h>
#include "FreeRTOS.h"

#ifdef __cplusplus

#include "XBeeLib.h"

extern "C" void XBee_Init();
void XBee_Send(const char*);
extern "C" void XBee_Send(const uint8_t*, const size_t);
extern "C" void XBee_SendImmediately(const uint8_t*, size_t);
void XBee_Send(XBeeRequest &request);
extern "C" void XBee_Out(void *pvParameters);
extern "C" void XBee_In(void *pvParameters);

#else

#define MAX_FRAME_DATA 72

void XBee_Init();
void XBee_Send(const uint8_t*, const size_t);
void XBee_SendImmediately(const uint8_t*, size_t);
extern const xTaskParameters XBeeInTaskDefinition;
extern const xTaskParameters XBeeOutTaskDefinition;

#endif



#endif /* XBEE_H_ */
