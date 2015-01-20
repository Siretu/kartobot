/*
 * Xbee.cpp
 *
 *  Created on: 7 aug 2010
 *      Author: exuvo
 */

//API
#include "XBeeLib.h"
#include "XBee.h"
#include "Time2.h"
#include <string.h>
#include "term_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "utils.h"
#include "UART.h"
#include "USB.h"
#include "Comm.h"
#include "dbg.h"

//TODO check if anything in the new XBee module needs to be implemented

#define WAIT 10

struct AddressMap{
	uint64_t a64;
	uint16_t a16;
};

xQueueHandle XBee_out;
static xSemaphoreHandle XBee_lock;

typedef struct{
	AddressMap addressmap[8];
	XBeeAddress64 computer;
	ZBRxResponse *zbRx ;
}xbeeVars;

static xbeeVars xv;

portSTACK_TYPE XBeeInTaskStack[512];
extern "C" const xTaskParameters XBeeInTaskDefinition =
{
	XBee_In,   					/* pvTaskCode */
	(signed char *) "XBee In", 	/* pcName */
	512,             			/* usStackDepth - defined in words, not bytes. */
    NULL,            			/* pvParameters */
    6 | portPRIVILEGE_BIT,      /* uxPriority */
    XBeeInTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

portSTACK_TYPE XBeeOutTaskStack[256];
extern "C" const xTaskParameters XBeeOutTaskDefinition =
{
	XBee_Out,   				/* pvTaskCode */
	(signed char *) "XBee Out", /* pcName */
	256,             			/* usStackDepth - defined in words, not bytes. */
    NULL,            			/* pvParameters */
    6 | portPRIVILEGE_BIT,      /* uxPriority */
    XBeeOutTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

extern "C" void XBee_Init(){
	xv.computer = XBeeAddress64(0x0013A200, 0x405C2C95);
	XBee_out = xQueueCreate(32, sizeof(uint8_t*));
	XBee_lock = xSemaphoreCreateMutex();
}

void XBee_Send(const char *data){
	XBee_Send((uint8_t*)data, strlen(data));
}

extern "C" void XBee_Send(const uint8_t *data, size_t length){//ca 150bytes/s http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1272620731
	if(xSemaphoreTake(XBee_lock, portMAX_DELAY) == pdTRUE){
		while(uxQueueMessagesWaiting(XBee_out) >= 32){
			vTaskDelay(1);
		}

		uint8_t size = length > MAX_FRAME_DATA ? MAX_FRAME_DATA : length;
		uint8_t *p = (uint8_t*) pvPortMalloc2(size+1);
		if(p != NULL){
			*p = size;
			memcpy(p+1, data, size);
			if(xQueueSendToBack(XBee_out, &p, WAIT) != pdPASS ){
				vPortFree2(p);
			}
		}

		xSemaphoreGive(XBee_lock);
	}
}

static uint16_t getA16(XBeeAddress64 a64){
	uint64_t a = ((uint64_t)a64.getMsb() << 32) | a64.getLsb();
	for(uint32_t i=0; i<ARRAY_LENGTH(xv.addressmap); i++){
		AddressMap *m = &xv.addressmap[i];
		if(m->a64 == a){
			return m->a16;
		}
	}
	return ZB_BROADCAST_ADDRESS;
}

static void checkAddress(XBeeAddress64 a64, uint16_t a16){
	uint64_t a = ((uint64_t)a64.getMsb() << 32) | a64.getLsb();
	for(uint32_t i=0; i<ARRAY_LENGTH(xv.addressmap); i++){
		AddressMap *m = &xv.addressmap[i];
		if(m->a64 == a){
			m->a16 = a16;
			return;
		}
	}
	//new address
	for(uint32_t i=0; i<ARRAY_LENGTH(xv.addressmap); i++){
		AddressMap *m = &xv.addressmap[i];
		if(m->a64 == 0){
			m->a64 = ((uint64_t)a64.getMsb() << 32) | a64.getLsb();
			m->a16 = a16;
			return;
		}
	}
}

static uint8_t* escape(uint8_t d, uint8_t* buffer) {
	if (d == START_BYTE || d == ESCAPE || d == XON || d == XOFF) {
		*buffer++ = ESCAPE;
		*buffer++ = d ^ 0x20;
	} else {
		*buffer++ = d;
	}
	return buffer;
}

static size_t XBee_Packetize(XBeeRequest &request, uint8_t * buffer){
	uint8_t *p = buffer;
	*p++ = START_BYTE;

	// length
	uint8_t msbLen = ((request.getFrameDataLength() + 2) >> 8) & 0xff;
	uint8_t lsbLen = (request.getFrameDataLength() + 2) & 0xff;
	p = escape(msbLen, p);
	p = escape(lsbLen, p);

	// api id
	p = escape(request.getApiId(), p);
	p = escape(request.getFrameId(), p);

	uint8_t checksum = 0;
	checksum+= request.getApiId();
	checksum+= request.getFrameId();

	// data
	for (uint8_t i = 0; i < request.getFrameDataLength(); i++) {
		p = escape(request.getFrameData(i), p);
		checksum+= request.getFrameData(i);
	}

	// perform 2s complement
	checksum = 0xff - checksum;
	p = escape(checksum, p);
	return p - buffer;
}

static void XBee_SendRequest(XBeeRequest &request) {
	uint8_t buffer[MAX_FRAME_DATA_SIZE];
	size_t length = XBee_Packetize(request, buffer);
	UART1_send(buffer, length);
}

//Sends a text warning
extern "C" void XBee_SendImmediately(const uint8_t* data, size_t toSend){
	uint32_t size, sent = 0;
	while(sent < toSend){
		size = toSend - sent > MAX_FRAME_DATA ? MAX_FRAME_DATA : toSend - sent;

		uint8_t data2[size+1]; data2[0] = 'w';
		memcpy(data2+1, data + sent, size);

		ZBTxRequest zbTx = ZBTxRequest(xv.computer, getA16(xv.computer), ZB_BROADCAST_RADIUS_MAX_HOPS, ZB_TX_UNICAST, data2, size, 0);//0=skip TX response
		XBeeRequest &request = zbTx;

		uint8_t buffer[MAX_FRAME_DATA_SIZE];
		size_t length = XBee_Packetize(request, buffer);

		UART1_sendForced(buffer, length);
		sent += size;
	}
}

extern "C" void XBee_Out(void *pvParameters){
	XBeeAddress64 destination = xv.computer;
	xQueueHandle out = XBee_out;

	XBee_Send("B");

	portTickType xLastWakeTime = xTaskGetTickCount();
	while(true){
		vTaskDelayUntil(&xLastWakeTime, 5 / portTICK_RATE_MS);

		if(term_io_available()){
			uint8_t data[MAX_FRAME_DATA]; data[0] = 'c';
			uint16_t size = term_io_get((char*) &data[1], MAX_FRAME_DATA-1) + 1;

			if(size > 1){
				if(USB_isConnected()){
					USB_Send(data, size);
				}else{
					ZBTxRequest zbTx = ZBTxRequest(destination, getA16(destination), ZB_BROADCAST_RADIUS_MAX_HOPS, ZB_TX_UNICAST, data, size, 0);//0=skip TX response
					XBee_SendRequest(zbTx);
				}
			}
		}

		uint8_t *p;
		while(xQueueReceive(out, &p, 0) == pdPASS){
			uint8_t size = *p;
			uint8_t data[size];
			memcpy(data, p+1, size);

			if(USB_isConnected() && destination.getMsb() == xv.computer.getMsb() && destination.getLsb() == xv.computer.getLsb()){
				USB_Send(data, size);
			}else{
				ZBTxRequest zbTx = ZBTxRequest(destination, getA16(destination), ZB_BROADCAST_RADIUS_MAX_HOPS, ZB_TX_UNICAST, data, size, 0);//0=skip TX response
				XBee_SendRequest(zbTx);
			}
			vPortFree2(p);
		}
	}
}

extern "C" uint8_t XBee__available(){
	return xv.zbRx->available();
}

extern "C" uint8_t XBee__next(){
//	debug("n %c %u", xv.zbRx->getData()[xv.zbRx->getPosition()], xv.zbRx->getPosition());
	return xv.zbRx->next();
}

extern "C" void XBee__send(const uint8_t *data, uint16_t length){
	XBee_Send(data, length);
}

extern "C" void XBee_In(void *pvParameters){
	XBee xbee = XBee();
	uint32_t lastReceived = Time_Millis();

	portTickType xLastWakeTime = xTaskGetTickCount();
	while(true){
		vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_RATE_MS);

		while(xbee.readPacket()){
			if(xbee.getResponse().isAvailable()){
				switch (xbee.getResponse().getApiId()){
					case AT_COMMAND_RESPONSE:{
						AtCommandResponse atResponse = AtCommandResponse();
						xbee.getResponse().getAtCommandResponse(atResponse);
						if (atResponse.isOk()) {
							// << "Command [" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "] was successful!" << endl;
							if (atResponse.getValueLength() > 0) {
								// << "Command value: ";
								for (int i = 0; i < atResponse.getValueLength(); i++) {
									// << _HEX(atResponse.getValue()[i]) << " ";
								}
							}
						}else {
							// << "Command return error code: " << _HEX(atResponse.getStatus()) << endl;
						}
						break;
					}
					case ZB_TX_STATUS_RESPONSE:{
						ZBTxStatusResponse txStatus = ZBTxStatusResponse();
						xbee.getResponse().getZBTxStatusResponse(txStatus);

						if (txStatus.getDeliveryStatus() == SUCCESS) {
							// success.
						} else {
							// the remote XBee did not receive our packet. is it powered on?
						}
						break;
					}
					case ZB_RX_RESPONSE:{
						// data received
						ZBRxResponse zbRx = ZBRxResponse();
						xbee.getResponse().getZBRxResponse(zbRx);

						if (zbRx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
							// the sender got an ACK
						} else {
							// we got it (obviously) but sender didn't get an ACK
						}

						checkAddress(zbRx.getRemoteAddress64(), zbRx.getRemoteAddress16());
						if(zbRx.getRemoteAddress64().getMsb() == xv.computer.getMsb() && zbRx.getRemoteAddress64().getLsb() == xv.computer.getLsb()){
							lastReceived = Time_Millis();

							while(zbRx.available() > 0){
								switch(zbRx.next()){//Command
									case 't':{	//TCP packet
										uint8_t b[2] = {'t', zbRx.next()};
										XBee_Send(b, sizeof(b));
										break;
									}
									case 'p':{	//ping
										XBee_Send("p");
										break;
									}
									default:
										zbRx.setPosition(zbRx.getPosition()-1);
										xv.zbRx = &zbRx;
										Communication(&XBee__next, &XBee__available, &XBee__send);
								}
							}
							break;
						}else{//Someone else?

						}
						break;
					}
					case MODEM_STATUS_RESPONSE:{
						ModemStatusResponse msr = ModemStatusResponse();
						xbee.getResponse().getModemStatusResponse(msr);
						// the local XBee sends this response on certain events, like association/dissociation

						if(msr.getStatus() == ASSOCIATED){
						  // yay this is great.  flash led

						}else if (msr.getStatus() == DISASSOCIATED){
						  // this is awful.. flash led to show our discontent

						}else{
						  // another status

						}
						break;
					}
					default://Unknown command received
						uint8_t b[2] = {'e',16};
						XBee_Send(b, sizeof(b));
						break;
				}
			}else if (xbee.getResponse().isError()) {
				uint8_t b[2] = {'e', xbee.getResponse().getErrorCode() + 11};
				XBee_Send(b, sizeof(b));
			}
		}

		if(Time_Millis() - lastReceived > 2000){//Lost connection

		}
	}
}


