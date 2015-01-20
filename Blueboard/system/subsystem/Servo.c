/*
 * servopwm.c
 *
 *  Created on: 24 mar 2012
 *      Author: per
 */

#include "Servo.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "utils.h"


void Servo_Init() {
	TIM_TIMERCFG_Type timCfgDat;
	timCfgDat.PrescaleOption = TIM_PRESCALE_TICKVAL;
	//50Hz
	//divider    1                 10                  10000           100000
	//resolution 500,000 0.020s -> 50,000 0.002s -> 50 0,000 002s -> 5 0.000 0002s
	timCfgDat.PrescaleValue = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER2) / (50*10000); //50
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, (void *) &timCfgDat);

	TIM_MATCHCFG_Type matCfgDat;
	matCfgDat.MatchChannel = 3;
	matCfgDat.IntOnMatch = ENABLE;
	matCfgDat.ResetOnMatch = ENABLE;
	matCfgDat.StopOnMatch = DISABLE;
	matCfgDat.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_ConfigMatch(LPC_TIM2, &matCfgDat);
	TIM_UpdateMatchValue(LPC_TIM2, 3, 10000);

	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_TRISTATE;
	pin.Funcnum = 2;
	pin.Portnum = 4;
	pin.Pinnum = 28;//ch0
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = 29;//ch1
	PINSEL_ConfigPin(&pin);

	matCfgDat.MatchChannel = 0;
	matCfgDat.IntOnMatch = DISABLE;
	matCfgDat.ResetOnMatch = DISABLE;
	matCfgDat.StopOnMatch = DISABLE;
	matCfgDat.ExtMatchOutputType = TIM_EXTMATCH_LOW;
	TIM_ConfigMatch(LPC_TIM2, &matCfgDat);
	TIM_UpdateMatchValue(LPC_TIM2, 0, 10001);

	matCfgDat.MatchChannel = 1;
	TIM_ConfigMatch(LPC_TIM2, &matCfgDat);
	TIM_UpdateMatchValue(LPC_TIM2, 1, 10001);

	TIM_ResetCounter(LPC_TIM2);
	TIM_Cmd(LPC_TIM2, ENABLE);

	NVIC_SetPriority(TIMER2_IRQn, 14);
	NVIC_EnableIRQ(TIMER2_IRQn);
}

/**
 * 0 <= channel <= 1
 * 0 <= value < 1024
 */
void Servo_SetDuty(uint8_t channel, uint16_t value) {
	TIM_UpdateMatchValue(LPC_TIM2, channel, map(value, 0, 1023, 190, 1170));
}

void TIMER2_IRQHandler() {
	LPC_TIM2->EMR |= _BV(0) | _BV(1);
	LPC_TIM2->IR |= _BV(3);
}
