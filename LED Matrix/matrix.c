/*
 * matrix.c
 *
 *  Created on: 15 Mar 2012
 *      Author: exuvo
 */

#include <util/delay.h>
#include "utils.h"
#include "matrix.h"
#include "bar.h"
#include <string.h>

uint16_t matrix[8] = {0b0000000000000000,
					  0b0000000000000000,
                      0b0000000000000000,
                      0b0000000000000000,
                      0b0000000000000000,
                      0b0000000000000000,
                      0b0000000000000000,
                      0b0000000000000000};
uint8_t rows[8] = {1, 2, 4, 6, 3, 5, 7, 8};

void matrixRed(){
	memset(matrix, 0b0101010101010101, sizeof(matrix));
}

void matrixGreen(){
	memset(matrix, 0b1010101010101010, sizeof(matrix));
}

void matrixYellow(){
	memset(matrix, 0b1111111111111111, sizeof(matrix));
}

void matrixClear(){
	memset(matrix, 0, sizeof(matrix));
}

void matrixSetPixel(uint8_t x, uint8_t y, uint8_t color){
	uint16_t row = matrix[y];
	row &= ~(_BV(x*2) | _BV(x*2 +1));

	if(color & GREEN){
		row |= _BV(x*2+1);
	}
	if(color & RED){
		row |= _BV(x*2);
	}
	matrix[y] = row;
}

void matrixUpdate(uint8_t row, uint16_t data){
	matrix[row] = data;
}

void matrixCoordinateCheck(){
	matrixSetPixel(0, 0, RED);
	matrixSetPixel(1, 1, RED);
	matrixSetPixel(2, 2, RED);
	matrixSetPixel(3, 3, RED);
	matrixSetPixel(4, 4, RED);
	matrixSetPixel(5, 5, RED);
	matrixSetPixel(6, 6, RED);
	matrixSetPixel(7, 7, RED);
	matrixSetPixel(0, 1, YELLOW);
	matrixSetPixel(0, 2, YELLOW);
	matrixSetPixel(0, 3, YELLOW);
	matrixSetPixel(0, 4, YELLOW);
	matrixSetPixel(0, 5, YELLOW);
	matrixSetPixel(0, 6, YELLOW);
	matrixSetPixel(0, 7, YELLOW);
	matrixSetPixel(1, 0, GREEN);
	matrixSetPixel(2, 0, GREEN);
	matrixSetPixel(3, 0, GREEN);
	matrixSetPixel(4, 0, GREEN);
	matrixSetPixel(5, 0, GREEN);
	matrixSetPixel(6, 0, GREEN);
	matrixSetPixel(7, 0, GREEN);
}

void matrixDraw(){
	static uint8_t i=0;
//	for(; i<8; i++){
		uint32_t v = matrix[i];
		v = (v & 0xFF00) | (reverse(v & 0xFF) >> 8);
		v = (v << 8) | (~_BV(rows[i]-1) & 0xFF);
		shift(&PORTD, 1, 2, 0, v, 24);
//		_delay_ms(1);
//	}
	if(++i == 8)i=0;
}
