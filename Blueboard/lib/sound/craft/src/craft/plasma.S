
		.global	plasma

plasma:
		rcall	waitvbl

		lds	r21, framecount
		lds	r20, framecount+1
		ror	r20
		ror	r21

		ldi	r16, 31			; y
		ldi	r17, 59			; x

		ldi	r26, 0x00
		ldi	r27, 0x03

		ldi	r31, hi8(sine)

1:		rcall	calcpixel
		lsl	r18
		lsl	r18
		mov	r25, r18
		dec	r17
		rcall	calcpixel
		or	r25, r18
		lsl	r25
		lsl	r25
		dec	r17
		rcall	calcpixel
		or	r25, r18
		lsl	r25
		lsl	r25
		dec	r17
		rcall	calcpixel
		or	r25, r18
		st	X+, r25
		subi	r17, 1
		brsh	1b
		eor	r1, r1
		add	r26, r2
		adc	r27, r1
		ldi	r17, 59
		subi	r16, 1
		brsh	1b

		ret

calcpixel:
		movw	r28, r16

		sub	r28, r21
		subi	r29, 14

		muls	r28, r28
		mov	r30, r1
		muls	r29, r29
		add	r30, r1		; r18 = (x*x + y*y) / 256

		lsl	r30
		lsl	r30

		lpm	r18, Z
		asr	r18
		asr	r18

		movw	r28, r16

		sub	r29, r21
		subi	r28, 54

		muls	r28, r28
		mov	r30, r1
		muls	r29, r29
		add	r30, r1		; r18 = (x*x + y*y) / 256

		lsl	r30
		lsl	r30
		lsl	r30

		lpm	r19, Z
		asr	r19
		asr	r19

		add	r18, r19

		movw	r28, r16

		subi	r29, 16
		subi	r28, 97

		muls	r28, r28
		mov	r30, r1
		muls	r29, r29
		add	r30, r1		; r18 = (x*x + y*y) / 256

		lsl	r30
		lsl	r30
		lsl	r30

		lpm	r19, Z
		asr	r19
		asr	r19

		add	r18, r19

		rol	r18
		rol	r18
		rol	r18

		andi	r18, 3

		ret
