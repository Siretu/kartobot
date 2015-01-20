/*
 * USB.c
 *
 *  Created on: 21 Jun 2012
 *      Author: exuvo
 */

#include "lpc_types.h"

#include "USB.h"
#include "queue.h"
#include <math.h>
#include "utils.h"
#include "term_io.h"
#include "main.h"
#include "usbcore.h"
#include "usbhw.h"
#include "cdcuser.h"
#include "semphr.h"
#include "usbcfg.h"
#include "DiskImg.h"
#include <string.h>
#include "mscuser.h"
#include "usbdescCDC.h"
#include "usbdescMSC.h"
#include "usbdescMULTI.h"
#include "Comm.h"
#include "Time2.h"

#define START_BYTE 0x7e
#define ESCAPE 0x7d

#define USB_IN_SIZE 	256U
#define USB_OUT_SIZE	256U

static uint8_t USB_in[USB_IN_SIZE];
static volatile uint16_t USB_inW;
static volatile uint16_t USB_inR;

static uint8_t USB_out[USB_OUT_SIZE];
static volatile uint16_t USB_outW;
static volatile uint16_t USB_outR;

static xSemaphoreHandle USB_lock;

//1 Connected, 2 Authenticated
uint8_t USB_Connected;

static portSTACK_TYPE usbTaskStack[128];
const xTaskParameters usbTaskDefinition =
{
		USB_Run,   							/* pvTaskCode */
		(signed char *) "USB",  /* pcName */
    128,             				/* usStackDepth - defined in words, not bytes. */
    NULL,            				/* pvParameters */
    6 | portPRIVILEGE_BIT,  /* uxPriority */
    usbTaskStack,      			/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

uint8_t USB_isConnected(){
	return USB_Connected == 2;
}

uint8_t USB_hasConnection(){
	return USB_Connected > 0;
}

static uint8_t* escape(uint8_t d, uint8_t* buffer) {
	if (d == START_BYTE || d == ESCAPE) {
		*buffer++ = ESCAPE;
		*buffer++ = d ^ 0x20;
	} else {
		*buffer++ = d;
	}
	return buffer;
}

static uint16_t escapeCount(const uint8_t* data, uint16_t length) {
	uint16_t size = 0;

	if ((length >> 8) == START_BYTE || (length >> 8) == ESCAPE) {
		size++;
	}
	if ((length & 0xFF) == START_BYTE || (length & 0xFF) == ESCAPE) {
		size++;
	}

	for(uint16_t i=0; i<length; i++){
		if (data[i] == START_BYTE || data[i] == ESCAPE) {
			size++;
		}
	}

	return size;
}

//  START, SIZE, SIZE, DATA..
void USB_Send(const uint8_t *data, uint16_t length){
	xSemaphoreTake(USB_lock, portMAX_DELAY);

	uint16_t size = length + 3 + escapeCount(data, length);
	uint8_t *buf = (uint8_t*) pvPortMalloc(size);

	if(buf != NULL){
		uint8_t *p = buf;
		*p++ = START_BYTE;

		p = escape(length >> 8, p);
		p = escape(length, p);

		for (uint8_t i=0; i < length; i++) {
			p = escape(data[i], p);
		}

		p = buf;
		while(size > 0){
			while((uint16_t)(USB_outW - USB_outR) == USB_OUT_SIZE){
				portYIELD();// vTaskDelay(1 / portTICK_RATE_MS);
				if(!USB_isConnected()){
					goto ret;
				}
			}
			USB_out[USB_outW++ % USB_OUT_SIZE] = *p++;
			size--;
		}

		ret:
		vPortFree(buf);
	}

	xSemaphoreGive(USB_lock);
}

uint16_t USB_Available(){
	return USB_inW - USB_inR;
}

uint8_t USB_Available2(){
	uint16_t a = USB_Available();
	return a > 255 ? 255 : a;
}

uint8_t USB_Read(){
	if((uint16_t)(USB_inW - USB_inR) != 0){
		return USB_in[USB_inR++ % USB_IN_SIZE];
	}else{
		vTaskDelay(1);
		if((uint16_t)(USB_inW - USB_inR) != 0){
			return USB_in[USB_inR++ % USB_IN_SIZE];
		}else{
			return 0;
		}
	}
}

uint16_t USB_ToSend(){
	return USB_outW - USB_outR;
}

uint8_t USB_Put(const uint8_t *data, uint16_t length){
	while(length > 0){
		if((uint16_t)(USB_inW - USB_inR) == USB_IN_SIZE){
			return FALSE;
		}
		USB_in[USB_inW++ % USB_IN_SIZE] = *data++;
		length--;
	}
	return TRUE;
}

uint16_t USB_Get(uint8_t *buff, uint16_t len){
	uint16_t read = 0;

	while((uint16_t)(USB_outW - USB_outR) != 0 && read < len){
		*buff++ = USB_out[USB_outR++ % USB_OUT_SIZE];
		read++;
	}

	return read;
}

void USB_ResetBuffers(){
	memset(USB_in, 0, USB_IN_SIZE);
	memset(USB_out, 0, USB_OUT_SIZE);
	USB_inW = USB_inR = USB_outW = USB_outR = 0;
}

static uint8_t changeDescriptorClass;

void USB_ChangeDescriptor(uint8_t descriptorClass){
	USB_Connect(FALSE);
	USB_Connected = 0;
	changeDescriptorClass = descriptorClass;
	xTaskResumeFromISR(tasks.usb);
}

void USB_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();
	uint32_t lastReceived = Time_Millis();
	USB_Connect(TRUE);

	while(1){
		if(USB_Connected > 0){

			if(USB_DeviceDescriptor == USB_CDC_DeviceDescriptor && USB_ToSend()){
				if(CDC_DepInEmpty){
					uint16_t size = USB_Available();
					uint8_t data[USB_CDC_BUFSIZE];

					USB_Get(data, size > USB_CDC_BUFSIZE ? USB_CDC_BUFSIZE : size);

					CDC_DepInEmpty = 0;
					USB_WriteEP (CDC_DEP_IN, data, size);
				}
			}

			while(USB_Available()){//TODO wait for packet header
//				xprintf("USB Available %u\n", (unsigned int) USB_Available());
				lastReceived = Time_Millis();
				switch(USB_Read()){
					case '?':{
						USB_Send((uint8_t*) "?KartoBot", 9);
						break;
					}
					case '!':{
						//TODO check password
						USB_Connected = 2;
						USB_Send((uint8_t*) "!", 1);
						xprintf("USB Authenticated\n");
						break;
					}
					case 'p':{
						USB_Send((uint8_t*) "p", 1);
						break;
					}
					default:{
						if(USB_isConnected()){
							USB_inR--;
							Communication(&USB_Read, &USB_Available2, &USB_Send);
						}
						break;
					}
				}
			}

			if(USB_isConnected()){
				if(Time_Millis() - lastReceived > 2000){//Lost connection
					USB_Connected = 1;
					xprintf("USB Lost Connection\n");
				}
			}

			vTaskDelayUntil(&xLastWakeTime, 2);

		}else{
			if(changeDescriptorClass){
				uint8_t descriptorClass = changeDescriptorClass;
				changeDescriptorClass = 0;
				vTaskDelay(300);

				if(descriptorClass == DESCRIPTOR_TOGGLE){
					if(USB_DeviceDescriptor == USB_CDC_DeviceDescriptor){
						descriptorClass = DESCRIPTOR_MSC;
					}else{
						descriptorClass = DESCRIPTOR_CDC;
					}
				}

				if(descriptorClass == DESCRIPTOR_CDC){
					USB_DeviceDescriptor = USB_CDC_DeviceDescriptor;
					USB_ConfigDescriptor = USB_CDC_ConfigDescriptor;
					USB_StringDescriptor = USB_CDC_StringDescriptor;
				}else if(descriptorClass == DESCRIPTOR_MSC){
					USB_DeviceDescriptor = USB_MSC_DeviceDescriptor;
					USB_ConfigDescriptor = USB_MSC_ConfigDescriptor;
					USB_StringDescriptor = USB_MSC_StringDescriptor;
				}else if(descriptorClass == DESCRIPTOR_MULTI){
					USB_DeviceDescriptor = USB_MULTI_DeviceDescriptor;
					USB_ConfigDescriptor = USB_MULTI_ConfigDescriptor;
					USB_StringDescriptor = USB_MULTI_StringDescriptor;
				}

				USB_Connect(TRUE);
			}

			vTaskSuspend(NULL);
		}
	}
}

void USB_Init2(){
	USB_lock = xSemaphoreCreateMutex();

#ifndef MSC_RO
	memcpy(MSC_Memory, DiskImage, MSC_ImageSize); /* Copy Initial Disk Image from Flash to RAM */
#endif

	CDC_Init();
	NVIC_SetPriority(USB_IRQn, 17);
	USB_Init();
}
