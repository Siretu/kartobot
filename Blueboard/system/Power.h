/*
 * Power.h
 *
 *  Created on: 4 Feb 2012
 *      Author: exuvo
 */

#ifndef POWER_H_
#define POWER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define AREF 3.30

#define POWER_SOURCE_NONE 0
#define POWER_SOURCE_USB 1
#define POWER_SOURCE_BATTERY 2
#define POWER_SOURCE_BATTERY_LOW 3
#define POWER_SOURCE_UNKNOWN 4

uint16_t Power_GetVoltage();
int16_t Power_GetCurrent();
uint8_t Power_GetSource();
void Power_Run(void *pvParameters);
void Power_Init();
extern const xTaskParameters powerTaskDefinition;

#ifdef __cplusplus
}
#endif
#endif /* POWER_H_ */
