/*
 * utils.c
 *
 *  Created on: 10 okt 2011
 *      Author: Erik
 */

#include "lpc_types.h"
#include "utils.h"

long map(long x, long in_min, long in_max, long out_min, long out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double mapD(double x, double in_min, double in_max, double out_min, double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t upper_power_of_two(uint32_t v){
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

float smooth(float currentData, float previousData, float smoothFactor) {
	return (previousData * (1.0f - smoothFactor) + (currentData * smoothFactor));
}

double smoothD(double currentData, double previousData, double smoothFactor) {
	return (previousData * (1.0 - smoothFactor) + (currentData * smoothFactor));
}

float mean(float newData, struct mean *s){
	s->a[s->prev++ % s->size] = newData;

	float sum = 0;
	for(uint8_t i=0; i < s->size; i++){
		sum += s->a[i];
	}
	return sum;
}

