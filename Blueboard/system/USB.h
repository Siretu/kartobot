/*
 * USB.h
 *
 *  Created on: 21 Jun 2012
 *      Author: exuvo
 */

#ifndef USB_H_
#define USB_H_

#include "FreeRTOS.h"
#include "task.h"

#define DESCRIPTOR_CDC 1
#define DESCRIPTOR_MSC 2
#define DESCRIPTOR_MULTI 3
#define DESCRIPTOR_TOGGLE 4

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t USB_Connected;

uint8_t USB_isConnected();
uint8_t USB_hasConnection();
void USB_Send(const uint8_t *data, uint16_t length);
uint16_t USB_Available();
uint8_t USB_Read();
uint16_t USB_ToSend();
uint8_t USB_Put(const uint8_t *data, uint16_t length);
uint16_t USB_Get(uint8_t *buff, uint16_t len);
void USB_ResetBuffers();
void USB_ChangeDescriptor(uint8_t descriptorClass);
void USB_Run(void *pvParameters);
void USB_Init2();
extern const xTaskParameters usbTaskDefinition;

#ifdef __cplusplus
}
#endif

#endif /* USB_H_ */
