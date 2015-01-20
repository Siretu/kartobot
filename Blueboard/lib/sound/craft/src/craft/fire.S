
		.global	initfire
		.global	fire


initfire:	ldi	r26, 1
		ldi	r27, 0

		ldi	r29, 0x03
		ldi	r28, 0
1:
		add	r26, r26
		adc	r27, r27
		sbrc	r27, 7
		eor	r26, r2
		sbrc	r27, 6
		eor	r26, r2
		st	Y+, r27

		cpi	r29, 0x05
		brne	1b

		ret

fire:
		rcall	waitvbl

		; apply fire

		ldi	r28, 0x00
		ldi	r29, 0x03

		ldd	r20, Y+15
		ldd	r21, Y+16

2:
		ldd	r22, Y+17
		rcall	firesub
		rcall	firesub
		rcall	firesub
		rcall	firesub
		st	Y+, r24

		cpi	r28, 0xef
		brne	2b
		cpi	r29, 0x04
		brne	2b

		;ldi	r29, 0x04
		;ldi	r28, 0xf0
		;ldi	r18, 15

1:
		;add	r26, r26
		;adc	r27, r27
		;sbrc	r27, 7
		;eor	r26, r2
		;sbrc	r27, 6
		;eor	r26, r2
		;st	Y+, r27
		;dec	r18
		;brne	1b

		; draw spirougraph

		ldi	r31, hi8(sine)
		ldi	r23, 16
		lds	r30, framecount
		lds	r29, timer3
6:
		push	r30
		push	r29

		ldi	r16, 25
		ldi	r17, 0

		rcall	rotate
		movw	r20, r16
		lsl	r20

		mov	r30, r29
		com	r30
		ldi	r16, 0
		ldi	r17, 30
		rcall	rotate
		add	r20, r16
		add	r21, r17

		eor	r1, r1

		subi	r20, 128
		lsl	r21

		ldi	r29, 0x04
		lsl	r21
		sbc	r29, r1
		andi	r21, 0xf0
		mov	r28, r21

		mov	r16, r20
		swap	r16
		andi	r16, 0x0f
		or	r28, r16
		ldi	r17, 0xc0
		andi	r20, 0x0c
4:		subi	r20, 0x04
		brcs	5f
		lsr	r17
		lsr	r17
		rjmp	4b
5:
		ld	r16, Y
		or	r16, r17
		st	Y, r16

		pop	r29
		pop	r30
		subi	r30, 1
		subi	r29, 3
		subi	r23, 1
		brsh	6b

		ret

firesub:
		ldi	r17, 0
		sbrc	r20, 1
		subi	r17, 256-2
		sbrc	r20, 0
		subi	r17, 256-1
		sbrc	r21, 7
		subi	r17, 256-8
		sbrc	r21, 6
		subi	r17, 256-4
		sbrc	r21, 5
		subi	r17, 256-2
		sbrc	r21, 4
		subi	r17, 256-1

		add	r26, r26
		adc	r27, r27
		sbrc	r27, 7
		eor	r26, r2
		sbrc	r27, 6
		eor	r26, r2

		mov	r18, r27
		andi	r18, 6
		add	r17, r18

		lsl	r17
		lsl	r17
		lsl	r17

		lsl	r17
		rol	r24
		lsl	r17
		rol	r24

		rol	r22
		rol	r21
		rol	r20
		rol	r22
		rol	r21
		rol	r20

		ret
