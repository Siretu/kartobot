/*
 * code.c
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#include "AudioInternal.h"
#include "code.h"

void Audio_Code_Setup(){
	uint32_t c = ((uint32_t)currentSong.data);
	if(c == 7){
		Audio_SetFrequency(44100);
	}else if(c == 9){
		Audio_SetFrequency(32000);
	}else{
		Audio_SetFrequency(8000);
	}

	playing = 1;
}

uint16_t Audio_Code_Play(){
	#define t position
	uint32_t c = ((uint32_t)currentSong.data);
	uint16_t data = 0xFFFF;

	//default 8bit 8kHz
	if(c == 1){//miiro
		data = ( t*5 &(t>>7) ) | ( t*3 &(t*4 >>10) );

	}else if(c == 2){//"the 42 melody"
		data = t*(42&t>>10);

	}else if(c == 3){//viznut
		data = t*((t>>12|t>>8)&63&t>>4);

	}else if(c == 4){//red-
		data = (t|(t>>9|t>>7))*t&(t>>11|t>>9);

	}else if(c == 5){//skurk
		data = ((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t>>2:t));

	}else if(c == 6){//bst
		data = (int32_t)(t/1e7*t*t+t)%127|t>>4|t>>5|t%127+(t>>16)|t;

	}else if(c == 7){//ryg 44.1kHz
		data = ((t*("36364689"[t>>13&7]&15))/12&128)+(((((t>>12)^(t>>12)-2)%11*t)/4|t>>13)&127);

	}else if(c == 8){//stepth
		data = (t*9&t>>4|t*5&t>>7|t*3&t/1024)-1;

	}else if(c == 9){//mu6e 32.0 kHz
		uint32_t y=t&16383, x=t*"6689"[t>>16&3]/24&127;
		data = ((int32_t)(3e3/(y))&1)*35 +x*y/4e4 +((t>>8^t>>10|t>>14|x)&63);//almost works

	}else if(c == 10){//ryg
		data = ((t>>4)*(13&(0x8898a989>>(t>>11&30)))&255)+((((t>>9|(t>>2)|t>>8)*10+4*((t>>2)&t>>15|t>>8))&255)>>1);//not working

	}else{//No code is being used
		playing = 0;
	}

	t++;
	#undef t

//				data = map(data, 0, 0xFF, 0, 0x3FF);
	return((data & 0xFF) << 1);
}

void Audio_Code_Cleanup(){

}

