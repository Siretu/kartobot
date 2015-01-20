/*
 * Positioning.c
 *
 *  Created on: 13 Jan 2012
 *      Author: exuvo
 */

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <math.h>
#include "utils.h"
#include "Position.h"
#include "Magneto.h"
#include "Wheels.h"
#include "motor.h"
#include "XBee.h"
#include "AHRS.h"
#include "term_io.h"
#include <stdio.h>
#include "USB.h"
#include "PID.h"
#include "EEPROM.h"
#include "Power.h"

#define MOTOR_CURRENT_SCALE 1

static xSemaphoreHandle Position_lock;

/*
 * -- ABBREVIATIONS --
 *    ang = angle
 *   dist = distance
 *   pos  = position
 *  throt = throttle
 *     ^d = delta
 *  ^[^d] = double //You get the idea.
 *     t  = time
 *     l  = left
 *     r  = right
 */
/*      0
 *   -\ | /+
 *     \|/
 *    Robot
 *      |
 *  -180|180
 */

static Point pos;
static Point setPos;
static double xD, yD;

static double ang;
static double setAng;

static double speed;
static double setSpeed = 2.0;

static uint8_t order = POS_STOP;
static uint8_t newOrder = FALSE, isNavigating = FALSE, canMove = FALSE;

PIDdata PIDL = {.eeprom = EEPROM_POS_PIDL, .windupGuard = 1000, .name = "Motor Left"};
PIDdata PIDR = {.eeprom = EEPROM_POS_PIDR, .windupGuard = 1000, .name = "Motor Right"};


static portSTACK_TYPE positionTaskStack[128];
const xTaskParameters positionTaskDefinition =
{
	Position_Run,   				/* pvTaskCode */
	(signed char *) "position",  	/* pcName */
    128,             				/* usStackDepth - defined in words, not bytes. */
    NULL,            				/* pvParameters */
    7 | portPRIVILEGE_BIT,      	/* uxPriority */
    positionTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

static void Position_SetOrder(uint8_t Order);

double fixAng(double angle){
	while (angle < -M_PI) angle += M_TWOPI;
	while (angle >  M_PI) angle -= M_TWOPI;
	return angle;
}

double setAngle(int16_t dx, int16_t dy) {
	return atan2(-dy, dx);
}

void Position_Run(void *pvParameters){
	xD = pos.x;
	yD = pos.y;
	double speedL = 0, speedR = 0;
	double oldDistL = 0, oldDistR = 0;
	uint16_t throtL = 600, throtR = 600;

	PID_Load(&PIDL);
	PID_Load(&PIDR);

	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastPositionSend = xLastWakeTime;
	portTickType xLastSpeedUpdate = xLastWakeTime;
	portTickType xLastRun = xLastWakeTime;

	while(1){
		if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
			ang = fixAng(-AHRS_GetYaw() + M_PI_2);//0, 90
			double dAng = fixAng(setAng - ang);

			double dt = (xTaskGetTickCount() - xLastRun) / 1000.0; //Time period (seconds)
			xLastRun = xTaskGetTickCount();

			if(xLastRun - xLastSpeedUpdate >= 100){ //Calculate speed
				double dt_speedUpdate = (xLastRun - xLastSpeedUpdate) / 1000.0;
//				double distL = (Wheels_GetDistanceD(MOTOR_FL) + Wheels_GetDistanceD(MOTOR_RL)) / 2;
				double distL = Wheels_GetDistanceD(MOTOR_RL);
				double distR = (Wheels_GetDistanceD(MOTOR_FR) + Wheels_GetDistanceD(MOTOR_RR)) / 2;
				//TODO take average of last x values instead of smooth

				static float al[4];
				static struct mean ml = {.size = 4, .a = al};
				speedL = mean((distL - oldDistL) / dt_speedUpdate, &ml);
				speedR = smooth((distL - oldDistL) / dt_speedUpdate, speedL, 0.8f);
//				speedR = smooth((distR - oldDistR) / dt_speedUpdate, speedR, 0.8f);
				oldDistL = distL;
				oldDistR = distR;

				float d = PID_Update(setSpeed, ABS(speedL), &PIDL);
				float e = PID_Update(setSpeed, ABS(speedR), &PIDL);
//				xprintf("%lu %lu, %lu %lu\n", (int32_t)(d), (uint32_t)(ABS(speedL)*100), (int32_t)(e), (uint32_t)(ABS(speedR)*100));
				throtL = constrain(d, 0, 2000);
				throtR = constrain(e, 0, 2000);
//				throtR = constrain(PID_Update(setSpeed, ABS(speedR), &PIDR), 0, 2000);

				if(Power_GetCurrent() > Power_GetVoltage() * (throtL + throtR) * MOTOR_CURRENT_SCALE){
					//TODO detect stall using current sensor
				}

				xLastSpeedUpdate = xLastRun;
			}

			if(order != POS_ROTATE){//order == POS_FORWARD || order == POS_BACK
				speed = (speedL + speedR) / 2;
				xD += cos(ang) * speed * dt;
				yD += -sin(ang) * speed * dt;
				pos.x = xD;
				pos.y = yD;
			}else{
				speed = 0;
				if(order == POS_ROTATE && ABS(dAng) <= TO_RAD(10)){
					Position_SetOrder(POS_STOP);
				}
			}

			if(isNavigating){
				int16_t dx = setPos.x - pos.x, dy = setPos.y - pos.y;
				if(ABS(dAng) > TO_RAD(10)){//If needs to rotate, rotate
					if(order != POS_ROTATE){
						setAng = setAngle(dx, dy);
						Position_SetOrder(POS_ROTATE);
					}
				}else if(sqrt(dx*dx + dy*dy) > 5){  //Else if needs to move, move
					if(order != POS_FORWARD){
						Position_SetOrder(POS_FORWARD);
					}
				}else{   //Else the robot reached the target pos
					Position_SetOrder(POS_STOP);
					isNavigating = FALSE;
				}
			}

			if(canMove){
				Motor_ControlSpeed(SIDE_LEFT, throtL);
				Motor_ControlSpeed(SIDE_RIGHT, throtR);
				if(newOrder){
					switch(order){
					case POS_ROTATE:
						if(dAng < 0){
							Motor_Control(SIDE_LEFT, GO_FORWARD);
							Motor_Control(SIDE_RIGHT, GO_BACK);
						}else{
							Motor_Control(SIDE_LEFT, GO_BACK);
							Motor_Control(SIDE_RIGHT, GO_FORWARD);
						}
						break;
					case POS_FORWARD:
						Motor_Control(SIDE_LEFT, GO_FORWARD);
						Motor_Control(SIDE_RIGHT, GO_FORWARD);
						break;
					case POS_BACK:
						Motor_Control(SIDE_LEFT, GO_BACK);
						Motor_Control(SIDE_RIGHT, GO_BACK);
						break;
					case POS_STOP:
					default:
						Motor_Control(SIDE_LEFT, GO_STOP);
						Motor_Control(SIDE_RIGHT, GO_STOP);
						break;
					}
					newOrder = FALSE;
				}
			}else{
				Motor_Control(SIDE_LEFT, GO_STOP);
				Motor_Control(SIDE_RIGHT, GO_STOP);
				Motor_ControlSpeed(SIDE_LEFT, 0);
				Motor_ControlSpeed(SIDE_RIGHT, 0);
			}

			if(xLastWakeTime - xLastPositionSend > (USB_isConnected() ? 100 : 300)){
				int16_t currentSpeedL = speedL * 16;
				int16_t currentSpeedR = speedR * 16;
				int16_t setA = TO_DEG(fixAng(-setAng - M_PI_2)) * 64.0;//TO_DEG(setAng) * 64.0;
				uint8_t d[16] = {'P', pos.x >> 8, pos.x, pos.y >> 8, pos.y, order, throtL >> 8, throtL, throtR >> 8, throtR,
								currentSpeedL >> 8, currentSpeedL, currentSpeedR >> 8, currentSpeedR, setA >> 8, setA};
				XBee_Send(d, 16);
				xLastPositionSend = xLastWakeTime;

//				char b[64];//25*3+1
//				sprintf(b, "ang %+07.2f dAng %+07.2f\n", TO_DEG(ang), TO_DEG(dAng));
//				xputs(b);
			}

			xSemaphoreGive(Position_lock);
		}
		vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_RATE_MS);
	}
}

void Position_Init(){
	Position_lock = xSemaphoreCreateMutex();
	// Grid is 128x128 so we want to start approximately in the middle. Coordinates are in centimeters.
	pos.x = 320; pos.y = 320;
}

void Position_GetXY(Point *p){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		p->x = pos.x;
		p->y = pos.y;
		xSemaphoreGive(Position_lock);
	}
}

static void Position_SetOrder(uint8_t Order){
		order = Order;
		newOrder = TRUE;
}

/* static */void Position_FaceAngle(double newAngle){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		setAng = newAngle;
		Position_SetOrder(POS_ROTATE);
		xSemaphoreGive(Position_lock);
	}

}

void Position_GoTo(Point p){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		setPos.x = p.x;
		setPos.y = p.y;
		isNavigating = TRUE;
		setAng = setAngle(setPos.x - pos.x, setPos.y - pos.y);
		xSemaphoreGive(Position_lock);
	}
}

void Position_SetPos(Point p){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		xD = p.x;
		yD = p.y;
		xSemaphoreGive(Position_lock);
	}
}

void Position_StopMoving(){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		isNavigating = FALSE;
		Position_SetOrder(POS_STOP);
		xSemaphoreGive(Position_lock);
	}
}

void Position_CanMove(uint8_t boolean){
	canMove = boolean;
	if(boolean && isNavigating){
		newOrder = TRUE;
	}
}

void Position_SetSpeed(double newSpeed){
	if(xSemaphoreTake(Position_lock, portMAX_DELAY) == pdTRUE){
		setSpeed = newSpeed;

		xSemaphoreGive(Position_lock);
	}

}

