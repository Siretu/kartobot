/*
 * Display.h
 *
 *  Created on: 28 Mar 2012
 *      Author: exuvo
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define BLACK 	0
#define GREEN 	1
#define RED 		2
#define YELLOW 	3

#ifdef __cplusplus
extern "C" {
#endif

extern const uint16_t barMap[11];

void Display_Clear();
void Display_SetPixel(uint8_t x, uint8_t y, uint8_t color);
void Display_SetBar(uint16_t value);
void Display_FaultError(uint32_t codePos, uint32_t memPos, uint16_t faultType);
void Display_Run(void *pvParameters);
extern const xTaskParameters displayTaskDefinition;

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */
