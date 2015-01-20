/*
 * ADC.h
 *
 *  Created on: 26 Jul 2011
 *      Author: exuvo
 */

#ifndef ADC_H_
#define ADC_H_

/*
 * ADC.c
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#include <avr/io.h>
#ifdef __cplusplus
extern "C" {
#endif

#define INTERNAL 3
#define DEFAULT 1
#define EXTERNAL 0

extern uint8_t analog_reference;
void ADCinit();
uint16_t ADCread(uint8_t pin);


#ifdef __cplusplus
}
#endif
#endif /* ADC_H_ */
