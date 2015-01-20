/*
 * Power.c
 *
 *  Created on: 4 Feb 2012
 *      Author: exuvo
 */

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "power.h"
#include "queue.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include <math.h>
#include "utils.h"
#include "Audio.h"
#include "XBee.h"
#include "term_io.h"
#include "motor.h"
#include "main.h"
#include "Display.h"
#include "USB.h"
#include "Diskio.h"

uint32_t powerFails;
const char Low_Battery_Path[] = "/stargate/SGA_midway_selfdestruct.wav";
const Song Low_Battery = {.type = A_WAV, .data = Low_Battery_Path, .loops = 255};

static uint16_t voltage;//100 * V
static int16_t current;//10 * A
static uint8_t source;

static portSTACK_TYPE powerTaskStack[96];
const xTaskParameters powerTaskDefinition =
{
		Power_Run,   							/* pvTaskCode */
		(signed char *) "power",  /* pcName */
    96,             					/* usStackDepth - defined in words, not bytes. */
    NULL,            					/* pvParameters */
    5 | portPRIVILEGE_BIT,    //TODO fix: Seems to have NO memory access at all in user mode								/* uxPriority */
    powerTaskStack,      			/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

static void Bar(uint16_t value){
	if(!(Audio_Playing()) || (Audio_CurrentSong().type == Low_Battery.type && Audio_CurrentSong().data == Low_Battery.data)){
		Display_SetBar(value);
	}
}

void Power_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastPowerSend = xLastWakeTime;
	portTickType xLastVGet = xLastWakeTime;
	uint8_t ch = 0;
	uint8_t blink = 0;

	while(1){
		if(xLastWakeTime - xLastVGet > 1000){
			if(voltage == 0){
//				xputs("No battery supply\n");
				source = POWER_SOURCE_NONE;
			}else if(USB_hasConnection() && voltage > 4.4 * 100 && voltage < 5.2 * 100){
				source = POWER_SOURCE_USB;
				Bar(0b1100000011);
			}else if(voltage < 3.3 * 2 * 100){
				if(Audio_Playing()){
					if((Audio_CurrentSong().type != Low_Battery.type || Audio_CurrentSong().data != Low_Battery.data)){
						Audio_Stop();
						Audio_PlaySong(Low_Battery);
					}
				}else{
					if( !(disk_status(0) & STA_NOINIT) ){
						Audio_PlaySong(Low_Battery);
					}
				}

				xprintf("LOW VOLTAGE %d\n", voltage);
				source = POWER_SOURCE_BATTERY_LOW;
				if(blink){
					Bar(0b0000000000);
					blink = 0;
				}else{
					Bar(barMap[map(voltage, 600, 840, 0, 10)]);
					blink = 1;
				}
			}else if(voltage >= 3.3 * 2 * 100){
				source = POWER_SOURCE_BATTERY;
				Bar(barMap[map(voltage, 600, 840, 0, 10)]);
			}else{
				source = POWER_SOURCE_UNKNOWN;
				Bar(0b0010000100);
			}

			if(source == POWER_SOURCE_BATTERY){
				if(!Motor_Enabled()){
					Motor_Enable();
				}
			}else{
				if(Motor_Enabled()){
					Motor_Disable();
				}
			}

			if(source == POWER_SOURCE_BATTERY || source == POWER_SOURCE_USB){
				if(FlashRateGet() == FLASH_FATAL){//TODO check flash reason?
					FlashNormal();
				}

				if(Audio_Playing() && Audio_CurrentSong().type == Low_Battery.type && Audio_CurrentSong().data == Low_Battery.data){
					Audio_Stop();
				}
			}else{
				if(FlashRateGet() == FLASH_NORMAL){
					FlashFatal();
				}
			}
			xLastVGet = xLastWakeTime;
		}

		if(xLastWakeTime - xLastPowerSend > (USB_isConnected() ? 100 : 250)){
			uint8_t d[] = {'b', voltage >> 8, voltage, current >> 8, current, source};
			XBee_Send(d, sizeof(d));
			xLastPowerSend = xLastWakeTime;
		}

		if(ch == 0){
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, DISABLE);
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);
			ch = 2;
		}else if(ch == 2){
//			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, DISABLE);//Seems bugged, CH2 does not work with this line
			ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
			ch = 0;
		}
		ADC_StartCmd(LPC_ADC, ADC_START_NOW);

		vTaskDelayUntil(&xLastWakeTime, 10);
	}
}

void Power_Init(){
//	P1.31 AD0.5 seems broken, 50Ohm to ground when PINSEL is configured. UPDATE: Func should be 3 schematic is wrong

	ADC_Init(LPC_ADC, 200000);
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, ENABLE);

	//Config P0.23, P0.25 as AD 0.0, 0.2
	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_TRISTATE;
	pin.Funcnum = 1;
	pin.Portnum = 0;
	pin.Pinnum = 23;
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = 25;
	PINSEL_ConfigPin(&pin);

	NVIC_SetPriority(ADC_IRQn, 22);
	NVIC_EnableIRQ(ADC_IRQn);
}

////TODO http://gvworks.blogspot.com/2011/03/lpc1768-adc-has-too-many-glitches.html
void ADC_IRQHandler(void){
	uint32_t gdr = LPC_ADC->ADGDR;
	if(gdr & ADC_GDR_OVERRUN_FLAG){
		powerFails++;
	}

	if(gdr & ADC_GDR_DONE_FLAG){
		int16_t value = ADC_GDR_RESULT(gdr);
		int8_t ch = ADC_GDR_CH(gdr);

		if(ch == 2){
			//RC filter 106KHz cutoff, 220pF 6.8kOhm
			static float f1 = 0;
			//TODO calibrate
			f1 = smooth((10 * ((value + 10 - 0x7FF) / 4095.0 * AREF / 0.040)), f1, 0.1);//40mV / Ampere
//			f = smooth(1000 * ((value + 10) / 4095.0 * AREF), f, 0.1);//Correct voltage measurement, 0.0008V / LSB
//			v.current = value;
			current = f1 - 6.5f;//Offset

		}else if(ch == 0){
			//Measured 7,63 2,78 = 0.3644    4,45 1,61 = 0.3618
			//Voltage Divider R1=6650, R2=3840, Vin=9V, Vout=3.295V
			//RC filter 111KHz cutoff, 215pF R1
			//Expected resolution 0.00220V/LSb
			static float f2 = 0;
			f2 = smooth((100 * ((value + 0) / 4095.0 * AREF / 0.3630)), f2, 0.2);//Theoretical Voltage Divider ratio 0.3661
//			v.voltage = value;
			voltage = f2 - 0.0f;//Offset
		}
	}
}

uint16_t Power_GetVoltage(){
	return voltage;
}

int16_t Power_GetCurrent(){
	return current;
}

uint8_t Power_GetSource(){
	return source;
}
