/*
 * chiptune.c
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#include "AudioInternal.h"
#include "chiptune.h"
#include "Display.h"
#include "term_io.h"
#include "AudioSong2.h"
#include "utils.h"

#define TRACKLEN 32

//Chiptunes
//http://www.youtube.com/watch?v=aEjcK5JFEFE&feature=BFa&list=UL8l1YWXrGjyA&lf=like-suggest
//http://www.linusakesson.net/hardware/chiptune.php

static const uint16_t freqtable[] = {
	0x0085, 0x008d, 0x0096, 0x009f, 0x00a8, 0x00b2, 0x00bd, 0x00c8, 0x00d4,
	0x00e1, 0x00ee, 0x00fc, 0x010b, 0x011b, 0x012c, 0x013e, 0x0151, 0x0165,
	0x017a, 0x0191, 0x01a9, 0x01c2, 0x01dd, 0x01f9, 0x0217, 0x0237, 0x0259,
	0x027d, 0x02a3, 0x02cb, 0x02f5, 0x0322, 0x0352, 0x0385, 0x03ba, 0x03f3,
	0x042f, 0x046f, 0x04b2, 0x04fa, 0x0546, 0x0596, 0x05eb, 0x0645, 0x06a5,
	0x070a, 0x0775, 0x07e6, 0x085f, 0x08de, 0x0965, 0x09f4, 0x0a8c, 0x0b2c,
	0x0bd6, 0x0c8b, 0x0d4a, 0x0e14, 0x0eea, 0x0fcd, 0x10be, 0x11bd, 0x12cb,
	0x13e9, 0x1518, 0x1659, 0x17ad, 0x1916, 0x1a94, 0x1c28, 0x1dd5, 0x1f9b,
	0x217c, 0x237a, 0x2596, 0x27d3, 0x2a31, 0x2cb3, 0x2f5b, 0x322c, 0x3528,
	0x3851, 0x3bab, 0x3f37
};

static const int8_t sinetable[] = {
	0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126,
	127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12,
	0, -12, -25, -37, -49, -60, -71, -81, -90, -98, -106, -112, -117, -122,
	-125, -126, -127, -126, -125, -122, -117, -112, -106, -98, -90, -81,
	-71, -60, -49, -37, -25, -12
};

static const uint8_t validcmds[] = "0dfijlmtvw~+=";

enum {
	WF_TRI,
	WF_SAW,
	WF_PUL,
	WF_NOI
};

volatile struct oscillator {
	uint16_t	freq;
	uint16_t	phase;
	uint16_t	duty;
	uint8_t		waveform;
	uint8_t		volume;	// 0-255
} osc[4];

struct trackline {
	uint8_t	note;
	uint8_t	instr;
	uint8_t	cmd[2];
	uint8_t	param[2];
};

struct track {
	struct trackline	line[TRACKLEN];
};

struct unpacker {
	uint16_t	nextbyte;
	uint8_t		buffer;
	uint8_t		bits;
};

struct channel {
	struct unpacker		trackup;
	uint8_t			tnum;
	int8_t			transp;
	uint8_t			tnote;
	uint8_t			lastinstr;
	uint8_t			inum;
	uint16_t			iptr;
	uint8_t			iwait;
	uint8_t			inote;
	int8_t			bendd;
	int16_t			bend;
	int8_t			volumed;
	int16_t			dutyd;
	uint8_t			vdepth;
	uint8_t			vrate;
	uint8_t			vpos;
	int16_t			inertia;
	uint16_t			slur;
} channel[4];

static uint8_t trackwait;
static uint8_t trackpos;
static uint8_t callbackwait;

static struct unpacker chiptuneUP;


uint8_t readsongbyte(uint16_t offset){
	return ((Chiptune*)currentSong.data)->data[offset];
}

uint8_t readbit(struct unpacker *up) {
	uint8_t val;

	if(!up->bits) {
		up->buffer = readsongbyte(up->nextbyte++);
		up->bits = 8;
	}

	up->bits--;
	val = up->buffer & 1;
	up->buffer >>= 1;

	return val;
}

uint16_t readchunk(struct unpacker *up, uint8_t n) {
	uint16_t val = 0;
	uint8_t i;

	for(i = 0; i < n; i++) {
		if(readbit(up)) {
			val |= (1 << i);
		}
	}

	return val;
}

void initup(struct unpacker *up, uint16_t offset) {
	up->nextbyte = offset;
	up->bits = 0;
}

void readinstr(uint8_t num, uint8_t pos, uint8_t *dest) {
	dest[0] = readsongbyte(((uint16_t*)resources)[num] + 2 * pos + 0);
	dest[1] = readsongbyte(((uint16_t*)resources)[num] + 2 * pos + 1);
}

void runcmd(uint8_t ch, uint8_t cmd, uint8_t param) {
	switch(validcmds[cmd]) {
		case '0':
			channel[ch].inum = 0;
			break;
		case 'd':
			osc[ch].duty = param << 8;
			break;
		case 'f':
			channel[ch].volumed = param;
			break;
		case 'i':
			channel[ch].inertia = param << 1;
			break;
		case 'j':
			channel[ch].iptr = param;
			break;
		case 'l':
			channel[ch].bendd = param;
			break;
		case 'm':
			channel[ch].dutyd = param << 6;
			break;
		case 't':
			channel[ch].iwait = param;
			break;
		case 'v':
			osc[ch].volume = param;
			break;
		case 'w':
			osc[ch].waveform = param;
			break;
		case '+':
			channel[ch].inote = param + channel[ch].tnote - 12 * 4;
			break;
		case '=':
			channel[ch].inote = param;
			break;
		case '~':
			if(channel[ch].vdepth != (param >> 4)) {
				channel[ch].vpos = 0;
			}
			channel[ch].vdepth = param >> 4;
			channel[ch].vrate = param & 15;
			break;
	}
}

void playroutine() {	// called at 50 Hz
	uint8_t ch;
	static uint8_t light[8];

	if(playing) {
		if(trackwait) {
			trackwait--;
		} else {
			trackwait = 4;

			if(!trackpos) {
				if(position >= ((Chiptune*)currentSong.data)->length) {
					playing = 0;
				} else {
					for(ch = 0; ch < 4; ch++) {
						uint8_t gottransp;
						uint8_t transp;

						gottransp = readchunk(&chiptuneUP, 1);
						channel[ch].tnum = readchunk(&chiptuneUP, 6);
						if(gottransp) {
							transp = readchunk(&chiptuneUP, 4);
							if(transp & 0x8) transp |= 0xf0;
						} else {
							transp = 0;
						}
						channel[ch].transp = (int8_t) transp;
						if(channel[ch].tnum) {
							initup(&channel[ch].trackup, ((uint16_t*)resources)[16 + channel[ch].tnum - 1]);
						}
					}
					position++;
				}
			}

			if(playing) {
				for(ch = 0; ch < 4; ch++) {
					if(channel[ch].tnum) {
						uint8_t note, instr, cmd, param;
						uint8_t fields;

						fields = readchunk(&channel[ch].trackup, 3);
						note = 0;
						instr = 0;
						cmd = 0;
						param = 0;
						if(fields & 1) note = readchunk(&channel[ch].trackup, 7);
						if(fields & 2) instr = readchunk(&channel[ch].trackup, 4);
						if(fields & 4) {
							cmd = readchunk(&channel[ch].trackup, 4);
							param = readchunk(&channel[ch].trackup, 8);
						}
						if(note) {
							channel[ch].tnote = note + channel[ch].transp;
							if(!instr) instr = channel[ch].lastinstr;
						}
						if(instr) {
							if(instr == 2) light[1 + ch*2] = 5;
							if(instr == 1) {
								light[0 + ch*2] = 5;
								if(channel[ch].tnum == 4) {
									light[0 + ch*2] = light[1 + ch*2] = 3;
								}
							}
							if(instr == 7) {
								light[0 + ch*2] = light[1 + ch*2] = 30;
							}
							channel[ch].lastinstr = instr;
							channel[ch].inum = instr;
							channel[ch].iptr = 0;
							channel[ch].iwait = 0;
							channel[ch].bend = 0;
							channel[ch].bendd = 0;
							channel[ch].volumed = 0;
							channel[ch].dutyd = 0;
							channel[ch].vdepth = 0;
						}
						if(cmd) runcmd(ch, cmd, param);
					}
				}

				trackpos++;
				trackpos &= 31;
			}
		}
	}

	for(ch = 0; ch < 4; ch++) {
		int16_t vol;
		uint16_t duty;
		uint16_t slur;

		while(channel[ch].inum && !channel[ch].iwait) {
			uint8_t il[2];

			readinstr(channel[ch].inum, channel[ch].iptr, il);
			channel[ch].iptr++;

			runcmd(ch, il[0], il[1]);
		}
		if(channel[ch].iwait) channel[ch].iwait--;

		if(channel[ch].inertia) {
			int16_t diff;

			slur = channel[ch].slur;
			diff = freqtable[channel[ch].inote] - slur;
			//diff >>= channel[ch].inertia;
			if(diff > 0) {
				if(diff > channel[ch].inertia) diff = channel[ch].inertia;
			} else if(diff < 0) {
				if(diff < -channel[ch].inertia) diff = -channel[ch].inertia;
			}
			slur += diff;
			channel[ch].slur = slur;
		} else {
			slur = freqtable[channel[ch].inote];
		}
		osc[ch].freq =
			slur +
			channel[ch].bend +
			((channel[ch].vdepth * sinetable[channel[ch].vpos & 63]) >> 2);
		channel[ch].bend += channel[ch].bendd;
		vol = osc[ch].volume + channel[ch].volumed;
		if(vol < 0) vol = 0;
		if(vol > 255) vol = 255;
		osc[ch].volume = vol;

		duty = osc[ch].duty + channel[ch].dutyd;
		if(duty > 0xe000) duty = 0x2000;
		if(duty < 0x2000) duty = 0xe000;
		osc[ch].duty = duty;

		channel[ch].vpos += channel[ch].vrate;
	}

	uint8_t lights = 0;
	for(uint8_t i=0; i<8; i++){
		if(light[i]) {
			light[i]--;
			lights |= _BV(i);
		}
	}

//	Display_SetBar(lights);//Skips a bit during the initial buffer filling
}

void initresources() {
	struct unpacker up;

	resources = (uint16_t*) pvPortMalloc2(16 + ((Chiptune*)currentSong.data)->maxtrack);
	if(resources == NULL){
		playing = 0;
		xputs("Audio: Chiptune failed to allocate resources.");
		return;
	}
	initup(&up, 0);
	for(uint8_t i = 0; i < 16 + ((Chiptune*)currentSong.data)->maxtrack; i++) {
		((uint16_t*)resources)[i] = readchunk(&up, 13);
	}

	initup(&chiptuneUP, ((uint16_t*)resources)[0]);
}

void Audio_Chiptune_Setup(){//TODO there is something strange with the first seconds of sound
	trackwait = 0;
	trackpos = 0;

	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;

	Audio_SetFrequency(CHIPTUNE_FREQ);

	playing = 1;
	initresources();
}

uint16_t Audio_Chiptune_Play(){
	uint8_t i;
	int16_t acc;
	static uint32_t noiseseed = 1;
	uint8_t newbit;

	newbit = 0;
	if(noiseseed & 0x80000000L) newbit ^= 1;
	if(noiseseed & 0x01000000L) newbit ^= 1;
	if(noiseseed & 0x00000040L) newbit ^= 1;
	if(noiseseed & 0x00000200L) newbit ^= 1;
	noiseseed = (noiseseed << 1) | newbit;

	if(callbackwait) {
		callbackwait--;
	} else {
		playroutine();
		callbackwait = 180 - 1;
	}

	acc = 0;
	for(i = 0; i < 4; i++) {
		int8_t value; // [-32,31]

		switch(osc[i].waveform) {
			case WF_TRI:
				if(osc[i].phase < 0x8000) {
					value = -32 + (osc[i].phase >> 9);
				} else {
					value = 31 - ((osc[i].phase - 0x8000) >> 9);
				}
				break;
			case WF_SAW:
				value = -32 + (osc[i].phase >> 10);
				break;
			case WF_PUL:
				value = (osc[i].phase > osc[i].duty)? -32 : 31;
				break;
			case WF_NOI:
				value = (noiseseed & 63) - 32;
				break;
			default:
				value = 0;
				break;
		}
		osc[i].phase += osc[i].freq;

		acc += value * osc[i].volume; // rhs = [-8160,7905]
	}
//	acc [-32640,31620]
//	write(map(((uint8_t)(128 + (acc >> 8))), 0, 0xFF, 0, 0x3FF)); // [1,251]
	return(512 + (acc >> 6)); // [1,251]
}

void Audio_Chiptune_Cleanup(){
	if(resources != NULL){
		vPortFree2(resources);
		resources = NULL;
	}
}

