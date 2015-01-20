/*
 * Positioning.h
 *
 *  Created on: 13 Jan 2012
 *      Author: exuvo
 */

#ifndef POSITION_H_
#define POSITION_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "PID.h"

typedef struct {
	uint16_t x, y;
}Point;

//Orders
#define POS_STOP 0
#define POS_FORWARD 1
#define POS_BACK 2
#define POS_ROTATE 3

void Position_Init();
void Position_Run(void *pvParameters);
void Position_GetXY(Point *p);
void Position_FaceAngle(double newAngle);
void Position_GoTo(Point p);
void Position_StopMoving();
void Position_CanMove(uint8_t boolean);
void Position_SetSpeed(double newSpeed);
void Position_SetPos(Point p);
extern const xTaskParameters positionTaskDefinition;

#ifdef __cplusplus
}
#endif
#endif /* POSITION_H_ */
