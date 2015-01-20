/*
 * Radar.h
 *
 *  Created on: 13 Sep 2011
 *      Author: exuvo
 */

#ifndef SONAR_H_
#define SONAR_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"
#include "queue.h"

#define MINPOS 30
#define MAXPOS 1010

typedef struct {
	int16_t x, y;
}Ping;

extern xQueueHandle newPoints;

void Sonar_Run(void *pvParameters);
void Sonar_Init();
extern const xTaskParameters sonarTaskDefinition;

#ifdef __cplusplus
}
#endif
#endif /* RADAR_H_ */
