/*
 * AudioInternal.h
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#ifndef AUDIOINTERNAL_H_
#define AUDIOINTERNAL_H_

#include "Audio.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t playing;
uint32_t position, length;
void *resources;
Song currentSong;

void Audio_SetFrequency(uint32_t hz);

#ifdef __cplusplus
}
#endif
#endif /* AUDIOINTERNAL_H_ */
