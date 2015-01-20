/*
 * Audio.h
 *
 *  Created on: 26 Nov 2011
 *      Author: exuvo
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	A_NOTHING=0, A_CHIPTUNE=1, A_WAV=2, A_CODE=3, A_MPC=4
};
typedef uint8_t AudioType;

typedef struct Song_t{
	AudioType type;
	void * data;//Points to either a Chiptune or WAV. For Code its value decides what to play.
	uint8_t loops;//Amount of times to loop the song.
} Song;

void Audio_Stop();
uint8_t Audio_PlaySong(Song song);
uint8_t Audio_Playing();
const Song Audio_CurrentSong();

void Audio_Init();
void Audio_Play();
extern const xTaskParameters AudioTaskDefinition;


#ifdef __cplusplus
}
#endif
#endif /* AUDIO_H_ */


/**
 * Hardware analog lowpass RC filter
 * Cutoff frequency = 10610Hz
 * R = 1.5 Ohm
 * C = 10uF
 */
