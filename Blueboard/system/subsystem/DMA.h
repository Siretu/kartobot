/*
 * DMA.h
 *
 *  Created on: 26 Nov 2011
 *      Author: exuvo
 */

#ifndef DMA_H_
#define DMA_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc17xx_gpdma.h"

typedef void (*DMA_Callback)(GPDMA_Status_Type, uint32_t counter);


void DMA_Init();
void DMA_RegisterCallback(uint8_t channel, DMA_Callback);

#ifdef __cplusplus
}
#endif
#endif /* DMA_H_ */
