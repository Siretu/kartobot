/*
 * Comm.h
 *
 *  Created on: 22 Jun 2012
 *      Author: exuvo
 */

#ifndef COMM_H_
#define COMM_H_


#ifdef __cplusplus
extern "C" {
#endif

void Communication(uint8_t (*next)(), uint8_t (*available)(), void (*send)(const uint8_t *data, uint16_t length));

#ifdef __cplusplus
}
#endif

#endif /* COMM_H_ */
