/*
 * Audio.c
 *
 *  Created on: 26 Nov 2011
 *      Author: exuvo
 */

#include "LPC17xx.h"
#include "lpc_types.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_clkpwr.h"
#include "Audio.h"
#include "DMA.h"
#include "utils.h"
#include "term_io.h"
#include <string.h>
#include "chiptune.h"
#include "code.h"
#include "wav.h"
#include "mpc.h"
#include "Display.h"
#include "Power.h"

#define PLAYINGSIZE 256U
#define BUFFERSIZE 512U
#define DEFAULT_FREQ 8000U //24000

//Code
//http://pelulamu.net/countercomplex/music_formula_collection.txt

//WAV
//https://github.com/olleolleolle/wav2c

__attribute__((section(".ram1"))) static __attribute__((aligned(4))) uint32_t buffer2[PLAYINGSIZE];
__attribute__((section(".ram1"))) static uint16_t buffer[BUFFERSIZE];

static volatile uint16_t w;
static volatile uint16_t r;
volatile uint32_t audioUnderrun;

static GPDMA_Channel_CFG_Type DMAConfig;

uint8_t playing;
uint32_t position, length;
void *resources;

Song currentSong;

static xQueueHandle Audio_next;

static void Audio_Enable(){
	GPDMA_ChannelCmd(0, DISABLE);

	if(GPDMA_Setup(&DMAConfig, GPDMA_DO_INCREMENT) == SUCCESS){
		GPDMA_ChannelCmd(0, ENABLE);
	}else{
		xprintfD("Audio_Enable: Failed");
	}
}

static void Audio_Disable(){
	GPDMA_ChannelCmd(0, DISABLE);
	LPC_DAC->DACR = DAC_VALUE(0);
}

static void Audio_Callback(GPDMA_Status_Type status, uint32_t counter){
	if(status == GPDMA_STAT_INTTC){

		if((uint16_t)(w-r) >= PLAYINGSIZE){
			uint16_t i=0;
			while(w-r != 0U && i < PLAYINGSIZE){
					buffer2[i++] = DAC_VALUE(buffer[r++ % BUFFERSIZE]);// | DAC_BIAS_EN;
			}
		}else{
			audioUnderrun++;
		}

		if(playing){
			Audio_Enable();
		}else{
			Audio_Disable();
		}
	}else if(status == GPDMA_STAT_INTERR){
		Audio_Disable();
		xprintfD("Audio_Callback: GPDMA_STAT_INTERR %lu", counter);
	}
}

void Audio_SetFrequency(uint32_t hz){
	DAC_SetDMATimeOut(LPC_DAC, CLKPWR_GetPCLK(CLKPWR_PCLKSEL_DAC) / (hz));
}

void Audio_Init(){
	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_TRISTATE;
	pin.Funcnum = 2;
	pin.Portnum = 0;
	pin.Pinnum = 26;
	PINSEL_ConfigPin(&pin);//AOUT on P0.26

	DAC_Init(LPC_DAC);
	Audio_SetFrequency(DEFAULT_FREQ);
	DAC_CONVERTER_CFG_Type DACConfig;
		DACConfig.DBLBUF_ENA = 1;
		DACConfig.CNT_ENA = 1;
		DACConfig.DMA_ENA = 1;
		DACConfig.RESERVED = 0;
	DAC_ConfigDAConverterControl(LPC_DAC, &DACConfig);

	DMAConfig.ChannelNum = 0;
	DMAConfig.SrcMemAddr = (uint32_t) buffer2;
	DMAConfig.DstMemAddr = 0;// unused
	DMAConfig.TransferWidth = 0;// unused
	DMAConfig.TransferType = GPDMA_TRANSFERTYPE_M2P;
	DMAConfig.TransferSize = PLAYINGSIZE;
	DMAConfig.SrcConn = 0;// unused
	DMAConfig.DstConn = GPDMA_CONN_DAC;
	DMAConfig.DMALLI = 0;

	DMA_RegisterCallback(0, &Audio_Callback);
	Audio_next = xQueueCreate(8, sizeof(Song));
	Audio_Disable();
}

static void write(uint16_t data){
	while((uint16_t)(w-r) == BUFFERSIZE){
		vTaskDelay(5);//At 48kHz it will take 10ms for the buffer(256, 512) to become empty
	}
	buffer[w++ % BUFFERSIZE] = data;
}

void Audio_Stop(){
	playing = 0;
}

uint8_t Audio_PlaySong(Song song){
	if(xQueueSendToBack(Audio_next, &song, 0) == pdPASS ){
		return pdPASS;
	}else{
		return pdFAIL;
	}
}

uint8_t Audio_Playing(){
	return playing;
}

const Song Audio_CurrentSong(){
	return (const Song) currentSong;
}

static void Audio_play(const Song song){
	position = length = 0;
	currentSong = song;

	if(currentSong.type == A_CHIPTUNE){
		Audio_Chiptune_Setup();
	}else if(currentSong.type == A_CODE){
		Audio_Code_Setup();
	}else if(currentSong.type == A_WAV){
		Audio_WAV_Setup();
	}else if(currentSong.type == A_MPC){
		Audio_MPC_Setup();
	}

	if(playing){
		w = 0;
		r = 0;
		memset (buffer, 0, sizeof(buffer));
		memset (buffer2, 0, sizeof(buffer2));
		Audio_Enable();
	}
}

void Audio_Play(void *pvParameters){
	portTickType xLastBarUpdate = xTaskGetTickCount();

	while (1) {
		if(playing){
			uint16_t d;
			if(currentSong.type == A_CHIPTUNE){
				d = Audio_Chiptune_Play();
			}else if(currentSong.type == A_CODE){
				d = Audio_Code_Play();
			}else if(currentSong.type == A_WAV){
				d = Audio_WAV_Play();
			}else if(currentSong.type == A_MPC){
				d = Audio_MPC_Play();
			}
			if(playing){
				write(d);

				if(xTaskGetTickCount() - xLastBarUpdate > 100 && length != 0 && Power_GetSource() != POWER_SOURCE_BATTERY_LOW){
					Display_SetBar(barMap[1 + (uint8_t) mapD( (float)position / (float)length, 0, 1, 0, 10) ]);
					xLastBarUpdate = xTaskGetTickCount();
				}
			}
		}else{
			if(currentSong.type != A_NOTHING){
				switch(currentSong.type){
					case A_CHIPTUNE:
						Audio_Chiptune_Cleanup();
						break;
					case A_WAV:
						Audio_WAV_Cleanup();
						break;
					case A_CODE:
						Audio_Code_Cleanup();
						break;
					case A_MPC:
						Audio_MPC_Cleanup();
						break;
				}
				currentSong.type = A_NOTHING;
			}

			Song song;
			if(xQueueReceive(Audio_next, &song, 0) == pdPASS){
				Audio_play(song);
			}else{
				vTaskDelay(100 / portTICK_RATE_MS);
			}
		}
	}
}

portSTACK_TYPE AudioTaskStack[256];;
const xTaskParameters AudioTaskDefinition =
{
	Audio_Play,   				/* pvTaskCode */
	(signed char *) "audio",  	/* pcName */
    256,             			/* usStackDepth - defined in words, not bytes. */
    NULL,          				/* pvParameters */
    4| portPRIVILEGE_BIT,		/* uxPriority */
    AudioTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};


