/*------------------------------------------------------------------------/
/  Sound Streamer and RIFF-WAVE file player for LPC176x
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "AudioInternal.h"
#include <string.h>
#include "wav.h"
#include "term_io.h"
#include "ff.h"
#include "ff_test_term.h"
#include "dbg.h"

#define NBSIZE 64
#define FCC(c1,c2,c3,c4)	((c4<<24)+(c3<<16)+(c2<<8)+c1)	/* FourCC */

/* Size of buffer (must be power of 2) */
#define BSIZE 512

typedef struct {		/* Sound buffer control block */
	uint8_t buff[BSIZE];		/* Buffer (must be 4-byte aligned) */
	FIL file;
	DWORD offw;
	uint32_t mode;		/* Data format b0: mono(0)/stereo(1), b1: 8bit(0)/16bit(1) */
	uint32_t ri;	/* Buffer read index */
} WAVFIFO;


/*-----------------------------------------------------*/
/* WAV file loader                                     */
int load_wav (
	FIL *fp,			/* Pointer to the open file object to play */
	const char *title	/* Title (file name, etc...) */
)
{
	void *work = ((WAVFIFO*)resources)->buff;
	UINT md, br;
	DWORD sz, ssz, offw, wsmp, fsmp, eof;
	WAVFIFO *fcb = ((WAVFIFO*)resources);
	BYTE *buff = work;
	char *p, nam[NBSIZE], art[NBSIZE];


	xprintf("Playing %s\ ", title);	/* Put title */

	/* Is it a WAV file? */
	if (f_read(fp, buff, 12, &br) || br != 12) return -1;
	if (LD_DWORD(&buff[0]) != FCC('R','I','F','F')) return -2;
	if (LD_DWORD(&buff[8]) != FCC('W','A','V','E')) return -3;
	eof = LD_DWORD(&buff[4]) + 8;

	/* Analyse the RIFF-WAVE header and get properties */
	nam[0] = art[0] = 0;
	md = fsmp = wsmp = offw = 0;
	while (f_tell(fp) < eof) {
		if (f_read(fp, buff, 8, &br) || br != 8) return -4;
		sz = (LD_DWORD(&buff[4]) + 1) & ~1;
		switch (LD_DWORD(&buff[0])) {
		case FCC('f','m','t',' ') :	/* fmt chunk */
			if (sz > 1000 || sz < 16 || f_read(fp, buff, sz, &br) || sz != br) return -5;
			if (LD_WORD(&buff[0]) != 0x1) return -6;	/* Check if LPCM */
			if (LD_WORD(&buff[2]) == 2) {	/* Channels (1 or 2) */
				md = 1; wsmp = 2;
			} else {
				md = 0; wsmp = 1;
			}
			if (LD_WORD(&buff[14]) == 16) {	/* Resolution (8 or 16) */
				md |= 2; wsmp *= 2;
			}
			fsmp = LD_DWORD(&buff[4]);		/* Sampling rate */
			break;

		case FCC('f','a','c','t') :	/* fact chunk */
			f_lseek(fp, f_tell(fp) + sz);
			break;

		case FCC('d','a','t','a') :	/* data chunk */
			offw = f_tell(fp);	/* Wave data start offset */
			length = sz;			/* Wave data length [byte] */
			f_lseek(fp, offw + sz);
			break;

		case FCC('L','I','S','T'):	/* LIST chunk */
			sz += f_tell(fp);
			if (f_read(fp, buff, 4, &br) || br != 4) return -7;
			if (LD_DWORD(buff) == FCC('I','N','F','O')) {	/* LIST/INFO sub-chunk */
				while (f_tell(fp) < sz) {
					if (f_read(fp, buff, 8, &br) || br != 8) return -8;
					ssz = (LD_DWORD(&buff[4]) + 1) & ~1;
					p = 0;
					switch (LD_DWORD(buff)) {
					case FCC('I','N','A','M'):		/* INAM field */
						p = nam; break;
					case FCC('I','A','R','T'):		/* IART field */
						p = art; break;
					}
					if (p && ssz <= NBSIZE) {
						if (f_read(fp, p, ssz, &br) || br != ssz) return -9;
					} else {
						if (f_lseek(fp, f_tell(fp) + ssz)) return -10;
					}
				}
			} else {
				if (f_lseek(fp, sz)) return -11;	/* Skip unknown sub-chunk type */
			}
			break;

		default :	/* Unknown chunk */
//			debug("Unknown chunk: %c%c%c%c . Skipping.", buff[0], buff[1], buff[2], buff[3]);
			f_lseek(fp, f_tell(fp) + LD_DWORD(&buff[4]));
//			return -12;
			break;
		}
	}
	if (!length || !fsmp) return -13;		/* Check if valid WAV file */
	if (f_lseek(fp, offw)) return -14;	/* Seek to top of WAV data */
	fcb->ri = BSIZE;
	uint16_t tc = (UINT)(length / fsmp / wsmp);	/* Length (sec) */

//	xprintf("IART=%s\nINAM=%s\n", art, nam);
	xprintf("%u:%02u %lu.%lukHz/%ubit/%s\n", tc / 60, tc % 60, fsmp / 1000, (fsmp / 100) % 10, (md & 2) ? 16 : 8, (md & 1) ? "Stereo" : "Mono");

	/* Initialise stream parameters and start sound streaming */
	fcb->mode = md;			/* Sampling: b0=mono(0)/stereo(1), b1=8bit(0)/16bit(1) */

	if (fsmp < 8000 || fsmp > 48000) return -15;	/* Check fs range */
	Audio_SetFrequency(fsmp);

	xputs("\n");
	fcb->offw = offw;
	return 0;
}

void Audio_WAV_Setup(){
	resources = pvPortMalloc2(sizeof(WAVFIFO));

	if(resources == NULL){
		xputs("Audio: WAV failed to allocate resources.\n");
		return;
	}
	memset(resources, 0, sizeof(WAVFIFO));

	WAVFIFO *fcb = (WAVFIFO*) resources;

	const TCHAR *path	= currentSong.data;
	FRESULT res = f_open(&fcb->file, path, FA_READ);
	if (res) {
		put_rc(res);
	} else {
		int ret = load_wav(&fcb->file, path);
		if(ret == 0){
			playing = 1;
		}else{
			xprintf("Failed to load WAV file. ret=%d", ret);
		}
	}
}

uint16_t Audio_WAV_Play(){
	WAVFIFO *fcb = (WAVFIFO*) resources;
	uint8_t *buff = fcb->buff;
	uint32_t ri = fcb->ri;

	replay:
	if (position < length) {
		if (ri == BSIZE) {
			UINT br, btr = (length - position) > BSIZE ? BSIZE : (length - position);
			FRESULT fr = f_read(&fcb->file, buff, btr, &br);
			if(fr){
				xprintf("File error while playing WAV file. ");
				put_rc(fr);
				playing = 0;	/* Exit if illegal file termination or FS error */
				return 0;
			}
			if (br != btr){
				debug("br=%d btr=%d diff=%ld p=%lu l=%lu\n", br, btr, length - position, position, length);
				playing = 0;
				return 0;
			}
			ri = 0;
		}

		uint32_t l, r;

		switch (fcb->mode) {
			case 0:		/* Mono, 8bit */
				l = r = buff[ri++] << 8;
				break;
			case 1:		/* Stereo, 8bit */
				l = buff[ri++] << 8;
				r = buff[ri++] << 8;
				break;
			case 2:		/* Mono, 16bit */
				l = r = LD_WORD(buff+ri) ^ 0x8000;
				ri += 2;
				break;
			default:	/* Stereo, 16bit */
				l = LD_WORD(buff+ri) ^ 0x8000;;
				r = LD_WORD(buff+ri+2) ^ 0x8000;;
				ri += 4;
				break;
			}
			fcb->ri = ri;

			switch (fcb->mode) {
			case 0:		/* Mono, 8bit */
				position += 1;
				break;
			case 1:		/* Stereo, 8bit */
			case 2:		/* Mono, 16bit */
				position += 2;
				break;
			default:	/* Stereo, 16bit */
				position += 4;
				break;
			}

			l >>= 6; r >>= 6;
			return (l + r) / 2;
	}else{
		if(currentSong.loops > 0){
//			xprintf("Looping %u\n", currentSong.loops);
			currentSong.loops--;
			position = 0;
			ri = BSIZE;
			FRESULT fr = f_lseek(&fcb->file, fcb->offw);/* Seek to top of WAV data */
			if (fr){
				xprintf("File error while seeking WAV file. ");
				put_rc(fr);
				playing = 0;
				return 0;
			}
			goto replay;
		}else{
			playing = 0;
			return 0;
		}
	}
}

void Audio_WAV_Cleanup(){
	if(resources != NULL){
		FIL *f = &((WAVFIFO*)resources)->file;
		if(f){
			f_close(f);
		}

		vPortFree2(resources);
		resources = NULL;
	}

	if(currentSong.data != NULL && currentSong.data >= 0x10000000U){
		vPortFree2(currentSong.data);
	}
}



