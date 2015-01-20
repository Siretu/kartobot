
		.global	dotcube
		.global	starfield
		.global	rotate
		.global	divu16u8
		.global	project3

rotate:
		; Rotates (r16, r17) by r30 angular units. Requires hi8(sine) in r31.
		; Clobbers r0, r1, r18, r19, X.

		lpm	r18, Z			; get sine
		subi	r30, 256-64
		lpm	r19, Z			; get cosine

		; xout = x cos - y sin
		; yout = x sin + y cos

		fmuls	r16, r19
		mov	r26, r1
		fmuls	r17, r18
		sub	r26, r1
		fmuls	r16, r18
		mov	r27, r1
		fmuls	r17, r19
		add	r27, r1
		movw	r16, r26

		ret

divu16u8:
		; Divides r1:r0 by r19 and returns the quotient in r25:r24.
		; Clobbers r1, r0, r18 but preserves r19.

		clr	r18
		ldi	r24, 1
		ldi	r25, 0

1:		lsl	r0
		rol	r1
		rol	r18
		brlo	2f
		cp	r18, r19
		brlo	3f
2:		sub	r18, r19
		sec
		rjmp	4f
3:		clc
4:		rol	r24
		rol	r25
		brcc	1b

		ret

project1:
		; Project a dot (r20, r21, r22) into 2d space (r20, r21) by performing z division.
		; Coordinates should be in the range [-96, 96] to fit on the screen.
		; Clobbers r0, r1, r16, r17, r18, r19, r24, r25.

		;ldi	r17, 0x40
		;ldi	r16, 0x00
		ldi	r17, 0x40
		mov	r1, r17
		eor	r0, r0
		ldi	r19, 161
		add	r19, r22
		rcall	divu16u8

		; lsb (fractional part) in r24

		mov	r16, r24

		mulsu	r20, r16
		mov	r20, r1

		mov	r24, r21		; Multiply y coordinate by 1.5 for correct aspect ratio.
		asr	r24			; Conceptually, this is done after the projection, but in order to save
		add	r21, r24		; significant bits we do it before the projection instead.

		mulsu	r21, r16
		mov	r21, r1

		ret

project2:
		; Project a dot (r20, r21, r22) into 2d space (r20, r21) by performing z division.
		; Coordinates should be in the range [-96, 96] but some will not fit on the screen, and will be clipped.
		; Clobbers r0, r1, r16, r17, r18, r19, r24, r25.

		;ldi	r17, 0x40
		;ldi	r16, 0x00
		ldi	r17, 0x40
		mov	r1, r17
		eor	r0, r0
		ldi	r19, 161
		add	r19, r22
		rcall	divu16u8

		; lsb (fractional part) in r24

		mov	r16, r24

		fmulsu	r20, r16
		mov	r20, r1

		; c:r20 is now in [-192, 192], but has to be in [-79, 78] to fit on the screen.
		; Discard the point if c = 0 and r20 > 78 or c = 1 and r20 < 255 - 79.

		brsh	1f

		; Carry was set.
		cpi	r20, 176
		brlo	9f
		rjmp	2f
1:
		; Carry was clear.
		cpi	r20, 79
		brsh	9f
2:

		fmulsu	r21, r16
		mov	r17, r1

		; c:r21 is now in [-192, 192], but has to be in [-79, 78] in [80, 175]) to fit on the screen

		brsh	1f

		; Carry was set.
		cpi	r17, 176
		brlo	9f
		rjmp	2f
1:
		; Carry was clear.
		cpi	r17, 79
		brsh	9f
2:

		mov	r24, r21		; multiply y coordinate by 1.5 for correct aspect ratio
		asr	r24
		add	r21, r24

		fmulsu	r21, r16
		mov	r21, r1

		ret

9:		ldi	r21, 120
		ret

project3:
		; Like project1 but without the aspect correction.

		;ldi	r17, 0x40
		;ldi	r16, 0x00
		ldi	r17, 0x40
		mov	r1, r17
		eor	r0, r0
		ldi	r19, 161
		add	r19, r22
		rcall	divu16u8

		; lsb (fractional part) in r24

		mov	r16, r24

		mulsu	r20, r16
		mov	r20, r1

		mulsu	r21, r16
		mov	r21, r1

		ret

plot2d:
		; Put a dot at (r20, r21) where the high bits of r22 select a colour.
		; Coordinates must satisfy x in [-79, 78], y in [-120, 119] for the dot to be seen.
		; Clobbers r16, r20, r21, r22, X.

		subi	r20, 255-79		; map origo to screen position (79, 120)
		subi	r21, 255-120

		mov	r26, r21

		ldi	r27, 0x03
		st	X, r20

		ldi	r27, 0x04
		mov	r16, r22
		asr	r22
		asr	r22
		add	r16, r22
		subi	r16, 128
		st	X, r16

		ret

clearfield:
		ldi	r27, 0x03
		ldi	r26, 0x00
		ldi	r16, 200

1:		st	X+, r16
		tst	r26
		brne	1b

		ret

dotcube:
		rcall	waitvbl
		rcall	clearfield

		lds	r29, framecount+1
		lsr	r29
		lds	r29, framecount
		ror	r29
		ldi	r23, 35
		ldi	r31, hi8(sine)

		ldi	r22, 3
2:
		push	r22
		mul	r22, r23
		mov	r22, r0
		subi	r22, 53

		ldi	r21, 3
3:
		push	r21
		mul	r21, r23
		mov	r21, r0
		subi	r21, 53

		ldi	r20, 3
4:
		push	r20

		mul	r20, r23
		mov	r20, r0
		subi	r20, 53

		push	r21
		push	r22

		movw	r16, r20
		mov	r30, r29
		add	r30, r29
		rcall	rotate
		mov	r21, r17
		;mov	r20, r16

		;mov	r16, r20
		mov	r17, r22
		mov	r30, r29
		subi	r30, 37
		rcall	rotate
		mov	r20, r16
		mov	r22, r17

		rcall	project1
		subi	r20, 30
		rcall	plot2d

		pop	r22
		pop	r21

		pop	r20

		subi	r20, 1
		brsh	4b

		pop	r21
		subi	r21, 1
		brsh	3b

		pop	r22
		subi	r22, 1
		brlo	6f
		rjmp	2b
6:
		eor	r1, r1
		ret


starfield:
		rcall	waitvbl

		push	r26
		push	r27
		rcall	clearfield
		pop	r27
		pop	r26

		lds	r30, framecount
		lds	r31, framecount+1
		lsr	r31
		ror	r30
		lsr	r31
		ror	r30
		lsr	r31
		ror	r30

		ldi	r31, hi8(sine)
		lpm	r16, Z

		eor	r17, r17
		sbrc	r16, 7
		ser	r17
		add	r26, r16
		adc	r27, r17
		add	r26, r16
		adc	r27, r17

		subi	r30, 256-64
		lpm	r16, Z

		eor	r17, r17
		sbrc	r16, 7
		ser	r17
		add	r28, r16
		adc	r29, r17
		add	r28, r16
		adc	r29, r17
		add	r28, r16
		adc	r29, r17

		mov	r30, r29
		mov	r31, r27

		push	r26
		push	r27

		ldi	r22, 96
1:
		push	r22

		add	r22, r31
		add	r22, r31
		add	r22, r31

		mul	r22, r22
		mov	r20, r0

		subi	r22, 45
		mul	r22, r22
		mov	r21, r0

		pop	r22
		push	r22

		sub	r20, r30
		;sub	r22, r31

		asr	r20
		mov	r16, r20
		asr	r16
		add	r20, r16

		asr	r21
		mov	r16, r21
		asr	r16
		add	r21, r16

		rcall	project2
		rcall	plot2d

		pop	r22
		subi	r22, 3

		cpi	r22, 256-96
		brne	1b

		pop	r27
		pop	r26

		eor	r1, r1
		ret
