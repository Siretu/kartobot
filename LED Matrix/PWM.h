/*
 * PWM.h
 *
 *  Created on: 24 Jul 2011
 *      Author: exuvo
 */

#ifndef PWM_H_
#define PWM_H_
#ifdef __cplusplus
extern "C" {
#endif

//timer compare to pin
#define TIMER1A 1
#define TIMER1B 2
#define TIMER2 3

void PWM(uint8_t pin, uint8_t val);

#ifdef __cplusplus
}
#endif
#endif /* PWM_H_ */
