/*
 * Radar.c
 *
 *  Created on: 13 Sep 2011
 *      Author: exuvo
 */

#include <inttypes.h>
#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "navigation.h"
#include "sonar.h"
#include "queue.h"
#include <math.h>
#include "term_io.h"
#include "XBee.h"
#include "utils.h"
#include "Position.h"
#include "semphr.h"
#include "USB.h"
#include <string.h>
#include "dbg.h"

#define INT16_MIN 	-32768
#define INT16_MAX 	 32767
#define S_MAX		0xFF

#define LEFT -1
#define RIGHT 1

#define NORTH 0
#define SOUTH 2
#define WEST  1
#define EAST  3

#define PROBABILITY_OF_WALKING_THRU_WALLS 10

__attribute__((section(".ram2"))) uint32_t closed[GRIDSIZE*GRIDSIZE/32];
__attribute__((section(".ram2"))) uint32_t open[GRIDSIZE*GRIDSIZE/32];
__attribute__((section(".ram2"))) uint64_t came_from[GRIDSIZE*GRIDSIZE/32];

#define gridClear(g, i) 	(g[ (i)/32 ] &= ~ ((uint32_t) 1 << ( (uint32_t)(i) % 32) ) )
#define gridSet(g, i) 		(g[ (i)/32 ] |=   ((uint32_t) 1 << ( (uint32_t)(i) % 32) ) )
#define gridGet(g, i) 		(g[ (i)/32 ] &    ((uint32_t) 1 << ( (uint32_t)(i) % 32) ) )

#define setClosed(i) 		(gridSet(closed, (i)))
#define clearClosed(i) 	(gridClear(closed, (i)))
#define getClosed(i) 		(gridGet(closed, (i)))
#define setOpen(i) 			(gridSet(open, (i)))
#define clearOpen(i) 		(gridClear(open, (i)))
#define getOpen(i) 			(gridGet(open, (i)))

#define clearCameFrom(i) 	(came_from[ (i)/32 ] &= ~ ((uint64_t) 3 << (2 * ((uint32_t)(i) % 32) )) )
#define setCameFrom(i, v) (came_from[ (i)/32 ] |=   ((uint64_t)(v) << (2 * ((uint32_t)(i) % 32) )) )
#define getCameFrom(i) 		( (came_from[ (i)/32 ] >> (2 * ((uint32_t)(i) % 32) )) & 3 )

#define toI(x, y) ((x) + (y) * GRIDSIZE)
#define toIs(s) toI( (s).x, (s).y)
#define toGx(i)	((i) % GRIDSIZE)
#define toGy(i)	((i) / GRIDSIZE)
#define toGs(s, i)	((s).x = toGx(i), (s).y = toGy(i))

__attribute__((section(".ram2"))) uint8_t g_score[GRIDSIZE * GRIDSIZE];//Also used to save path
__attribute__((section(".ram2"))) uint8_t f_score[GRIDSIZE * GRIDSIZE];//Also used as buffer for path transmission
__attribute__((section(".ram2"))) uint8_t grid[GRIDSIZE][GRIDSIZE];

static Point position;
static Point destination;
static xSemaphoreHandle Nav_lock;
static uint8_t exploring = FALSE;
static uint8_t pathing = FALSE;

static portSTACK_TYPE navigationTaskStack[1024];
extern "C" const xTaskParameters navigationTaskDefinition =
{
		Navigation_Run,   							/* pvTaskCode */
		(signed char *) "navigation",  	/* pcName */
		1024,             								/* usStackDepth - defined in words, not bytes. */
		NULL,            								/* pvParameters */
		2 | portPRIVILEGE_BIT,      		/* uxPriority */
		navigationTaskStack,      			/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

extern "C" void Navigation_Exploring(uint8_t boolean){
	exploring = boolean;
}

extern "C" void Navigation_Pathing(uint8_t boolean){
	pathing = boolean;
}

extern "C" void Navigation_GoTo(Point goal){
	if(xSemaphoreTake(Nav_lock, portMAX_DELAY) == pdTRUE){

		if(goal.x > 1279)goal.x = 1279;
		if(goal.y > 1279)goal.y = 1279;
		destination = goal;

		xSemaphoreGive(Nav_lock);
	}
}

extern "C" void Navigation_AddWall(Square wall){
	if(xSemaphoreTake(Nav_lock, portMAX_DELAY) == pdTRUE){

		grid[wall.x][wall.y] = 255;

		xSemaphoreGive(Nav_lock);
	}
}

static void clear(){
	for(uint16_t i=0; i < GRIDSIZE*GRIDSIZE/32; i++){
		open[i] = 0;
		closed[i] = 0;
		came_from[i] = 0;
	}

	for(uint16_t i=0; i < GRIDSIZE * GRIDSIZE; i++){
		f_score[i] = 0;
		g_score[i] = 0;
	}
}

static int16_t aStar(Square startPos, Square goal, Square *currPath);

extern "C" void Navigation_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastUpdate = xLastWakeTime;
	portTickType xLastSend = xLastWakeTime;
	portTickType xLastDecay = xLastWakeTime;
	Position_GetXY(&position);
	destination.x = position.x; destination.y = position.y;
	int8_t direction = 1;

	//	portSWITCH_TO_USER_MODE();

	while(1){
		if(xSemaphoreTake(Nav_lock, portMAX_DELAY) == pdTRUE){
			Position_GetXY(&position);//Coordinates are in centimetres while one square is one decimetres.

			{
				Ping p;
				while(xQueueReceive(newPoints, &p, 0) == pdPASS){
					p.x += position.x; p.y += position.y;
					uint8_t d[5] = {'s', p.x >> 8, p.x, p.y >> 8, p.y};
					XBee_Send(d, 5);

					if(p.x >= 0 && p.x <= GRIDSIZE*10 && p.y >= 0 && p.y <= GRIDSIZE*10 && grid[p.x/10][p.y/10] < 255){
						grid[p.x / 10][p.y / 10] += 1;
					}
				}
			}

			if(xLastWakeTime - xLastSend > 1000){
				xLastSend = xLastWakeTime;

				uint8_t d[72];
				d[0] = 'n';

				static uint8_t x = 0,y = 0;
				uint8_t i = 1;
				uint16_t c = 0;
				while(1){
					if(x++ == 63){x = 0;}
					while(1){
						if(y++ == 63){y = -1; break;}

						if(grid[x][y] != 0){
							d[i++] = x;
							d[i++] = y;
							d[i++] = grid[x][y];
							if(i == 70){//23st
								if(USB_isConnected()){
									USB_Send(d, i);
									i = 1;
								}else{
									goto send;
								}
							}
						}

						if(c++ == (GRIDSIZE * GRIDSIZE -1)){
							goto send;
						}
					}
				}
				send:
				if(i > 1){
					XBee_Send(d, i);
				}
			}

			if(xLastWakeTime - xLastDecay > 10000){
				xLastDecay = xLastWakeTime;
				for(uint8_t x = 0;x<GRIDSIZE;x++){
					for(uint8_t y = 0;y<GRIDSIZE;y++){
						if(grid[x][y] > 0 && grid[x][y] < 5){//Remove false positives
							grid[x][y]--;
						}
					}
				}
			}

			if(xLastWakeTime - xLastUpdate > 1000){
				xLastUpdate = xLastWakeTime;

				Square pos; pos.x = position.x / 10; pos.y = position.y / 10;
				Square goal; goal.x = destination.x / 10; goal.y = destination.y / 10;

				Square *path = (Square *) g_score;

				clear();
				portTickType time = xTaskGetTickCount();
				int16_t length = aStar(pos, goal, path);
				time = xTaskGetTickCount() - time;

				if(USB_isConnected()){
					uint8_t *d = (uint8_t*) f_score;

					d[0] = 'N';
					d[1] = time >> 8;
					d[2] = time;
					d[3] = length >> 8;
					d[4] = length;

					if(length == -1){
						length = 0;
					}else{
						length = (uint16_t)length > (sizeof(f_score) - 3) ? sizeof(f_score) -3 : length;
						memcpy(&d[5], path, length * sizeof(Square));
					}

					USB_Send(d, 5 + length * sizeof(Square));
				}else{
					uint8_t d[72];
					d[0] = 'N';
					d[1] = time >> 8;
					d[2] = time;
					d[3] = length >> 8;
					d[4] = length;
					uint8_t i=5;

					if(length != -1){
						for(uint8_t o=0; o < length && o < 22; o++){
							d[i++] = path[o].x;
							d[i++] = path[o].y;
						}
					}

					XBee_Send(d, i);
				}

				if(length != -1 && position.x != destination.x && position.y != destination.y){
					if(pathing){
						Point p;
						p.x = path[0].x * 10 + 5;
						p.y = path[0].y * 10 + 5;

						Position_GoTo(p);
					}
				} else if(exploring) {
					if(direction == 1){
						if(destination.x > 600){ // 600+600*direction = 0 or 1200
							if(destination.y > 600){
								xputs("Finished exploration without crashing. Awesome! This will probably never happen.\n");
							} else {
								destination.y += 50;
								direction = -direction;
							}
						} else {
							destination.x += 50 * direction;
						}
					} else if(direction == -1){
						if(destination.x == 0){ // 600+600*direction = 0 or 1200
							if(destination.y > 600){
								xputs("Finished exploration without crashing. Awesome! This will probably never happen.\n");
							} else {
								destination.y += 50;
								direction = -direction;
							}
						} else {
							destination.x += 50 * direction;
						}
					} else {
						xputs("Unspecified direction. Should not happen. Direction should be 1/-1\n");
					}
					xprintf("Changed position to %u,%u\n",destination.x, destination.y);
				}

			}

			xSemaphoreGive(Nav_lock);
		}

		vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_RATE_MS);
	}
}

static uint16_t h_estimate(Square start, Square end){
	return ABS(start.x - end.x) + ABS(start.y - end.y);

}

static uint16_t movementcost(Square to){
	uint16_t cost = 1;
	for(uint16_t i = to.x-1; i < to.x+2; i++){
		for(uint16_t j = to.y-1; j < to.y+2; j++){
			if(i > 0 && i < GRIDSIZE && j > 0 && j < GRIDSIZE){
				if(i == to.x && j == to.y){
					cost += grid[i][j] > 1 ? 1 : 0;
				} else {
					cost += grid[i][j] >= PROBABILITY_OF_WALKING_THRU_WALLS/2 ? 2 : 0;
				}
			}
		}
	}
	return cost;
}

static uint16_t getPrev(uint16_t ind){
	switch((uint32_t) getCameFrom(ind)){
		case NORTH:
			return ind + GRIDSIZE;
		case SOUTH:
			return ind - GRIDSIZE;
		case WEST:
			return ind +1;
		case EAST:
			return ind -1;
		default:
			debug("Invalid direction");
			vTaskSuspend(NULL);
			return -1;
	}
}

static int16_t findPath(uint16_t startPos, uint16_t currPos, Square *currPath){
//	xprintf("Path\n");
	uint16_t ind = currPos;
	uint16_t size = 0;

	while(ind != startPos){
		ind = getPrev(ind);
		size++;
//		if(size > 100){
//			debug("Unable to reconstruct path");
//			vTaskSuspend(NULL);
//		}
	}

	ind = currPos;
	uint16_t counter = size;

	while(ind != startPos){
		if(counter-- < (GRIDSIZE * GRIDSIZE)/2){
			Square pos;
			toGs(pos, ind);
			currPath[counter] = pos;
		}
		ind = getPrev(ind);
	}
//	xprintf("Path Done\n");
	return size;
}

//TODO fix: finds non-optimal paths sometimes
static int16_t aStar(Square start, Square goal, Square *currPath){
	if(start.x == goal.x && start.y == goal.y){
		return 0;
	}
	if(grid[goal.x][goal.y] > PROBABILITY_OF_WALKING_THRU_WALLS){
		//TODO go as close as possible
		xputs("Navigation: Unable to park in a wall.\n");
		return -1;
	}
	uint16_t goalI = toIs(goal);
	uint16_t startI = toIs(start);
	uint16_t heaplen = 1;
	uint16_t openheap[SETSIZE];
	openheap[1] = startI;
	setOpen(startI);
	g_score[startI] = 0;
	f_score[startI] = h_estimate(start, goal);
	uint16_t current;
	while(heaplen > 0) {
//		xprintf("Next\n");
		current = openheap[1];
		if(current == toIs(goal)){
			return findPath(startI, current, currPath);
		}
		setClosed(current);
		uint16_t w = 1;
		openheap[1] = openheap[heaplen];
		heaplen -= 1;
		while(1){
			uint16_t u = w;
			if(2*u+1 <= heaplen){ //If both children exist
				//Select the lowest of the children.
//				xprintf("U %u H %u\n", u, heaplen);
				if(f_score[openheap[u]] >= f_score[openheap[2*u]]){ w = 2*u;}
				if(f_score[openheap[w]] >= f_score[openheap[2*u+1]]){ w = 2*u+1;}
			} else if(2*u <= heaplen){ //If only child #1 exists
				// Check if the F cost is greater than the child
				if(f_score[openheap[u]] >= f_score[openheap[2*u]]){w = 2*u;}
			}

			if(f_score[openheap[u]] > f_score[openheap[w]]){
				uint16_t temp = openheap[u];
				openheap[u] = openheap[w];
				openheap[w] = temp;
			} else {
				break;
			}
		}
		Square neighbor[4];
		neighbor[0].x = toGx(current);
		neighbor[0].y = toGy(current) - 1;
		neighbor[1].x = toGx(current) - 1;
		neighbor[1].y = toGy(current);
		neighbor[2].x = toGx(current);
		neighbor[2].y = toGy(current) + 1;
		neighbor[3].x = toGx(current) + 1;
		neighbor[3].y = toGy(current);

		for(uint8_t dir=0; dir < 4; dir++){
			if(neighbor[dir].x < GRIDSIZE && neighbor[dir].y < GRIDSIZE && grid[neighbor[dir].x][neighbor[dir].y] < PROBABILITY_OF_WALKING_THRU_WALLS){
				uint16_t index = toIs(neighbor[dir]);

				if(getClosed(index)){
					continue;
				}

				uint8_t in_open = 0;
				if(getOpen(index)){
					in_open = 1;
				}

				uint16_t tentative;

				if(goalI == index){
					tentative = g_score[current] + 1;
				} else {
					tentative = g_score[current] + movementcost(neighbor[dir]);
				}
				if(tentative > 255){
					debug("Cost overflow!");
					vTaskSuspend(NULL);
				}

				uint8_t better_tentative;
				if(in_open == 0){
					setOpen(index);
					better_tentative = 1;
				} else if(tentative < g_score[index]){
					better_tentative = 1;
				} else {
					better_tentative = 0;
				}

				if(better_tentative == 1){
					uint16_t h = h_estimate(neighbor[dir], goal);

					clearCameFrom(index);
					setCameFrom(index, dir);
					g_score[index] = tentative;
					f_score[index] = g_score[index] + h;
					if(in_open == 0){
//						xprintf("New %u, c %u, i %u, x %u, y %u\n", dir, tentative, index, neighbor[dir].x, neighbor[dir].y);
						heaplen += 1;
						if(heaplen > SETSIZE){
							debug("Heap overflow!");
							vTaskSuspend(NULL);
						}
						openheap[heaplen] = index;
						uint16_t m = heaplen;
						while(m != 1){ //While item hasn't bubbled to the top (m=1)
							//Check if child is <= parent. If so, swap them.
							if(f_score[openheap[m]] <= f_score[openheap[m/2]]){
								uint16_t temp = openheap[m/2];
								openheap[m/2] = openheap[m];
								openheap[m] = temp;
								m = m/2;
							} else {
								break;
							}
						}
					}
				}
			}
		}
		f_score[current] = -1;
		g_score[current] = -1;
	}
	xputs("Failed to find path.\n");
	return -1;

}

extern "C" void Navigation_Init(){
	Nav_lock = xSemaphoreCreateMutex();
}
