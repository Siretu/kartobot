/*
 * chiptune.h
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#ifndef CHIPTUNE_H_
#define CHIPTUNE_H_

#define CHIPTUNE_FREQ 16000U

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Chiptune {
	uint8_t maxtrack;
	uint8_t length;
	uint8_t data[];
} Chiptune;

void Audio_Chiptune_Setup();
uint16_t Audio_Chiptune_Play();
void Audio_Chiptune_Cleanup();

#ifdef __cplusplus
}
#endif
#endif /* CHIPTUNE_H_ */
