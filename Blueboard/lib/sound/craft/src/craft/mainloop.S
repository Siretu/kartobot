#include "stuff.i"

		.global	waitvbl
		.global	mainloop
		.global	infloop
		.global	initcolumn
		.global	initgreetings
		.global	blankon
		.global	blankoff

waitvbl:
		sbi	GPIOR0, GFLAG_VBLANK
1:		sbic	GPIOR0, GFLAG_VBLANK
		rjmp	1b
		ret

infloop:	rjmp	infloop

callvector:
		ldi	r30, lo8(pm(jumptable))
		ldi	r31, hi8(pm(jumptable))
		add	r30, r17
		adc	r31, r1
		ijmp

mainloop:
		eor	r1, r1

		lds	r16, storywait
		lds	r17, storywait+1
		cli
		lds	r18, framecount
		lds	r19, framecount+1
		sei

		cp	r18, r16
		cpc	r19, r17
		brsh	ml_fetch

		lds	r17, mlvector
		rcall	callvector
		rjmp	mainloop

ml_fetch:
		lds	r30, storyptr
		lds	r31, storyptr+1
		lds	r16, storybits
2:
		ldi	r18, 3
		rcall	readchunk

		cpi	r17, 0			; wait
		brne	3f

		ldi	r18, 6
		rcall	readchunk
		sts	storywait+1, r17
		ldi	r18, 5
		rcall	readchunk
		lsl	r17
		lsl	r17
		lsl	r17
		sts	storywait, r17

		sts	storybits, r16
		sts	storyptr, r30
		sts	storyptr+1, r31

		rjmp	mainloop
3:
		cpi	r17, 1			; call
		brne	3f

		ldi	r18, 6
		rcall	readchunk

		sts	storybits, r16
		sts	storyptr, r30
		sts	storyptr+1, r31
		
		rcall	callvector
		rjmp	ml_fetch
3:
		cpi	r17, 2			; mainloop
		brne	3f

		ldi	r18, 6
		rcall	readchunk
		sts	mlvector, r17
		rjmp	2b
3:
		cpi	r17, 3			; vblank
		brne	3f

		ldi	r18, 6
		rcall	readchunk
		sts	vblvector, r17
		rjmp	2b
3:
		cpi	r17, 4			; palette
		brne	3f

		ldi	r18, 6
		rcall	readchunk
		sts	palette+0, r17
		ldi	r18, 6
		rcall	readchunk
		sts	palette+1, r17
		ldi	r18, 6
		rcall	readchunk
		sts	palette+2, r17
		ldi	r18, 6
		rcall	readchunk
		sts	palette+3, r17
		rjmp	2b
3:
		; assume 5, copper

		ldi	r26, lo8(copperlist)
		ldi	r27, hi8(copperlist)
6:
		ldi	r18, 6
		rcall	readchunk
		tst	r17
		breq	5f

		st	X+, r17
		ldi	r18, 8
		rcall	readchunk
		st	X+, r17
		rjmp	6b
5:
		st	X+, r1
		st	X+, r1
		rjmp	2b


initgreetings:
		ldi	r16, hi8(scroll2)
		sts	scrollptr+1, r16
		ldi	r16, lo8(scroll2)
		sts	scrollptr, r16
		sts	scrollbits, r1
		ret

initcolumn:
		ldi	r16, 0x11
		sts	bg, r16
		ldi	r16, 5
		sts	scrollx, r16
		rjmp	blankoff

blankon:
		sbi	GPIOR0, GFLAG_BLANK
		ret

blankoff:
		cbi	GPIOR0, GFLAG_BLANK
		ret
