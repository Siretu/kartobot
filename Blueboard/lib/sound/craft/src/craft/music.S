
#include "stuff.i"

		.global	m_song
		.global	m_trackdata
		.global	freq
		.global	playroutine
		.global	plonk
		.global	readchunk
		.global	readchunkee

readchunk:	; Read r18 bits from the unpacker represented by a pointer in Z and a bit buffer in r16.
		; Output in r17.

		mov	r17, r1

2:		lsl	r16
		brne	1f

		lpm	r16, Z+
		sec
		rol	r16

1:		rol	r17
		subi	r18, 1
		brne	2b

		ret

readchunkee:	; Read some bits from the unpacker represented by an eeprom pointer in Z and a bit buffer in r16.
		; Input is r17, which should be 0x100 >> number_of_bits.
		; Output in r17.

2:		lsl	r16
		brne	1f

		rcall	eefromz
		in	r16, EEDR
		adiw	r30, 1
		sec
		rol	r16

1:		rol	r17
		brcc	2b

		ret


runcmd:		; run instrument command r16 with channel data at Y

		mov	r19, r16
		andi	r19, 0x0f
		mov	r17, r19
		swap	r17
		andi	r16, 0xf0

		;cpi	r16, 0x00
		brne	1f
		ldd	r18, Y+c_iloop
		std	Y+c_iptr, r18
		ret
1:
		cpi	r16, 0x10
		brne	1f
		ldd	r18, Y+c_iptr
		std	Y+c_iloop, r18
		ret
1:
		cpi	r16, 0x30
		brne	1f
		std	Y+c_bendd, r17
		ret
1:
		cpi	r16, 0x40
		brne	1f
		lsr	r17
		std	Y+c_vrate, r17
		ret
1:
		cpi	r16, 0x50
		brne	1f
		std	Y+c_vdepth, r17
		ret
1:
		cpi	r16, 0x60
		brne	1f
		lsl	r19
		std	Y+c_vol, r19
		ret
1:
		cpi	r16, 0x70
		brne	1f
		sbrc	r19, 3
		ori	r19, 0xf0
		std	Y+c_vold, r19
		ret
1:
		cpi	r16, 0x90
		brne	1f
		mov	r16, r19
		lsl	r16
		andi	r16, 0x18
		andi	r19, 3
		ldi	r17, 4
		sub	r17, r19
		lds	r18, noisevolume
		tst	r18
		breq	2f
		lds	r18, noiserelease
		cp	r17, r18
		brsh	3f
2:		sts	noisevolume, r16
		sts	noiserelease, r17
3:		ret
1:
		cpi	r16, 0xa0
		brne	1f
		ldd	r17, Y+c_tnote
		add	r17, r19
		std	Y+c_inote+1, r17
		ret
1:
		cpi	r16, 0xb0
		brne	1f
		ldd	r17, Y+c_tnote
		sub	r17, r19
		std	Y+c_inote+1, r17
		ret
1:
		cpi	r16, 0xd0
		brne	1f
		std	Y+c_timer, r19
		ret
1:
		cpi	r16, 0xe0
		brne	1f
		std	Y+c_timer, r17
1:
		;cpi	r16, 0xf0
		;brne	1f
		sbi	GPIOR0, GFLAG_SONGEND
1:
		ret

plonk:		; play note r16 using instrument ptr r17 with channel data at Y

		std	Y+c_vol, r1
		std	Y+c_iptr, r17
		std	Y+c_iloop, r1
		std	Y+c_timer, r1
		std	Y+c_tnote, r16
		std	Y+c_inote, r1
		std	Y+c_inote+1, r16
		std	Y+c_bendd, r1
		std	Y+c_vold, r1
		ldi	r17, 24
		std	Y+c_vrate, r17
		std	Y+c_vdepth, r1
		ret

playroutine:

		sbic	GPIOR0, GFLAG_SONGEND
		rjmp	play_sound

		lds	r16, tracktimer
		subi	r16, 1
		brlo	1f
		rjmp	play_nonewline
1:
		lds	r16, trackpos
		subi	r16, 1
		brlo	2f
		rjmp	play_nonewpos
2:

		; new track

		ldi	r28, lo8(ch)		; loop through channels with Y as channel pointer
		ldi	r29, hi8(ch)
play_songch:
		lds	r30, songptr
		lds	r31, songptr+1
		lds	r16, songbits

		ldi	r17, 0x04		; 0x100 >> tracknumbits
		rcall	readchunkee

		sts	songptr, r30
		sts	songptr+1, r31
		sts	songbits, r16

		mov	r16, r1
		tst	r17
		breq	4f

		subi	r17, 1
		lsr	r17			; r17 is tracknum/2, so r17 * 3 gives a byte offset into tracktab
		rol	r16			; lsb of r16 is tracknum & 1, and tells us which part of the 3-byte struct to use

		ldi	r30, lo8(m_tracktab)
		ldi	r31, hi8(m_tracktab)
		add	r30, r17
		adc	r31, r1
		lsl	r17
		add	r30, r17
		adc	r31, r1

		andi	r16, 1
		breq	1f
2:
		; use second half of struct

		adiw	r30, 1
		rcall	eefromz
		in	r17, EEDR
		adiw	r30, 1
		rcall	eefromz
		in	r16, EEDR

		;lpm	r17, Z+
		;lpm	r17, Z+			; nibbles (x, 2)
		;lpm	r16, Z			; nibbles (1, 0)
		rjmp	3f
1:
		; use first half of struct

		rcall	eefromz
		in	r16, EEDR
		adiw	r30, 1
		rcall	eefromz
		in	r17, EEDR

		;lpm	r16, Z+			; nibbles (1, 0)
		;lpm	r17, Z			; nibbles (2, x)
		swap	r17
3:
		andi	r17, 0x0f
		ldi	r18, lo8(m_trackdata)
		ldi	r19, hi8(m_trackdata)
		add	r16, r18
		adc	r17, r19
4:		std	Y+c_tptr, r16
		std	Y+c_tptr+1, r17
		std	Y+c_tbits, r1

		ldi	r16, c_size
		add	r28, r16
		adc	r29, r1
		cpi	r28, lo8(ch+3*c_size)
		brne	play_songch

		ldi	r16, 32-1		; tracklen
play_nonewpos:
		sts	trackpos, r16

		; new line

		ldi	r28, lo8(ch)		; loop through channels with Y as channel pointer
		ldi	r29, hi8(ch)
play_linech:
		ldd	r30, Y+c_tptr
		ldd	r31, Y+c_tptr+1

		mov	r16, r30
		or	r16, r31
		breq	play_notrack

		ldd	r16, Y+c_tbits

		ldi	r18, 2
		rcall	readchunk
		mov	r19, r17

		sbrs	r19, 1
		rjmp	play_noinstr

		ldi	r18, 5			; instrbits
		rcall	readchunk

		movw	r26, r30
		ldi	r30, lo8(m_instrtab-1)
		ldi	r31, hi8(m_instrtab-1)
		add	r30, r17
		adc	r31, r1
		;lpm	r17, Z			; get iptr for the specified inum

		rcall	eefromz
		in	r17, EEDR

		movw	r30, r26

		std	Y+c_lasti, r17
		std	Y+c_iptr, r17
		std	Y+c_iloop, r1
		std	Y+c_timer, r1

play_noinstr:
		sbrs	r19, 0
		rjmp	play_nonote

		ldi	r18, 7			; notebits
		rcall	readchunk

		mov	r20, r16
		mov	r16, r17
		subi	r16, 1
		ldd	r17, Y+c_lasti
		rcall	plonk
		mov	r16, r20

play_nonote:
		std	Y+c_tbits, r16
		std	Y+c_tptr, r30
		std	Y+c_tptr+1, r31
play_notrack:
		ldi	r16, c_size
		add	r28, r16
		adc	r29, r1
		cpi	r28, lo8(ch+3*c_size)
		brne	play_linech

		ldi	r16, 4-1		; tempo
play_nonewline:
		sts	tracktimer, r16

play_sound:

		ldi	r28, lo8(ch)		; loop through channels with Y as channel pointer
		ldi	r29, hi8(ch)
play_ch:
		ldd	r30, Y+c_iptr		; playing an instrument?
		ldd	r17, Y+c_timer		; waiting?
		subi	r30, 1
		brcs	play_idone
		tst	r17
		brne	play_idone

		;ldi	r30, lo8(m_instrdata-1)	; time to execute an instrument instruction
		;ldi	r31, hi8(m_instrdata-1)
		;add	r30, r18
		;adc	r31, r1

		mov	r31, r1

		rcall	eefromz
		in	r16, EEDR

		;lpm	r16, Z
		subi	r30, 256-2
		std	Y+c_iptr, r30

		rcall	runcmd
		rjmp	play_ch

play_idone:
		subi	r17, 1			; decrement timer
		brlo	1f
		std	Y+c_timer, r17
1:
		ldd	r18, Y+c_inote		; here's our note value in 8.8 format
		ldd	r19, Y+c_inote+1

		ldd	r30, Y+c_vpos
		ldi	r31, hi8(sine)
		lpm	r17, Z			; r17 is sin(vibrpos)
		ldd	r16, Y+c_vdepth
		mulsu	r17, r16		; r1:r0 is wanted vibrato offset times 64
		eor	r16, r16
		sbrc	r1, 7
		ser	r16
		rol	r0
		rol	r1
		rol	r16
		rol	r0
		rol	r1			; r16:r1 is wanted vibrato offset
		rol	r16

		add	r18, r1			; update note
		adc	r19, r16
		eor	r1, r1

		ldd	r16, Y+c_vrate
		add	r30, r16
		std	Y+c_vpos, r30

		ldi	r30, lo8(freq)
		ldi	r31, hi8(freq)
		lsl	r19
		add	r30, r19
		adc	r31, r1
		lpm	r20, Z+			; r21:r20 is freq
		lpm	r21, Z+
		lpm	r22, Z+
		lpm	r23, Z+
		sub	r22, r20
		sbc	r23, r21		; r23:r22 is dist

		; Now we want to compute freq += (dist * lo(note)) >> 8.
		; The offset to add to freq will be stored in r25:r24.

		mul	r23, r18		; r1:r0 is product of hi(dist) and lo(note)
		movw	r24, r0
		mul	r22, r18		; r1:r0 is product of lo(dist) and lo(note)
		add	r24, r1
		eor	r1, r1
		adc	r25, r1

		add	r20, r24
		adc	r21, r25

		; r21:r20 is freq. Now r22, r23, r24 and r25 are free again.

		ldd	r18, Y+c_inote		; here's our note value in 8.8 format
		ldd	r19, Y+c_inote+1
		ldd	r16, Y+c_bendd
		eor	r17, r17
		sbrc	r16, 7
		ser	r17
		add	r18, r16
		adc	r19, r17
		add	r18, r16
		adc	r19, r17
		sbrc	r19, 7
		mov	r18, r1
		sbrc	r19, 7
		mov	r19, r1
		std	Y+c_inote, r18
		std	Y+c_inote+1, r19

		ldd	r16, Y+c_vol
		ldd	r17, Y+c_vold
		add	r16, r17
		sbrc	r16, 7
		ldi	r16, 0
		sbrc	r16, 5
		ldi	r16, 31

		tst	r16
		brne	2f
		mov	r21, r1
		mov	r20, r1
2:
		std	Y+c_freq+1, r21
		std	Y+c_freq, r20
		std	Y+c_vol, r16

		ldi	r16, c_size
		add	r28, r16
		adc	r29, r1
		cpi	r28, lo8(ch+3*c_size)
		breq	3f
		rjmp	play_ch
3:
		lds	r16, noisevolume
		lds	r17, noiserelease
		sub	r16, r17
		sbrc	r16, 7
		mov	r16, r1
		sts	noisevolume, r16
		
		ret



freq:
	;.word	0x0044, 0x0048, 0x004c, 0x0050, 0x0055, 0x005a, 0x0060, 0x0065
	;.word	0x006c, 0x0072, 0x0079, 0x0080, 0x0088, 0x0090, 0x0098, 0x00a1
	;.word	0x00ab, 0x00b5, 0x00c0, 0x00cb, 0x00d8, 0x00e4, 0x00f2, 0x0100
	;.word	0x0110, 0x0120, 0x0131, 0x0143, 0x0156, 0x016b, 0x0180, 0x0197
	;.word	0x01b0, 0x01c9, 0x01e4, 0x0201, 0x0220, 0x0240, 0x0263, 0x0287
	;.word	0x02ad, 0x02d6, 0x0301, 0x032f, 0x0360, 0x0393, 0x03c9, 0x0403
	;.word	0x0440, 0x0481, 0x04c6, 0x050e, 0x055b, 0x05ad, 0x0603, 0x065f
	;.word	0x06c0, 0x0727, 0x0793, 0x0807, 0x0881, 0x0903, 0x098c, 0x0a1d
	;.word	0x0ab7, 0x0b5a, 0x0c07, 0x0cbe, 0x0d80, 0x0e4e, 0x0f27, 0x100e
	;.word	0x1103, 0x1206, 0x1318, 0x143b, 0x156f, 0x16b5, 0x180e, 0x197d
	;.word	0x1b01, 0x1c9c, 0x1e4f, 0x201d


	;.word	0x0040, 0x0044, 0x0048, 0x004d, 0x0051, 0x0056, 0x005b, 0x0061
	;.word	0x0067, 0x006d, 0x0073, 0x007a, 0x0081, 0x0089, 0x0091, 0x009a
	;.word	0x00a3, 0x00ad, 0x00b7, 0x00c2, 0x00ce, 0x00da, 0x00e7, 0x00f5
	;.word	0x0103, 0x0113, 0x0123, 0x0134, 0x0147, 0x015a, 0x016f, 0x0185
	;.word	0x019c, 0x01b4, 0x01ce, 0x01ea, 0x0207, 0x0226, 0x0247, 0x0269
	;.word	0x028e, 0x02b5, 0x02de, 0x030a, 0x0338, 0x0369, 0x039d, 0x03d4
	;.word	0x040f, 0x044d, 0x048e, 0x04d3, 0x051d, 0x056b, 0x05bd, 0x0615
	;.word	0x0671, 0x06d3, 0x073b, 0x07a9, 0x081e, 0x089a, 0x091d, 0x09a7
	;.word	0x0a3a, 0x0ad6, 0x0b7b, 0x0c2a, 0x0ce3, 0x0da7, 0x0e77, 0x0f53
	;.word	0x103d, 0x1134, 0x123a, 0x134f, 0x1475, 0x15ad, 0x16f7, 0x1854
	;.word	0x19c6, 0x1b4f, 0x1cef, 0x1ea7



	.word	0x004a, 0x004e, 0x0053, 0x0058, 0x005d, 0x0063, 0x0068, 0x006f
	.word	0x0075, 0x007c, 0x0084, 0x008c, 0x0094, 0x009d, 0x00a6, 0x00b0
	.word	0x00bb, 0x00c6, 0x00d1, 0x00de, 0x00eb, 0x00f9, 0x0108, 0x0118
	.word	0x0128, 0x013a, 0x014d, 0x0161, 0x0176, 0x018c, 0x01a3, 0x01bc
	.word	0x01d7, 0x01f3, 0x0211, 0x0230, 0x0251, 0x0275, 0x029a, 0x02c2
	.word	0x02ec, 0x0318, 0x0347, 0x0379, 0x03ae, 0x03e6, 0x0422, 0x0461
	.word	0x04a3, 0x04ea, 0x0535, 0x0584, 0x05d8, 0x0631, 0x068f, 0x06f3
	.word	0x075d, 0x07cd, 0x0844, 0x08c2, 0x0947, 0x09d4, 0x0a6a, 0x0b08
	.word	0x0bb0, 0x0c62, 0x0d1f, 0x0de7, 0x0eba, 0x0f9b, 0x1088, 0x1184
	.word	0x128e, 0x13a9, 0x14d4, 0x1611, 0x1761, 0x18c5, 0x1a3e, 0x1bce
	.word	0x1d75, 0x1f36, 0x2111, 0x2308
