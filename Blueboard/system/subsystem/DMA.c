/*
 * DMA.c
 *
 *  Created on: 26 Nov 2011
 *      Author: exuvo
 */

#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_gpdma.h"
#include "DMA.h"

// Terminal Counter
uint32_t chTC[8];
// Error Counter
uint32_t chErr[8];
DMA_Callback chCallback[8];

void DMA_Init(){
	GPDMA_Init();
	NVIC_SetPriority(DMA_IRQn, 16);
	NVIC_EnableIRQ(DMA_IRQn);
}

void DMA_RegisterCallback(uint8_t channel, DMA_Callback callback){
	if(callback != 0 && channel < 8){
		chCallback[channel] = callback;
	}
}

void DMA_IRQHandler(){
	for(uint8_t ch=0; ch < 8 ; ch++){
		if (GPDMA_IntGetStatus(GPDMA_STAT_INT, ch)){
			// Check counter terminal status
			if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, ch)){
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, ch);
				chTC[ch]++;
				if(chCallback[ch] != NULL){
					(*chCallback[ch])(GPDMA_STAT_INTTC, chTC[ch]);
				}
			}
			// Check error terminal status
			if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, ch)){
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, ch);
				chErr[ch]++;
				if(chCallback[ch] != NULL){
					(*chCallback[ch])(GPDMA_STAT_INTERR, chErr[ch]);
				}
			}
		}
	}
}

