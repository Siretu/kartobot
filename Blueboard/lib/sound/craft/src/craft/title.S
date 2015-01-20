#include "stuff.i"

		.global	nextcred
		.global	inittitle
		.global	titlescreen
		.global	enablecolour
		.global	enableflower1
		.global	enableflower2
		.global	enableflower3
		.global	disableflower3

nextcred:
		sts	bg, r1
		lds	r16, copperlist+1
		subi	r16, 256-36
		sts	copperlist+1, r16
		lds	r16, crednum
		ldi	r31, 1
		ldi	r30, lo8(credtable)
		add	r30, r16
		rcall	eefromz
		in	r30, EEDR
		subi	r16, 256-2
		sts	crednum, r16
		rjmp	unpacktitle

clearscroll:
		ldi	r18, 25
		ldi	r26, lo8(texty)
		ldi	r27, hi8(texty)
		ldi	r28, lo8(text)
		ldi	r29, hi8(text)
		ldi	r16, 8
1:
		st	X+, r16
		st	Y+, r1
		subi	r18, 1
		brne	1b

		ret

unpacktitle:
		rcall	clearscroll

		ldi	r16, 0
		ldi	r17, 0x10
		rcall	readchunkee
		mov	r20, r17
		ldi	r26, lo8(text + 12)
		lsr	r17
		sub	r26, r17
2:
		ldi	r17, 0x08
		rcall	readchunkee
		st	X+, r17
		subi	r20, 1
		brne	2b

		ret

inittitle:
		rcall	clearscroll
		sts	bg, r1
		ldi	r16, hi8(scroll1)
		sts	scrollptr+1, r16
		ldi	r16, lo8(scroll1)
		sts	scrollptr, r16
		ret

titlescreen:
		rcall	waitvbl

		ldi	r31, hi8(sine)
		ldi	r21, 11
		ldi	r27, 0x04

		ldi	r26, 0
1:
		st	X+, r1
		tst	r26
		brne	1b

		ldi	r27, 0x04

		lds	r30, framecount+1
		lsr	r30
		lds	r30, framecount
		ror	r30
		lpm	r17, Z
		ldi	r16, 5
		fmuls	r16, r17
		mov	r24, r1

		sbis	GPIOR0, GFLAG_COLOUR
		ldi	r24, 0

		lds	r30, framecount
		lsl	r30

		ldi	r25, 32*7
4:

		ldi	r17, 0x16
		ldi	r18, 0x17
		ldi	r19, 0x2b
		sbrc	r30, 7
		rcall	bar
		subi	r30, 85

		ldi	r17, 0x05
		ldi	r18, 0x0a
		ldi	r19, 0x1f
		sbrc	r30, 7
		rcall	bar
		subi	r30, 85

		ldi	r17, 0x01
		ldi	r18, 0x02
		ldi	r19, 0x16
		sbrc	r30, 7
		rcall	bar
		subi	r30, 86

		ldi	r17, 0x16
		ldi	r18, 0x17
		ldi	r19, 0x2b
		sbrs	r30, 7
		rcall	bar
		subi	r30, 85

		ldi	r17, 0x05
		ldi	r18, 0x0a
		ldi	r19, 0x1f
		sbrs	r30, 7
		rcall	bar
		subi	r30, 85

		ldi	r17, 0x01
		ldi	r18, 0x02
		ldi	r19, 0x16
		sbrs	r30, 7
		rcall	bar
		subi	r30, 86

		sub	r30, r24

		subi	r25, 32
		brsh	4b

		eor	r1, r1
		ret

enablecolour:
		sbi	GPIOR0, GFLAG_COLOUR
		ret

enableflower1:
		sbi	GPIOR0, GFLAG_FLOWER1
		ret

enableflower2:
		sbi	GPIOR0, GFLAG_FLOWER2
		ret

enableflower3:
		sbi	GPIOR0, GFLAG_FLOWER3
		ret

disableflower3:
		cbi	GPIOR0, GFLAG_FLOWER3
		ret

bar:
		sbic	GPIOR0, GFLAG_COLOUR
		rjmp	1f
		ldi	r17, 0x15
		ldi	r18, 0x2a
		ldi	r19, 0x3f
1:
		subi	r30, 64
		lpm	r20, Z
		subi	r30, 256-64
		fmuls	r21, r20
		mov	r26, r1
		subi	r26, 256-16+4
		add	r26, r25
		st	X+, r17
		st	X+, r18
		st	X+, r18
		st	X+, r19
		st	X+, r19
		st	X+, r18
		st	X+, r18
		st	X+, r17
		ret
