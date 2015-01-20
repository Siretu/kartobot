
		.global	main
		.global	__vectors
		.global	delayxplus16

__vectors:
		rjmp	main

		; exactly 10 words

delayxplus16:	; 3 + 13 + x

1:		subi	r16, 3			;				1   \_ 3 * (x / 3) + 2
		brsh	1b			;				1/2 /
		neg	r16			; [1,2,3], inverse of time left	1
		sbrs	r16, 0			;				1/2 \_ 2 if set, 3 if clear
		rjmp	.			;				2   /
		sbrs	r16, 1			;				1/2 \_ 2 if set, 3 if clear
		rjmp .				;				2   /
		sbrs	r16, 1			;				1/2 \_ 2 if set, 3 if clear
		rjmp .				;				2   /
		ret				;				4

		rjmp	__vector_11

main:
		eor	r1, r1

		ldi	r30, lo8(pokelist)
		ldi	r31, hi8(pokelist)
		ldi	r27, 0
1:
		lpm	r26, Z+
		tst	r26
		breq	2f
		lpm	r16, Z+
		st	X, r16
		rjmp	1b
2:
		ldi	r27, 0x01
3:
		st	X+, r1
		cpi	r27, 0x05
		brne	3b

asminit:
		eor	r4, r4
		eor	r5, r5
		inc	r4
		eor	r2, r2
		inc	r2
		mov	r3, r2
		inc	r3

		ldi	r16, lo8(story)
		sts	storyptr, r16
		ldi	r16, hi8(story)
		sts	storyptr+1, r16
		ldi	r16, lo8(m_song)
		sts	songptr, r16
		ldi	r16, hi8(m_song)
		sts	songptr+1, r16

		sts	vblvector, r2
		sts	mlvector, r2

		; unpack title graphics

		ldi	r27, 0x03
		ldi	r26, 0
		ldi	r30, lo8(craftgfx)
		ldi	r31, hi8(craftgfx)

2:
		lpm	r16, Z+
		st	X+, r16
		cpi	r26, 12*14
		brne	2b

		sei
		rjmp	mainloop

pokelist:
		.byte	0x5e,	0x02	// SPH
		.byte	0x5d,	0xff	// SPL

		.byte	0x24,	0x0e	// DDRB
		.byte	0x25,	0x02	// PORTB
		.byte	0x27,	0x3f	// DDRC
		.byte	0x2a,	0xff	// DDRD

		.byte	0x4c,	0x50	// SPCR
		.byte	0x4d,	0x01	// SPSR

		.byte	0x6f,	0x02	// TIMSK1
		.byte	0x80,	0x33	// TCCR1A
		.byte	0x81,	0x19	// TCCR1B
		.byte	0x89,	0x02	// OCR1AH
		.byte	0x88,	0x7a	// OCR1AL
		.byte	0x8b,	0x00	// OCR1BH
		.byte	0x8a,	0x4b	// OCR1BL

		.byte	0xb0,	0x33	// TCCR2A
		.byte	0xb1,	0x0a	// TCCR2B

		.byte	0, 0
