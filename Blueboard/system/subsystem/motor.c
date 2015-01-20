/*
 * motor.c
 *
 *  Created on: 13 okt 2011
 *      Author: per
 */

/*
					____FRAM____
					|          |
		/--	MOTOR_1 |		   | MOTOR_2 --\
		|			|		   |		   |
SIDA_L <-			|          |		   -> SIDA_R
		|			|		   |		   |
		\--	MOTOR_3 |		   | MOTOR_4 --/
					|		   |
					----BAK-----
*/

#include "FreeRTOS.h"
#include "motor.h"
#include "lpc17xx_clkpwr.h"

/* IN1, IN2, MatchChannel             Front        Rear 			*/
const uint8_t motors[2][2][3] = { { {0, 4, 5}, {10, 15, 2} },	//Left
								  { {8, 1, 6}, {14,  9, 4} } };	//Right
//motors[x][y][dataIndex], example: motors[0][0][dataIndex] = MOTOR_1, motors[1][0][dataIndex] = MOTOR_2


void Motor_Init() {
	// Match Channel 0
	PWM_TIMERCFG_Type pwmCfgDat;
	pwmCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	pwmCfgDat.PrescaleValue = 1;//50kHz	CLKPWR_GetPCLK(CLKPWR_PCLKSEL_PWM1) / (50000*2000)
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &pwmCfgDat);
	PWM_MatchUpdate(LPC_PWM1, 0, 2000, PWM_MATCH_UPDATE_NOW);

	PWM_MATCHCFG_Type pwmMatchCfgDat;
	pwmMatchCfgDat.MatchChannel = 0;
	pwmMatchCfgDat.IntOnMatch = DISABLE; 		//Interrupt on match
	pwmMatchCfgDat.ResetOnMatch = ENABLE;
	pwmMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &pwmMatchCfgDat);


	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_TRISTATE;
	uint8_t pwmPinNumber[2][2] = {{5, 20}, {4, 3}};		//Pin number for PWM output. Port and func is hardcoded. //25->20
	uint8_t temp_channel;
	pwmMatchCfgDat.IntOnMatch = DISABLE;
	pwmMatchCfgDat.ResetOnMatch = DISABLE;
	pwmMatchCfgDat.StopOnMatch = DISABLE;
	for(uint8_t x = 0; x < 2; x++) {
		for(uint8_t y = 0; y < 2; y++) {		//Change to "y < 2" to initialize back motors.
			//A, B pins
			pin.Funcnum = 0;
			pin.Portnum = 1;
			for(uint8_t z = 0; z < 2; z++) {
				pin.Pinnum = motors[x][y][z];
				PINSEL_ConfigPin(&pin);
				LPC_GPIO1 -> FIODIR |= (1 << pin.Pinnum);
				LPC_GPIO1 -> FIOCLR = (1 << pin.Pinnum);
			}
			//PWM pins
			if(x == 0 && y == 1) {
				pin.Funcnum = 2;
				pin.Portnum = 1;//3
				pin.Pinnum = pwmPinNumber[x][y];
				PINSEL_ConfigPin(&pin);
			} else {
				pin.Funcnum = 1;
				pin.Portnum = 2;
				pin.Pinnum = pwmPinNumber[x][y];
				PINSEL_ConfigPin(&pin);
			}

			//PWM Match Channel
			temp_channel = motors[x][y][2];
			PWM_ChannelConfig(LPC_PWM1, temp_channel, PWM_CHANNEL_SINGLE_EDGE);
			PWM_MatchUpdate(LPC_PWM1, temp_channel, 0, PWM_MATCH_UPDATE_NOW);
			pwmMatchCfgDat.MatchChannel = temp_channel;
			PWM_ConfigMatch(LPC_PWM1, &pwmMatchCfgDat);
			PWM_ChannelCmd(LPC_PWM1, temp_channel, ENABLE);
		}
	}

	PWM_ResetCounter(LPC_PWM1);
	PWM_Cmd(LPC_PWM1, ENABLE);
	PWM_CounterCmd(LPC_PWM1, ENABLE);

}

void Motor_Disable(){
	PWM_Cmd(LPC_PWM1, DISABLE);//TODO not working, set control pins to off instead
}

void Motor_Enable(){
	PWM_Cmd(LPC_PWM1, ENABLE);
}

uint8_t Motor_Enabled(){
	return LPC_PWM1->TCR & PWM_TCR_PWM_ENABLE;
}

/*
 * Assumes no outer changes has been done to $pin and $pwmMatchCfgDat.
 * It's dependent on GO_FORWARD and GO_BACK values, so these should not be changed.
 */
void Motor_Control(uint8_t motorSide, uint8_t dir) {
	if((motorSide == SIDE_RIGHT || motorSide == SIDE_LEFT) && (dir == GO_FORWARD || dir == GO_BACK)) {
		for(uint8_t y = 0; y < 2; y++) {	//Change to "y < 2" to set back motors.
			LPC_GPIO1 -> FIOSET = (1 << motors[motorSide][y][1 - dir]);	//HIGH
			LPC_GPIO1 -> FIOCLR = (1 << motors[motorSide][y][dir]);		//LOW
		}
	} else if(dir == GO_STOP) {
		for(uint8_t y = 0; y < 2; y++) {
			LPC_GPIO1 -> FIOCLR = (1 << motors[motorSide][y][dir]) | (1 << motors[motorSide][y][1 - dir]);		//LOW
		}
	}
}

//vel: 0-2000
void Motor_ControlSpeed(uint8_t motorSide, uint16_t vel) {
	for(uint8_t y = 0; y < 2; y++) {	//Change to "y < 2" to set back motors.
		PWM_MatchUpdate(LPC_PWM1, motors[motorSide][y][2], vel, PWM_MATCH_UPDATE_NEXT_RST);
	}
}
