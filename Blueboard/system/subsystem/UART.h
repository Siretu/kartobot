/*
 * UART.h
 *
 *  Created on: 5 Sep 2011
 *      Author: exuvo
 */

#ifndef UART_H_
#define UART_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"

void UARTS_Init();
void UART0_IRQHandler();
void UART1_IRQHandler();
void UART3_IRQHandler();
uint8_t UART0_send(const uint8_t *data, uint32_t length);
uint8_t UART1_send(const uint8_t *data, uint32_t length);
uint32_t UART1_sendForced(uint8_t *data, uint32_t length);
uint8_t UART3_send(const uint8_t *data, uint32_t length);
uint8_t UART0_sendS(const char *data);
uint8_t UART1_sendS(const char *data);
uint8_t UART3_sendS(const char *data);
uint8_t UART0_sendB(const uint8_t data);
uint8_t UART1_sendB(const uint8_t data);
uint8_t UART3_sendB(const uint8_t data);
uint16_t UART0_available();
uint16_t UART1_available();
uint16_t UART3_available();
uint8_t UART0_receive();
uint8_t UART1_receive();
uint8_t UART3_receive();

#ifdef __cplusplus
}
#endif
#endif /* UART_H_ */
