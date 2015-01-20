/*
 * matrix.h
 *
 *  Created on: 15 Mar 2012
 *      Author: exuvo
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BLACK 0
#define GREEN 1
#define RED 2
#define YELLOW 3

void matrixDraw();
void matrixSetPixel(uint8_t x, uint8_t y, uint8_t color);
void matrixUpdate(uint8_t row, uint16_t data);
void matrixClear();
void matrixRed();
void matrixGreen();
void matrixYellow();

#ifdef __cplusplus
}
#endif
#endif /* MATRIX_H_ */
