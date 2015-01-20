/*
 * I2C.c
 *
 *  Created on: 10 Sep 2011
 *      Author: exuvo
 */




#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "I2C.h"
#include "dbg.h"
#include "EEPROM.h"
#include "Time2.h"

#define WAIT 100
#define RETRIES 3

static xSemaphoreHandle I2C0_lock, I2C1_lock;

void I2CS_Init(){
	I2C0_lock = xSemaphoreCreateMutex();
	I2C1_lock = xSemaphoreCreateMutex();

	/* After an interruption in protocol, power loss or system reset, any two-wire
	 * part can be reset by following these steps: (a) Clock up to 9 cycles, (b) look for SDA high in
	 * each cycle while SCL is high and then (c) create a start condition as SDA is high.
	 */

	LPC_PINCON->PINMODE_OD0 |= (1 << 1) | (1 << 28);//Open drain
	LPC_GPIO0->FIOSET =  (1 << 1) | (1 << 28);//Default on
	LPC_GPIO0->FIODIR |= (1 << 1) | (1 << 28);//Enable outputs

	for(uint8_t i=0; i < 9; i++){//I2C0: 27 SDA, 28 SCL
		LPC_GPIO0->FIOSET = (1 << 28);
		Time_Delay(3);
		if(LPC_GPIO0->FIOPIN & (1 << 27)){
			break;
		}
		LPC_GPIO0->FIOCLR = (1 << 28);
		Time_Delay(3);
	}

	for(uint8_t i=0; i < 9; i++){//I2C1: 0 SDA, 1 SCL
		LPC_GPIO0->FIOSET = (1 << 1);
		Time_Delay(3);
		if(LPC_GPIO0->FIOPIN & (1 << 0)){
			break;
		}
		LPC_GPIO0->FIOCLR = (1 << 1);
		Time_Delay(3);
	}

	LPC_GPIO0->FIOCLR = (1 << 1) | (1 << 28);
	LPC_GPIO0->FIODIR &= ~((1 << 1) | (1 << 28));//Disable outputs

	{//EEPROM on this bus
		I2C_Init(LPC_I2C0, 400000);
		NVIC_SetPriority(I2C0_IRQn, 20);
		I2C_Cmd(LPC_I2C0, ENABLE);

		I2C_OWNSLAVEADDR_CFG_Type t;
		t.SlaveAddrChannel = 0;
		t.SlaveAddr_7bit = 3;
		t.GeneralCallState = FALSE;
		t.SlaveAddrMaskValue = 0;
		I2C_SetOwnSlaveAddr(LPC_I2C0, &t);

		LPC_PINCON->I2CPADCFG = 0;//PINSEL_SetI2C0Pins(PINSEL_I2C_Normal_Mode, ENABLE);

		PINSEL_CFG_Type pin;
		pin.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
		pin.Pinmode = PINSEL_PINMODE_TRISTATE;
		pin.Funcnum = 1;
		pin.Portnum = 0;
		pin.Pinnum = 27;
		PINSEL_ConfigPin(&pin);
		pin.Pinnum = 28;
		PINSEL_ConfigPin(&pin);
	}
	{
		I2C_Init(LPC_I2C1, 100000);
		NVIC_SetPriority(I2C1_IRQn, 20);
		I2C_Cmd(LPC_I2C1, ENABLE);

		I2C_OWNSLAVEADDR_CFG_Type t;
		t.SlaveAddrChannel = 0;
		t.SlaveAddr_7bit = 3;
		t.GeneralCallState = FALSE;
		t.SlaveAddrMaskValue = 0;
		I2C_SetOwnSlaveAddr(LPC_I2C1, &t);

		PINSEL_CFG_Type pin;
		pin.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
		pin.Pinmode = PINSEL_PINMODE_TRISTATE;
		pin.Funcnum = 3;
		pin.Portnum = 0;
		pin.Pinnum = 0;
		PINSEL_ConfigPin(&pin);
		pin.Pinnum = 1;
		PINSEL_ConfigPin(&pin);
	}

}

void I2C0_IRQHandler(){
    I2C_MasterHandler(LPC_I2C0);
}

void I2C1_IRQHandler(){
	//TODO handle slave calls
	I2C_MasterHandler(LPC_I2C1);
//	I2C_SlaveHandler(LPC_I2C1);
}

Status I2C0_sendS(I2C_M_SETUP_Type *TransferCfg){
	if(xSemaphoreTake(I2C0_lock, portMAX_DELAY) == pdTRUE){
		I2C_MasterTransferData(LPC_I2C0, TransferCfg, I2C_TRANSFER_INTERRUPT);
		uint32_t start = LPC_TIM0->TC;

		while(! I2C_MasterTransferComplete(LPC_I2C0)){
			taskYIELD();
			if(LPC_TIM0->TC - start > 2 * 20 * ((TransferCfg->tx_length + 1) * (TransferCfg->retransmissions_max + 1) + TransferCfg->rx_length)){
				NVIC_DisableIRQ(I2C0_IRQn);
				I2C_Stop(LPC_I2C0);
				break;
			}
		}
		xSemaphoreGive(I2C0_lock);
	}

	return TransferCfg->status & I2C_SETUP_STATUS_DONE ? SUCCESS : ERROR;
}

Status I2C1_sendS(I2C_M_SETUP_Type *TransferCfg){
	if(xSemaphoreTake(I2C1_lock, portMAX_DELAY) == pdTRUE){
		I2C_MasterTransferData(LPC_I2C1, TransferCfg, I2C_TRANSFER_INTERRUPT);
		uint32_t start = LPC_TIM0->TC;

		while(! I2C_MasterTransferComplete(LPC_I2C1)){
			taskYIELD();
			if(LPC_TIM0->TC - start > 2 * 80 * ((TransferCfg->tx_length + 1) * (TransferCfg->retransmissions_max + 1) + TransferCfg->rx_length)){
				NVIC_DisableIRQ(I2C1_IRQn);
				I2C_Stop(LPC_I2C1);
				break;
			}
		}
		xSemaphoreGive(I2C1_lock);
	}

	return TransferCfg->status & I2C_SETUP_STATUS_DONE ? SUCCESS : ERROR;
}

Status I2C0_send(uint8_t address, uint8_t *data, uint32_t length){
	I2C_M_SETUP_Type t;
	t.sl_addr7bit = address;
	t.tx_data = data;
	t.tx_length = length;
	t.rx_data = NULL;
	t.rx_length = 0;
	t.retransmissions_max = RETRIES;
	return I2C0_sendS(&t);
}

Status I2C1_send(uint8_t address, uint8_t *data, uint32_t length){
	I2C_M_SETUP_Type t;
	t.sl_addr7bit = address;
	t.tx_data = data;
	t.tx_length = length;
	t.rx_data = NULL;
	t.rx_length = 0;
	t.retransmissions_max = RETRIES;
	return I2C1_sendS(&t);
}

/*
 * TODO Add support for i2c enumeration.
 * Devices start in master mode and request an address from 0x00.
 * Host keeps a list of device type and address.
 */
void I2C1_receive(){
	//TODO stub
//	uint8_t data;
	while(1){
//		I2C_SlaveTransferComplete
	}
}

void I2C0_Test(){
	//I2C0 check
	uint8_t id;
	Status s = EEPROM_read(EEPROM_ID, &id, 1);
	check(s, "EEPROM communication error.");
	check(id == 0x50, "EEPROM id error.");

error:
	return;
}
