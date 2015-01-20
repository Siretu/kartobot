#include "delays.i"

		.global	vblank
		.global	vbl_scroll
		.global	vbl_column
		.global	vbl_column_r

vblank:

		rcall	playroutine

		lds	r16, timer3
		subi	r16, 256-3
		sts	timer3, r16

		lds	r16, vblvector

		ldi	r30, lo8(pm(jumptable))
		ldi	r31, hi8(pm(jumptable))
		add	r30, r16
		adc	r31, r1
		ijmp

vbl_column_r:
		lds	r16, copperlist+1
		subi	r16, 2
		brlo	1f
		sts	copperlist+1, r16
		sts	copperlist+3, r16
1:

vbl_column:
		lds	r30, framecount
		ldi	r31, hi8(sine)
		lpm	r16, Z
		asr	r16
		sts	twist, r16

		subi	r30, 256-64
		ldi	r31, hi8(sine)
		lpm	r16, Z
		asr	r16
		asr	r16
		sts	xoffset, r16

		subi	r30, 42
		lpm	r16, Z
		asr	r16
		ldi	r17, 191
		add	r17, r16
		sts	copperlist+2*2+1, r17
		sub	r17, r16
		sub	r17, r16
		sts	copperlist+8*2+1, r17

		ldi	r26, lo8(texty)
		ldi	r27, hi8(texty)
		ldi	r31, hi8(sine)
		ldi	r18, 25
		lds	r30, framecount
		lsl	r30
		lsl	r30
1:
		subi	r30, 8
		lpm	r16, Z
		asr	r16
		asr	r16
		asr	r16
		asr	r16
		andi	r16, 0xfe
		st	X+, r16
		dec	r18
		brne	1b

vbl_scroll:
		lds	r16, scrollx
		subi	r16, 1
		brsh	4f

		ldi	r28, lo8(text)
		ldi	r29, hi8(text)
		ldi	r18, 24
5:		ldd	r16, Y+1
		st	Y+, r16
		dec	r18
		brne	5b

		lds	r30, scrollptr
		lds	r31, scrollptr+1
		lds	r16, scrollbits
		ldi	r18, 5
		rcall	readchunk
		cpi	r17, 31
		brne	6f
		ldi	r16, 0xff
		ldi	r17, 0
6:
		sts	scrollbits, r16
		sts	scrollptr, r30
		sts	scrollptr+1, r31
		st	Y, r17

		ldi	r16, 0x05

4:		sts	scrollx, r16
		ret

; obsolete stuff follows


vbl_text1:
		;ser	r16
		;lds	r30, framecount
		;lsl	r30
		;lsl	r30
		;sub	r16, r30
		;sts	copp1+6*3+1, r16
		;sts	copp1+8*3+1, r16

		;lds	r30, framecount
		;lsl	r30
		;ldi	r31, hi8(sine)
		;andi	r30, 127
		;lpm	r16, Z
		;asr	r16
		;asr	r16
		;ldi	r17, 32
		;sub	r17, r16
		;sts	horizon, r17

		;lds	r30, framecount
		;subi	r30, 64
		;ldi	r31, hi8(sine)
		;lpm	r16, Z
		;asr	r16
		;asr	r16
		;asr	r16

		;ldi	r17, 40-1
		;add	r17, r16
		;sts	copp1+5*3+2, r17
		;ldi	r17, 40-1
		;sub	r17, r16
		;sts	copp1+9*3+2, r17

		;ret

