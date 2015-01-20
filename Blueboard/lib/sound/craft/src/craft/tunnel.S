#include "stuff.i"

		.global	tunnel
		.global	tunnelcut

divs16u8:
		sbrs	r1, 7
		rjmp	divu16u8

		com	r1
		neg	r0
		brlo	1f
		inc	r1
1:		rcall	divu16u8
		;com	r25
		neg	r24
		;sbci	r25, 0xff
		ret

dotunnel:
		; Calculates pixel value for tunnel at x = r20, y = r21 (and mirror pixel at x = r20, y = -r21).
		; Coordinates are 3.5 fixed point with x in [-2, 2] and y in [-1, 1].
		; r7 is the animation parameter.
		; r30 is sin(a) and r31 is cos(a), where a is the angle of rotation of the scene along the y axis.
		; The resulting pixel value is shifted into the low two bits of r28 (and r29, for the mirror pixel)
		; and r20 is incremented to the next x position.

		; May clobber: r0, r1, r14, r15, r16, r17, r18, r19, r22, r23, r24, r25, r27.

		movw	r14, r20

		lsl	r28
		lsl	r28
		lsl	r29
		lsl	r29

		ldi	r17, 32		; zv
		mov	r16, r20

		; xout = x cos - z sin
		; zout = x sin + z cos

		movw	r18, r30
		fmuls	r16, r19
		mov	r20, r1
		fmuls	r17, r18
		sub	r20, r1
		fmuls	r16, r18
		mov	r22, r1
		fmuls	r17, r19
		add	r22, r1

		muls	r20, r20	; xv * xv
		movw	r16, r0
		muls	r21, r21	; yv * yv
		add	r16, r0
		adc	r17, r1

		brne	4f
		cpi	r16, 81
		brlo	9f
4:
		;rcall	invsqrt
		; inline...
invsqrt:
		; Given the unsigned integer x = r17:r16, find a value r = r19 so that r * r * x = 256 * 16 (approximately).
		; This corresponds to a scaled inverse square root function.

		; Input: r17:r16
		; Output: r19
		; Assumed global constant: r3 = 2
		; Clobbers: r16, r17, r18

		lsl	r16
		rol	r17
		lsl	r16
		rol	r17
		lsl	r16
		rol	r17

		ldi	r18, 0x80
		ldi	r19, 0
1:
		add	r19, r18
		mul	r19, r19
		mul	r17, r1
		cp	r1, r3
		brlo	2f
		sub	r19, r18
2:		lsr	r18
		brne	1b

		;ret
		; end inline

		fmulsu	r22, r19
		mov	r22, r1

		sbrc	r20, 7
		rjmp	2f

		; xv >= 0

		mov	r16, r20
		sub	r16, r21
		ldi	r17, 16
		muls	r16, r17
		mov	r19, r20
		add	r19, r21
		inc	r19
		rcall	divs16u8
		ldi	r25, 16
		sub	r25, r24

		rjmp	3f
2:
		; xv < 0

		mov	r16, r21
		add	r16, r20
		ldi	r17, 16
		muls	r16, r17
		mov	r19, r21
		sub	r19, r20
		rcall	divs16u8
		ldi	r25, 48
		sub	r25, r24
3:
		sub	r25, r7

		add	r22, r7
		;add	r22, r7

		ldi	r16, 2
		sbrc	r22, 4
		eor	r16, r2
		sbrc	r25, 4
		eor	r16, r2

		or	r28, r16

		add	r25, r7
		neg	r25
		sub	r25, r7

		ldi	r16, 2
		sbrc	r22, 4
		eor	r16, r2
		sbrc	r25, 4
		eor	r16, r2

		or	r29, r16

9:
		movw	r20, r14
		subi	r20, 256-2
		ret

tunnel:
		in	r16, GPIOR0
		ldi	r17, 4
		eor	r16, r17
		out	GPIOR0, r16

		lds	r31, framecount+1
		lds	r30, framecount
		lsr	r31
		ror	r30
		lsr	r31
		ror	r30
		lsr	r31
		ror	r30
		lds	r31, tunneloffset
		add	r30, r31
		ldi	r31, hi8(sine)
		lpm	r16, Z
		subi	r30, 256-64
		lpm	r17, Z
		movw	r30, r16

		lds	r7, framecount
		lds	r15, framecount+1
		lsr	r15
		ror	r7
		;lsr	r15
		;ror	r7

		ldi	r26, 0
		sbic	GPIOR0, GFLAG_DBLBUF
		ldi	r26, 0x08

		ldi	r21, 30
		ldi	r20, 256-31

1:
		rcall	dotunnel
		rcall	dotunnel
		rcall	dotunnel
		rcall	dotunnel

		ldi	r16, 0xf0
		ldi	r27, 0x04
		eor	r26, r16
		st	X, r29
		ldi	r27, 0x03
		eor	r26, r16
		st	X+, r28

		cpi	r20, 33
		brne	1b

		subi	r26, 256-8
		ldi	r20, 256-31
		subi	r21, 2
		brsh	1b

		eor	r1, r1
		ret

tunnelcut:
		lds	r16, tunneloffset
		subi	r16, 256-30
		sts	tunneloffset, r16
		rjmp	tunnel
