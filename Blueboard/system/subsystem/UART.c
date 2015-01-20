/*
 * UART.c
 *
 *  Created on: 5 Sep 2011
 *      Author: exuvo
 */

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "UART.h"
#include <string.h>

#define UART_IN_SIZE 	128U
#define UART_OUT_SIZE	256U

#define UART0_IN_SIZE 	UART_IN_SIZE
static uint8_t UART0_in[UART0_IN_SIZE];
static volatile uint16_t UART0_inW;
static volatile uint16_t UART0_inR;

#define UART1_IN_SIZE 	UART_IN_SIZE
static uint8_t UART1_in[UART1_IN_SIZE];
static volatile uint16_t UART1_inW;
static volatile uint16_t UART1_inR;

#define UART3_IN_SIZE 	UART_IN_SIZE
static uint8_t UART3_in[UART3_IN_SIZE];
static volatile uint16_t UART3_inW;
static volatile uint16_t UART3_inR;

#define UART0_OUT_SIZE 	UART_OUT_SIZE
static uint8_t UART0_out[UART0_OUT_SIZE];
static volatile uint16_t UART0_outW;
static volatile uint16_t UART0_outR;

#define UART1_OUT_SIZE 	UART_OUT_SIZE
static uint8_t UART1_out[UART1_OUT_SIZE];
static volatile uint16_t UART1_outW;
static volatile uint16_t UART1_outR;

#define UART3_OUT_SIZE 	UART_OUT_SIZE
static uint8_t UART3_out[UART3_OUT_SIZE];
static volatile uint16_t UART3_outW;
static volatile uint16_t UART3_outR;

static xSemaphoreHandle UART0_lock, UART1_lock, UART3_lock;

#define WAIT portMAX_DELAY

void UARTS_Init(){
	UART0_lock = xSemaphoreCreateMutex();
	UART1_lock = xSemaphoreCreateMutex();
	UART3_lock = xSemaphoreCreateMutex();

	{
		PINSEL_CFG_Type pin;
		pin.OpenDrain = PINSEL_PINMODE_NORMAL;
		pin.Pinmode = PINSEL_PINMODE_PULLUP;
		pin.Funcnum = 1;
		pin.Portnum = 0;
		pin.Pinnum = 2;
		PINSEL_ConfigPin(&pin);
		pin.Pinnum = 3;
		PINSEL_ConfigPin(&pin);

		UART_CFG_Type t;
		t.Baud_rate = 9600;
		t.Parity = UART_PARITY_NONE;
		t.Databits = UART_DATABIT_8;
		t.Stopbits = UART_STOPBIT_1;
		UART_Init(LPC_UART0, &t);
		UART_FIFO_CFG_Type f;
		f.FIFO_ResetRxBuf = ENABLE;
		f.FIFO_ResetTxBuf = ENABLE;
		f.FIFO_DMAMode = DISABLE;
		f.FIFO_Level = UART_FIFO_TRGLEV2; //Interrupt when 8 bytes are ready
		UART_FIFOConfig(LPC_UART0, &f);
		UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
		UART_IntConfig(LPC_UART0, UART_INTCFG_RLS, ENABLE);
		NVIC_SetPriority(UART0_IRQn, 19);
		NVIC_EnableIRQ(UART0_IRQn);
		UART_TxCmd(LPC_UART0, ENABLE);
	}
	{
		PINSEL_CFG_Type pin;
		pin.OpenDrain = PINSEL_PINMODE_NORMAL;
		pin.Pinmode = PINSEL_PINMODE_PULLUP;
		pin.Funcnum = 2;
		pin.Portnum = 2;
		pin.Pinnum = 0;
		PINSEL_ConfigPin(&pin);//P2.0 TXD1
		pin.Pinnum = 1;
		PINSEL_ConfigPin(&pin);//P2.1 RXD1
		pin.Pinnum = 2;
		PINSEL_ConfigPin(&pin);//P2.2 (PWM1.3) CTS1
		pin.Pinnum = 7;
		PINSEL_ConfigPin(&pin);//P2.7 RTS1

		UART_CFG_Type t;
		t.Baud_rate = 111111;	//Actual XBee speed is 111111, not as advertised 115200
		t.Parity = UART_PARITY_NONE;
		t.Databits = UART_DATABIT_8;
		t.Stopbits = UART_STOPBIT_1;
		UART_Init((LPC_UART_TypeDef *)LPC_UART1, &t);
		UART_FIFO_CFG_Type f;
		f.FIFO_ResetRxBuf = ENABLE;
		f.FIFO_ResetTxBuf = ENABLE;
		f.FIFO_DMAMode = DISABLE;
		f.FIFO_Level = UART_FIFO_TRGLEV2; //Interrupt when 8 bytes are ready
		UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &f);
		// Configure UART1 hardware flow control RTS/CTS
		UART_FullModemConfigMode((LPC_UART1_TypeDef *)LPC_UART1, UART1_MODEM_MODE_AUTO_CTS, ACTIVE);
		UART_FullModemConfigMode((LPC_UART1_TypeDef *)LPC_UART1, UART1_MODEM_MODE_AUTO_RTS, ACTIVE);
		UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);
		UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_RBR, ENABLE);
		UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_RLS, ENABLE);
		NVIC_SetPriority(UART1_IRQn, 19);
		NVIC_EnableIRQ(UART1_IRQn);
	}
	{
		PINSEL_CFG_Type pin;
		pin.OpenDrain = PINSEL_PINMODE_NORMAL;
		pin.Pinmode = PINSEL_PINMODE_PULLUP;
		pin.Funcnum = 3;
		pin.Portnum = 4;
		pin.Pinnum = 28;
		PINSEL_ConfigPin(&pin);
		pin.Pinnum = 29;
		PINSEL_ConfigPin(&pin);

		UART_CFG_Type t;
		t.Baud_rate = 9600;
		t.Parity = UART_PARITY_NONE;
		t.Databits = UART_DATABIT_8;
		t.Stopbits = UART_STOPBIT_1;
		UART_Init(LPC_UART3, &t);
		UART_FIFO_CFG_Type f;
		f.FIFO_ResetRxBuf = ENABLE;
		f.FIFO_ResetTxBuf = ENABLE;
		f.FIFO_DMAMode = DISABLE;
		f.FIFO_Level = UART_FIFO_TRGLEV2; //Interrupt when 8 bytes are ready
		UART_FIFOConfig(LPC_UART3, &f);
		UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);
		UART_IntConfig(LPC_UART3, UART_INTCFG_RLS, ENABLE);
		NVIC_EnableIRQ(UART3_IRQn);
		NVIC_SetPriority(UART3_IRQn, 19);
		UART_TxCmd(LPC_UART3, ENABLE);
	}
}

void UART0_IRQHandler(){
	uint32_t intsrc, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART0) & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (intsrc == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART0);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
			//TODO warn UART_IntErr(tmp1);
		}
	}else

	// Receive Data Available or Character time-out
	if ((intsrc == UART_IIR_INTID_RDA) || (intsrc == UART_IIR_INTID_CTI)){
		uint8_t data[16];
		uint32_t length = UART_Receive(LPC_UART0, data, 16, NONE_BLOCKING);
		for(uint8_t i=0; i < length; i++){
			if((uint16_t)(UART0_inW - UART0_inR) != UART0_IN_SIZE){
				UART0_in[UART0_inW++ % UART0_IN_SIZE] = data[i];
			}else{
				break;
			}
		}
	}else

	// Transmit Holding Empty
	if (intsrc == UART_IIR_INTID_THRE){
		while (LPC_UART0->LSR & UART_LSR_THRE){
			if((uint16_t)(UART0_outW - UART0_outR) != 0){
				UART_SendByte(LPC_UART0, UART0_out[UART0_outR++ % UART0_OUT_SIZE]);
			}else{
				UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, DISABLE);
				break;
			}

		}
	}
}

void UART1_IRQHandler(){
	uint32_t intsrc, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId((LPC_UART_TypeDef *)LPC_UART1) & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (intsrc == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef *)LPC_UART1);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
			//TODO warn UART_IntErr(tmp1);
		}
	}else

	// Receive Data Available or Character time-out
	if ((intsrc == UART_IIR_INTID_RDA) || (intsrc == UART_IIR_INTID_CTI)){
		uint8_t data[16];
		uint32_t length = UART_Receive((LPC_UART_TypeDef *)LPC_UART1, data, 16, NONE_BLOCKING);
		for(uint8_t i=0; i < length; i++){
			if((uint16_t)(UART1_inW - UART1_inR) != UART1_IN_SIZE){
				UART1_in[UART1_inW++ % UART1_IN_SIZE] = data[i];
			}else{
				break;
			}
		}
	}else

	// Transmit Holding Empty
	if (intsrc == UART_IIR_INTID_THRE){
		while (LPC_UART1->LSR & UART_LSR_THRE){
			if((uint16_t)(UART1_outW - UART1_outR) != 0){
				UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, UART1_out[UART1_outR++ % UART1_OUT_SIZE]);
			}else{
				UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, DISABLE);
				break;
			}
}
	}
}

void UART3_IRQHandler(){
	uint32_t intsrc, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART3) & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (intsrc == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART3);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
			//TODO warn UART_IntErr(tmp1);
		}
	}else

	// Receive Data Available or Character time-out
	if ((intsrc == UART_IIR_INTID_RDA) || (intsrc == UART_IIR_INTID_CTI)){
		uint8_t data[16];
		uint32_t length = UART_Receive(LPC_UART3, data, 16, NONE_BLOCKING);
		for(uint8_t i=0; i < length; i++){
			if((uint16_t)(UART3_inW - UART3_inR) != UART3_IN_SIZE){
				UART3_in[UART3_inW++ % UART3_IN_SIZE] = data[i];
			}else{
				break;
			}
		}
	}else

	// Transmit Holding Empty
	if (intsrc == UART_IIR_INTID_THRE){
		while (LPC_UART3->LSR & UART_LSR_THRE){
			if((uint16_t)(UART3_outW - UART3_outR) != 0){
				UART_SendByte(LPC_UART3, UART3_out[UART3_outR++ % UART3_OUT_SIZE]);
			}else{
				UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, DISABLE);
				break;
			}
		}
	}
}

uint8_t UART0_send(const uint8_t *data, uint32_t length){
	uint8_t r = FALSE;
	if(xSemaphoreTake(UART0_lock, WAIT) == pdTRUE){
		if(! (LPC_UART0->IER & UART_IER_THREINT_EN)){
			while ((LPC_UART0->LSR & UART_LSR_THRE) && length > 0){
				UART_SendByte(LPC_UART0, *data++);
				length--;
			}
		}
		while(length > 0){
			while((uint16_t)(UART0_outW - UART0_outR) == UART0_OUT_SIZE){
				vTaskDelay(1 / portTICK_RATE_MS);
			}
			UART0_out[UART0_outW++ % UART0_OUT_SIZE] = *data++;
			length--;
		}
		UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);
		xSemaphoreGive(UART0_lock);
		r = TRUE;
	}
	return r;
}

uint8_t UART1_send(const uint8_t *data, uint32_t length){
	uint8_t r = FALSE;
	if(xSemaphoreTake(UART1_lock, WAIT) == pdTRUE){
		if(! (LPC_UART1->IER & UART_IER_THREINT_EN)){
			while ((LPC_UART1->LSR & UART_LSR_THRE) && length > 0){
				UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, *data++);
				length--;
			}
		}
		while(length > 0){
			while((uint16_t)(UART1_outW - UART1_outR) == UART1_OUT_SIZE){
				vTaskDelay(1 / portTICK_RATE_MS);
			}
			UART1_out[UART1_outW++ % UART1_OUT_SIZE] = *data++;
			length--;
		}
		UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, ENABLE);
		xSemaphoreGive(UART1_lock);
		r = TRUE;
	}
	return r;
}

uint32_t UART1_sendForced(uint8_t *data, uint32_t length){
	return UART_Send((LPC_UART_TypeDef *)LPC_UART1, data, length, BLOCKING);
}

uint8_t UART3_send(const uint8_t *data, uint32_t length){
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
	uint8_t r = FALSE;
	if(xSemaphoreTake(UART3_lock, WAIT) == pdTRUE){
		if(! (LPC_UART3->IER & UART_IER_THREINT_EN)){
			while ((LPC_UART3->LSR & UART_LSR_THRE) && length > 0){
				UART_SendByte(LPC_UART3, *data++);
				length--;
			}
		}
		while(length > 0){
			while((uint16_t)(UART3_outW - UART3_outR) == UART3_OUT_SIZE){
				vTaskDelay(1 / portTICK_RATE_MS);
			}
			UART3_out[UART3_outW++ % UART3_OUT_SIZE] = *data++;
			length--;
		}
		UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, ENABLE);
		xSemaphoreGive(UART3_lock);
		r = TRUE;
	}
	portRESET_PRIVILEGE(xRunningPrivileged);
	return r;
}

uint8_t UART0_sendS(const char *data){
	return UART0_send((const uint8_t*) data, (uint32_t) strlen(data));
}

uint8_t UART1_sendS(const char *data){
	return UART1_send((const uint8_t*) data, (uint32_t) strlen(data));
}

uint8_t UART3_sendS(const char *data){
	return UART3_send((const uint8_t*) data, (uint32_t) strlen(data));
}

uint8_t UART0_sendB(const uint8_t data){
	return UART0_send(&data, 1);
}

uint8_t UART1_sendB(const uint8_t data){
	return UART1_send(&data, 1);
}

uint8_t UART3_sendB(const uint8_t data){
	return UART3_send(&data, 1);
}

uint16_t UART0_available(){
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
	uint16_t r = UART0_inW - UART0_inR;
	portRESET_PRIVILEGE(xRunningPrivileged);
	return r;
}

uint16_t UART1_available(){
	return UART1_inW - UART1_inR;
}

uint16_t UART3_available(){
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
	uint16_t r = UART3_inW - UART3_inR;
	portRESET_PRIVILEGE(xRunningPrivileged);
	return r;
}

uint8_t UART0_receive(){
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
	uint8_t data = 0;
	if((uint16_t)(UART0_inW - UART0_inR) != 0){
		data = UART0_in[UART0_inR++ % UART0_IN_SIZE];
	}
	portRESET_PRIVILEGE(xRunningPrivileged);
	return data;
}

uint8_t UART1_receive(){
	uint8_t data = 0;
	if((uint16_t)(UART1_inW - UART1_inR) != 0){
		data = UART1_in[UART1_inR++ % UART1_IN_SIZE];
	}
	return data;
}

uint8_t UART3_receive(){
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
	uint8_t data = 0;
	if((uint16_t)(UART3_inW - UART3_inR) != 0){
		data = UART3_in[UART3_inR++ % UART3_IN_SIZE];
	}
	portRESET_PRIVILEGE(xRunningPrivileged);
	return data;
}


