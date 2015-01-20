/*
 * Comm.c
 *
 *  Created on: 22 Jun 2012
 *      Author: exuvo
 */

#include <string.h>
#include "term_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Comm.h"
#include "Time2.h"
#include "Wheels.h"
#include "EEPROM.h"
#include <stdio.h>
#include "motor.h"
#include "I2C.h"
#include "Magneto.h"
#include <math.h>
#include "Position.h"
#include "Audio.h"
#include "AHRS.h"
#include "navigation.h"
#include "utils.h"
#include "dbg.h"
#include "main.h"

#define n() (*next)()
#define l() (*available)()
#define r(d, l) (*send)(d, l)

extern "C" void Communication(uint8_t (*next)(), uint8_t (*available)(), void (*send)(const uint8_t *data, uint16_t length)){
	switch(n()){
		case 'c':{//Console text input
			uint8_t length = l();//n();
			while(length--){
				term_io_put(n());
			}
			break;
		}
		case 's':{//system
			switch(n()){//Command
				case 's':{//Task status
					xprintf("\n-Run Time Stats-\nTasks %lu,  Free heap %lu %lu", uxTaskGetNumberOfTasks(), (uint32_t)xPortGetFreeHeapSize(), (uint32_t)xPortGetFreeHeapSize2());
					uint16_t size = 40*uxTaskGetNumberOfTasks();
					char buffer[size];

					vTaskGetRunTimeStats((signed char*)buffer);//Total execution time of each task, absolute count value and percentage of total system execution time.
					buffer[size-1] = 0;
					xputs(buffer);

					xputs("\n-Task List-\nName		State	Priority	Stack	ID");
					vTaskList((signed char*)buffer);//Lists all the current tasks, along with their current state and stack usage high water mark.
					buffer[size-1] = 0;
					xputs(buffer);

					xputs("\n");
					break;
				}
				case 'p':{//Set task priority
					uint8_t id = n();
					uint8_t priority = n();
					xTaskHandle pxTask = pxTaskGetByID( id );

					if(pxTask != NULL){
						vTaskPrioritySet( pxTask, priority );
						xprintf("Task \"%s\" ID %u priority changed to %u\n", pcTaskGetTaskName(pxTask), id, priority);
					}else{
						xprintf("Task with ID %u not found\n", id);
					}
					break;
				}
				case 'u':{//Suspend a task
					uint8_t id = n();
					xTaskHandle pxTask = pxTaskGetByID( id );

					if(pxTask != NULL){
						vTaskSuspend( pxTask );
						xprintf("Task \"%s\" ID %u suspended\n", pcTaskGetTaskName(pxTask), id);
					}else{
						xprintf("Task with ID %u not found\n", id);
					}
					break;
				}
				case 'v':{//Resume a task
					uint8_t id = n();
					xTaskHandle pxTask = pxTaskGetByID( id );

					if(pxTask != NULL){
						vTaskResume( pxTask );
						xprintf("Task \"%s\" ID %u resumed\n", pcTaskGetTaskName(pxTask), id);
					}else{
						xprintf("Task with ID %u not found\n", id);
					}
					break;
				}
				case 'r':{//Reset the system
					NVIC_SystemReset();
					break;
				}
				case 'c':{//CPUID
					xputs("\n-CPUID-\n");
					uint32_t cpuid = SCB->CPUID;
					xprintf ("Implementer %lx\n", (cpuid & SCB_CPUID_IMPLEMENTER_Msk) 	>> SCB_CPUID_IMPLEMENTER_Pos);
					xprintf ("Variant %lx\n", 	(cpuid & SCB_CPUID_VARIANT_Msk) 	>> SCB_CPUID_VARIANT_Pos);
					xprintf ("Constant %lx\n", 	(cpuid & (0xF << 16)) 				>> 16);
					xprintf ("PartNO %lx\n", 	(cpuid & SCB_CPUID_PARTNO_Msk) 		>> SCB_CPUID_PARTNO_Pos);
					xprintf ("Revision %lx\n", 	(cpuid & SCB_CPUID_REVISION_Msk) 	>> SCB_CPUID_REVISION_Pos);
					xprintf ("CPU speed %lu Hz\n", SystemCoreClock);
					break;
				}
				case 'e':{//EEPROM read
					uint16_t address = ((uint16_t)n() << 8) | n();
					uint8_t length = n();
					uint8_t data[length];

					if(EEPROM_read(address, data, length)){
						xprintf("Read %u bytes from EEPROM address %u:\n", length, address);
						for(uint8_t i=0; i < length; i++){
							xprintf("%2X ", data[i]);
						}
						xputs("\n");
					}else{
						xputs("Failed to read from EEPROM\n");
					}
					break;
				}
				case 'w':{//EEPROM write
					uint16_t address = ((uint16_t)n() << 8) | n();
					uint8_t length = n();
					uint8_t data[length];
					for(uint8_t i=0; i < length; i++){
						data[i] = n();
					}

					if(EEPROM_write(address, data, length)){
						xprintf("Wrote %u bytes to EEPROM address %u\n", length, address);
					}else{
						xputs("Failed to write to EEPROM\n");
					}
					break;
				}
				case 'i':{//I2C debug
					uint8_t channel = n();
					uint8_t address = n();
					uint8_t retries = n();
					uint8_t wLength = n();
					uint8_t out[wLength];
					for(uint8_t i=0; i < wLength; i++){
						out[i] = n();
					}
					uint8_t rLength = n();
					uint8_t in[rLength];

					I2C_M_SETUP_Type t;
					t.sl_addr7bit = address;
					t.tx_data = out;
					t.tx_length = wLength;
					t.rx_data = in;
					t.rx_length = rLength;
					t.retransmissions_max = retries;
					uint32_t status;
					if(channel == 0){
						status = I2C0_sendS(&t);
					}else{
						status = I2C1_sendS(&t);
					}

					xprintf("I2C%u: %2X. ", channel, address);
					if(t.status & I2C_SETUP_STATUS_ARBF){
						xputs("ARBF ");
					}
					if(t.status & I2C_SETUP_STATUS_NOACKF){
						xputs("NOACKF ");
					}
					if(t.status & I2C_SETUP_STATUS_DONE){
						xputs("DONE ");
					}

					switch(t.status & I2C_STAT_CODE_BITMASK){
						case I2C_I2STAT_NO_INF:
							xputs("NO_INF");
						case I2C_I2STAT_M_TX_START:
							xputs("START");
							break;
						case I2C_I2STAT_M_TX_RESTART:
							xputs("RESTART");
							break;
						case I2C_I2STAT_M_TX_SLAW_ACK:
							xputs("TX_SLAW_ACK");
							break;
						case I2C_I2STAT_M_TX_SLAW_NACK:
							xputs("TX_SLAW_NACK");
							break;
						case I2C_I2STAT_M_TX_DAT_ACK:
							xputs("TX_DAT_ACK");
							break;
						case I2C_I2STAT_M_TX_DAT_NACK:
							xputs("TX_DAT_NACK");
							break;
						case I2C_I2STAT_M_TX_ARB_LOST:
							xputs("ARB_LOST");
							break;
						case I2C_I2STAT_M_RX_SLAR_NACK:
							xputs("RX_SLAR_NACK");
							break;
						case I2C_I2STAT_M_RX_DAT_ACK:
							xputs("RX_DAT_ACK");
							break;
						case I2C_I2STAT_M_RX_DAT_NACK:
							xputs("RX_DAT_NACK");
							break;
					}

					xprintf(". Return %lX. Wrote %lu bytes. Read %lu bytes: ", status, t.tx_count, t.rx_count);
					for(uint8_t i=0; i < rLength; i++){
						xprintf("%2X ", in[i]);
					}
					xputs("\n");
					break;
				}
				case 'I':{//I2C scan
					uint8_t in;
					I2C_M_SETUP_Type t;
					t.tx_data = NULL;
					t.tx_length = 0;
					t.rx_data = &in;
					t.rx_length = 1;//TODO change to 0
					t.retransmissions_max = 0;

					xputs("I2C0 scan..\n");
					for(uint8_t address=0; address < 128; address++){
						t.sl_addr7bit = address;
						if(I2C0_sendS(&t)){
							xprintf("0x%2X responded\n", address);
						}
					}
					xputs("I2C0 scan complete.\n");

					xputs("I2C1 scan..\n");
					for(uint8_t address=0; address < 128; address++){
						t.sl_addr7bit = address;
						if(I2C1_sendS(&t)){
							xprintf("0x%2X responded\n", address);
						}
					}
					xputs("I2C1 scan complete.\n");
					break;
				}
				case 'a':{//Audio play
					uint8_t N = n();

					if(N == 0){
						Audio_Stop();
						break;
					}

					const Song s = {A_CODE, (void*)N };
					Audio_PlaySong(s);
					break;
				}
				case 'A':{//Audio play
					extern const Song song2;
					Audio_PlaySong(song2);
					break;
				}
			}
			break;
		}
		case 'n':{//Navigation
			switch(n()){//Command
				case 'A':{//Get magnetometer angle
					char b[32];

					sprintf(b, "%+03.4f", Magneto_GetHeading() * 180.0 / M_PI);

					xprintf("Magnetometer heading: %s\n", b);
					break;
				}
				case 'R':{//AHRS normal
					AHRS_ModeNormal();
					xprintf("AHRS in normal mode\n");
					break;
				}
				case 'C':{//AHRS calibrate
					AHRS_ModeCalibrate();
					xprintf("AHRS in calibration mode\n");
					break;
				}
				case 'D':{//AHRS debug
					AHRS_ModeDebug();
					xprintf("AHRS in debug mode\n");
					break;
				}
				case 'N':{//AHRS use next sensor for calibration mode
					AHRS_NextSensor();
					break;
				}
				case 'g':{//Path To Square
					Point p;
					p.x = 10 * n();
					p.y = 10 * n();
					Navigation_GoTo(p);
					break;
				}
				case 'w':{//Add wall
					Square s;
					s.x = n();
					s.y = n();
					Navigation_AddWall(s);
					break;
				}
				case 'E':{//Allow navigation to explore
					Navigation_Exploring(n());
					break;
				}
				case 'P':{//Allow navigation to path
					Navigation_Pathing(n());
					break;
				}
				case 's':{
					if(n()){
						LPC_GPIO1->FIOSET = (1 << 24) | (1 << 25);//Start ranging
						vTaskResume(tasks.sonar);
					}else{
						LPC_GPIO1->FIOCLR = (1 << 24) | (1 << 25);//Stop ranging
					}
					break;
				}
			}
			break;
		}
		case 'm':{//Movement
			switch(n()){//Command
				case 'e':{//Wheels status
					xputs("\n-Quadrature Wheels-\n");
					xprintf ("Front Left: 	%ld pulses.	%ld revolutions.	%ld cm\n", Wheels_GetPulses(MOTOR_FL), Wheels_GetRevolutions(MOTOR_FL), Wheels_GetDistance(MOTOR_FL));
					xprintf ("Front Right: 	%ld pulses.	%ld revolutions.	%ld cm\n", Wheels_GetPulses(MOTOR_FR), Wheels_GetRevolutions(MOTOR_FR), Wheels_GetDistance(MOTOR_FR));
					xprintf ("Rear  Left: 	%ld pulses.	%ld revolutions.	%ld cm\n", Wheels_GetPulses(MOTOR_RL), Wheels_GetRevolutions(MOTOR_RL), Wheels_GetDistance(MOTOR_RL));
					xprintf ("Rear  Right: 	%ld pulses.	%ld revolutions.	%ld cm\n", Wheels_GetPulses(MOTOR_RR), Wheels_GetRevolutions(MOTOR_RR), Wheels_GetDistance(MOTOR_RR));
					break;
				}
				case 'r':{//Reset Wheels
					Wheels_Reset();
					break;
				}
				case 'm':{//Manual motor control
					uint8_t motorSide = n();
					uint8_t dir = n();
					uint16_t vel = ((uint16_t)n() << 8) | n();
					Motor_Control(motorSide, dir);
					Motor_ControlSpeed(motorSide, vel);
					break;
				}
				case 'g':{//Go To Square
					Point p;
					p.x = 10 * n();
					p.y = 10 * n();
					Position_GoTo(p);
					break;
				}
				case 'G':{//Set pos
					Point p;
					p.x = 10 * n();
					p.y = 10 * n();
					Position_SetPos(p);
					break;
				}
				case 'a':{//Face angle
					double angle = TO_RAD( (double) ( ((int16_t) n() << 8) | n()) );
					Position_FaceAngle(angle);
					break;
				}
				case 's':{//Set speed
					double order = ( (uint32_t) n() << 16 | n() << 8 | n() ) / 1024.0;
					Position_SetSpeed(order);
					break;
				}
				case 'S':{//Stop
					Position_StopMoving();
					break;
				}
				case 'M':{//Allow robot to move
					Position_CanMove(n());
					break;
				}

				//TODO get motor target speed, current speed,
			}
			break;
		}
		case 'P':{//PIDs
			switch(n()){
				case 'l':{//Length
					uint8_t d[] = {'d', 'l', PID_Length()};
					r(d, sizeof(d));
					break;
				}
				case 'g':{//Get
					uint8_t id = n();
					PIDdata *p = PID_Get(id);
					if(!p)break;

					uint8_t d[12 + strlen(p->name) + 1];
					d[0] = 'd';
					d[1] = 'g';
					d[2] = id;
					d[3] = (int32_t)(p->P * 1024) >> 16;
					d[4] = (int32_t)(p->P * 1024) >> 8;
					d[5] = (int32_t)(p->P * 1024);
					d[6] = (int32_t)(p->I * 1024) >> 16;
					d[7] = (int32_t)(p->I * 1024) >> 8;
					d[8] = (int32_t)(p->I * 1024);
					d[9] = (int32_t)(p->D * 1024) >> 16;
					d[10] = (int32_t)(p->D * 1024) >> 8;
					d[11] = (int32_t)(p->D * 1024);

					uint8_t i=12;
					const char *c = p->name;
					while(*c){
						d[i++] = *c++;
					}
					d[i++] = 0;

					r(d, i);
					break;
				}
				case 's':{//Set
					PIDdata *p = PID_Get(n());
					float P = ( (uint32_t) n() << 16 | n() << 8 | n() ) / 1024.0;
					float I = ( (uint32_t) n() << 16 | n() << 8 | n() ) / 1024.0;
					float D = ( (uint32_t) n() << 16 | n() << 8 | n() ) / 1024.0;
					if(!p)break;
					PID_Set(p, P, I, D);
					break;
				}
				case 'S':{//Save
					PIDdata *p = PID_Get(n());
					if(!p)break;
					PID_Save(p);
					break;
				}
				case 'L':{//Load
					PIDdata *p = PID_Get(n());
					if(!p)break;
					PID_Load(p);
					break;
				}
			}
			break;
		}
		case 't':{//Test
			extern uint8_t lulz;
			lulz = !lulz;
			break;
		}
	}

}
