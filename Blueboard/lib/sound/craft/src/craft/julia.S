#include "stuff.i"

		.global	julia

dojulia:
		; input: coordinates in (r20, r21)
		; output: low two bits shifted into r28

		push	r20
		push	r21

		ldi	r29, 0
1:
		muls	r20, r20
		movw	r14, r0

		muls	r21, r21
		movw	r18, r0

		movw	r22, r14
		add	r22, r18
		adc	r23, r19
		cpi	r23, 0x10
		brsh	9f

		inc	r29
		sbrc	r29, 3
		rjmp	9f

		muls	r20, r21
		movw	r16, r0
		swap	r16
		swap	r17
		andi	r16, 0x0f
		andi	r17, 0xf0
		or	r16, r17
		mov	r21, r16
		add	r21, r31

		movw	r16, r14
		sub	r16, r18
		sbc	r17, r19
		asr	r17
		ror	r16
		swap	r16
		swap	r17
		andi	r16, 0x0f
		andi	r17, 0xf0
		or	r16, r17
		mov	r20, r16
		add	r20, r30

		rjmp	1b

9:
		andi	r29, 3

		pop	r21
		pop	r20

		lsl	r28
		lsl	r28
		or	r28, r29
		subi	r20, 256-2

		ret


julia:

		ldi	r31, hi8(sine)
		lds	r30, framecount
		lpm	r16, Z
		subi	r30, 256-64
		lpm	r17, Z

		ldi	r18, 42
		fmuls	r16, r18
		mov	r30, r1

		ldi	r18, 8
		fmuls	r17, r18
		ldi	r31, 28
		sub	r31, r1

		ldi	r27, 0x03
		ldi	r26, 0x00

1:
		; use y and x coordinates from r26

		mov	r21, r26
		swap	r21
		andi	r21, 0x0f
		subi	r21, 16
		lsl	r21

		mov	r20, r26
		andi	r20, 0x0f
		lsl	r20
		lsl	r20
		subi	r20, 16
		lsl	r20

		rcall	dojulia
		rcall	dojulia
		rcall	dojulia
		rcall	dojulia

		st	X+, r28

		sbrs	r26, 3
		rjmp	1b

		subi	r26, 256-0x08
		brne	1b


		eor	r1, r1
		ret
