/*
 * Radar.h
 *
 *  Created on: 13 Sep 2011
 *      Author: exuvo
 */

#ifndef NAVIGATION_H_
#define NAVIGATION_H_

#include "Position.h"

typedef struct {
	uint8_t x, y;
}Square;


#define GRIDSIZE 64
#define SETSIZE 1638

extern __attribute__((section(".ram2"))) uint8_t grid[GRIDSIZE][GRIDSIZE];

#ifdef __cplusplus

extern "C" void Navigation_Exploring(uint8_t boolean);
extern "C" void Navigation_Pathing(uint8_t boolean);
extern "C" void Navigation_GoTo(Point goal);
extern "C" void Navigation_AddWall(Square wall);
extern "C" void Navigation_Run(void *pvParameters);
extern "C" void Navigation_Init();

#else

void Navigation_Moving(uint8_t boolean);
void Navigation_GoTo(Point goal);
void Navigation_AddWall(Square wall);
void Navigation_Init();
extern const xTaskParameters navigationTaskDefinition;

#endif
#endif /* RADAR_H_ */


