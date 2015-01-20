/*
 * wav.h
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#ifndef WAV_H_
#define WAV_H_

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

void Audio_WAV_Setup();
uint16_t Audio_WAV_Play();
void Audio_WAV_Cleanup();

#ifdef __cplusplus
}
#endif
#endif /* WAV_H_ */
