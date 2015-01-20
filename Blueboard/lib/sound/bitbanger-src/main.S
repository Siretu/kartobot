
#include "delays.i"

SREG		= 0x3f
GIMSK		= 0x3b
GIFR		= 0x3a
TIMSK		= 0x39
TIFR		= 0x38
MCUCR		= 0x35
MCUSR		= 0x34
TCCR0		= 0x33
TCNT0		= 0x32
OSCCAL		= 0x31
TCCR1		= 0x30
TCNT1		= 0x2f
OCR1A		= 0x2e
OCR1B		= 0x2d
SFIOR		= 0x2c
WDTCR		= 0x21
EEAR		= 0x1e
EEDR		= 0x1d
EECR		= 0x1c
PORTB		= 0x18
DDRB		= 0x17
PINB		= 0x16
ACSR		= 0x08
ADMUX		= 0x07

;	r0				Working register

;	r1				Number of lines per buffer advance

;	r2				BLACK or VBLANK at all times

;	r3				Channel 2 duty

;	r4, r5				Channel 1 phase
;	r6, r7				Channel 2 phase

;	r8, r9				Channel 1 freq
;	r10, r11			Channel 2 freq

;	r12				Channel 1 volume (with sign)

;	r13				Animation pointer

;	r14				Noise volume (with sign)

;	r15				Frame buffer, column 3

;	r16				Working register
;	r17				Working register - current pixel value

;	r18, r19			Rasterline counter, 0x1df..0

;	r20, r21			Noise lfsr

;	r22, r23			Palette (col0, col0 ^ col1)

;	r24-r25				Frame buffer, columns 1 and 2

;	r26				Global flags

;	r27				Rasterlines left before frame buffer advance

;	r28-r29				Frame counter: 00eppppp lllllttt

;	r30, r31			Z: Work register, ROM pointer, r31 is always 0

; pinout:
;	B0 vsync
;	B1 pwm
;	B2 hsync
;	B3 c0
;	B4 c1

BLACK		= 0x15
HSYNC		= 0x11
VSYNC		= 0x14
HVSYNC		= 0x10
SOLID		= 0x1d
VU1		= 0x05
VU2		= 0x0d
COLOURMASK	= 0x18
COLOURLSB	= 0x08

GF_GLIDE	= 5
GF_SNARESWAP	= 6
GF_BLEEP	= 7

GF_INJ_FLICKER	= 0
GF_INJ_SNARE	= 1
GF_INJ_CUT	= 2
GF_INJ_FAST	= 3
GF_INJ_ROLL	= 4
GF_INJ_MASK	= 0x1f

ARPVOL		= 32
BASSVOL		= 32
HATVOL		= 32
GLIDERATE	= 80

; timing:

; A display line is 51 cycles (31.875 us at 1.6 MHz)
;	t1	BLACK (HSYNC off)
;	t6	start of visible area
;	t42	BLACK (end of visible area)
;	t46	HSYNC

; A display frame is 525 lines (59.76 Hz)
;	0-479	visible line
;	480-489	blank
;	490-491	vsync (along with hsync pulses)
;	492-524	blank

		rjmp	codestart

		; zero page rom here

#define PBASS(n)	(((n) << 1) | PHAT)

PSLIDE		= 0x80
PHAT		= 0x40
PBLEEP		= 0x20

PSNARE		= PBASS(7) | PSLIDE | PHAT

pattern		= 2
		.byte	PBASS(2)
		.byte	0

		.byte	PBASS(2)
		.byte	PBLEEP

		.byte	PSNARE
		.byte	0

		.byte	PHAT | PBLEEP
		.byte	PBASS(1)

		.byte	PHAT
		.byte	PBASS(1)

		.byte	PBASS(1)
		.byte	PBLEEP

		.byte	PSNARE
		.byte	0

		.byte	PBASS(5)
		.byte	PBLEEP

		.byte	PBASS(6)
		.byte	PBLEEP

		.byte	PBASS(6)
		.byte	PBLEEP

		.byte	PSNARE
		.byte	0

		.byte	PBASS(5)
		.byte	PBLEEP

		.byte	PBASS(4)
		.byte	PBLEEP

		.byte	PBASS(4) | PBLEEP
		.byte	PBLEEP

		.byte	PSNARE
		.byte	PBASS(3)

		.byte	PHAT | PBLEEP
		.byte	PBLEEP


NOTE0		= 0x3e6a
NOTE1		= 0x4220
NOTE2		= 0x460f
NOTE3		= 0x4a3a
NOTE4		= 0x4ea3
NOTE5		= 0x5350
NOTE6		= 0x5845
NOTE7		= 0x5d84
NOTE8		= 0x6314
NOTE9		= 0x68f8
NOTE10		= 0x6f36
NOTE11		= 0x75d3

arps		= pattern + 32
		.word	NOTE0 >> 3
		.word	NOTE8 >> 4
		.word	NOTE7 >> 4
		.word	NOTE3 >> 4

		.word	NOTE11 >> 4
		.word	NOTE7 >> 4
		.word	NOTE5 >> 4
		.word	NOTE2 >> 4

		.word	NOTE2 >> 3
		.word	NOTE10 >> 4
		.word	NOTE7 >> 4
		.word	NOTE3 >> 4

		.word	NOTE3 >> 3
		.word	NOTE0 >> 3
		.word	NOTE8 >> 4
		.word	NOTE5 >> 4

bassfreq	= arps + 32
		.word	NOTE7 >> 6
		.word	NOTE8 >> 6
		.word	NOTE9 >> 6
		.word	NOTE10 >> 6
		.word	NOTE11 >> 6
		.word	NOTE0 >> 5
		.word	NOTE0 >> 3

animation	= bassfreq + 14
		.byte	0x00, 0x00, 0x00	;
		.byte	0x00, 0x18, 0x18	; *
		.byte	0x7e, 0x78, 0x00	; L
		.byte	0x7e, 0x16, 0x08	; F
		.byte	0x60, 0x1e, 0x1e	; T
FRAMES		= 5
		.byte	0

codestart:

		ldi	r31, 0

		ldi	r16, 0x79	; from calibration area
		out	OSCCAL, r16

		ldi	r16, BLACK
		mov	r2, r16

		ldi	r16, 2
		mov	r20, r16
		mov	r21, r31

		ldi	r16, 0x1f
		out	DDRB, r16

		ldi	r16, 0x71
		out	TCCR1, r16

		ldi	r16, 1
		out	TCCR0, r16

		ldi	r27, 59

		ldi	r22, BLACK
		ldi	r23, BLACK ^ SOLID

		ldi	r28, 0
		ldi	r29, 0

		mov	r12, r31
		mov	r14, r31

		ldi	r26, 0

		ldi	r16, animation
		mov	r13, r16

		mov	r1, r31

blanking:
		; Line 480
		out	PORTB, r2					; t1
		rcall	soundnosync					; t2 (30)

		; Clear inject flags at start of pattern
		tst	r28						; t32
		brne	1f						; t33
		andi	r26, ~GF_INJ_MASK				; t34
1:
		DE7							; t35
		rcall	sound						; t42

		; Line 481: perform envelope decay

		sbrs	r26, GF_SNARESWAP				; t23
		rjmp	1f						; t24

		ldi	r16, 2						; t25
		sbrc	r14, 7						; t26
		neg	r16						; t27
		sub	r14, r16					; t28

		mov	r16, r14					; t29
		mov	r14, r12					; t30
		mov	r12, r16					; t31

		cpi	r29, 28						; t32
		brlo	3f						; t33
		subi	r22, COLOURLSB					; t34
3:
		DE5							; t35
		rjmp	2f						; t40
1:
		asr	r14						; t26
		DE15							; t27
2:
		rcall	sound						; t42

		; Line 482: get the current pattern event into r18
		mov	r30, r28					; t23
		lsr	r30						; t24
		lsr	r30						; t25
		lsr	r30						; t26
		subi	r30, 256-(pattern)				; t27
		lpm							; t28
		mov	r16, r28					; t31
		andi	r16, 7						; t32
		breq	1f						; t33
		clr	r0						; t34
1:
		mov	r18, r0						; t35
		ori	r29, 0						; t36
		brne	1f						; t37
		andi	r18, PHAT					; t38
1:
		ldi	r16, 203					; t39
		sbrs	r26, GF_INJ_SNARE				; t40
		out	OCR1B, r16					; t41
		rcall	sound						; t42

		; Line 483: perform injected events
		mov	r16, r26					; t23
		mov	r17, r28					; t24
		sbrc	r26, GF_INJ_FAST				; t25
		lsl	r17						; t26

		sbrc	r17, 2						; t27
		andi	r16, ~(1 << GF_INJ_SNARE)			; t28
		sbrs	r17, 2						; t29
		andi	r16, ~(1 << GF_INJ_CUT)				; t30

		sbrc	r16, GF_INJ_SNARE				; t31
		ldi	r18, PSNARE					; t32

		sbrs	r16, GF_INJ_CUT					; t33
		rjmp	1f						; t34

		mov	r12, r31					; t35
		mov	r14, r31					; t36
		mov	r3, r31						; t37
		DE2							; t38
		rjmp	2f						; t40
1:
		DE6							; t36
2:
		rcall	sound						; t42

		; Line 484: parse bass bits
		mov	r30, r18					; t23
		andi	r30, 0x0e					; t24
		brne	1f						; t25

		DE14							; t26
		rjmp	2f						; t40
1:
		subi	r30, 256-(bassfreq-2)				; t27
		lpm							; t28
		mov	r8, r0						; t31
		inc	r30						; t32
		lpm							; t33
		mov	r9, r0						; t36
		ldi	r16, BASSVOL					; t37
		mov	r12, r16					; t38
		andi	r26, ~((1 << GF_GLIDE) | (1 << GF_SNARESWAP))	; t39
		DE2							; t40
2:
		rcall	sound						; t42

		; Line 485: parse glide bit; perform glide
		sbrc	r18, 7						; t23
		ori	r26, (1 << GF_GLIDE) | (1 << GF_SNARESWAP)	; t24
		sbrc	r18, 7						; t25
		clr	r12						; t26

		ldi	r16, 203 - 1					; t27
		cpi	r29, 12						; t28
		brlo	2f						; t29
		mov	r16, r21					; t30
2:
		ori	r16, 0x80					; t31
		sbrc	r18, 7						; t32
		out	OCR1B, r16					; t33

		ldi	r16, 0						; t34
		sbrc	r26, GF_GLIDE					; t35
		ldi	r16, GLIDERATE					; t36
		sub	r8, r16						; t37
		sbc	r9, r31						; t38
		brcc	1f						; t39
		clr	r9						; t40
1:
		DE1							; t41
		rcall	sound						; t42

		; Line 486: parse hat and bleep bits
		ldi	r16, HATVOL					; t23
		sbrc	r18, 6						; t24
		mov	r14, r16					; t25

		sbrc	r20, 6						; t26
		andi	r18, ~PBLEEP					; t27
		sbrs	r29, 4						; t28
		andi	r18, ~PBLEEP					; t29

		sbrc	r18, 5						; t30
		rjmp	1f						; t31

		sbrc	r26, GF_BLEEP					; t32
		clr	r12						; t33
		andi	r26, ~(1 << GF_BLEEP)				; t34
		DE5							; t35
		rjmp	2f						; t40
1:
		ldi	r16, hi8(NOTE0)					; t33
		sbrc	r20, 5						; t34
		lsr	r16						; t35
		sbrc	r20, 4						; t36
		lsr	r16						; t37
		mov	r9, r16						; t38
		ori	r26, 1 << GF_BLEEP				; t39
		DE2							; t40
2:
		rcall	sound						; t42

		; Line 487: modify the frame buffer at notes
		ori	r18, 0						; t23
		breq	1f						; t24

		mov	r16, r20					; t25
		and	r16, r29					; t26
		sbrc	r16, 3						; t27
		eor	r24, r21					; t28
		sbrc	r16, 4						; t29
		eor	r25, r21					; t30
		sbrc	r16, 4						; t31
		eor	r15, r21					; t32
		DE7							; t33
		rjmp	2f						; t40
1:
		DE16							; t26
2:
		rcall	sound						; t42

		; Line 488: load frame buffer from animation
		mov	r30, r13					; t23
		or	r28, r31					; t24
		breq	1f						; t25

		DE14							; t26
		rjmp	2f						; t40
1:
		lpm							; t27
		mov	r24, r0						; t30
		inc	r30						; t31
		lpm							; t32
		mov	r25, r0						; t35
		inc	r30						; t36
		lpm							; t37
		mov	r15, r0						; t40
		inc	r30						; t41
2:
		rcall	sound						; t42

		; Line 489: load frame buffer from animation, continued
		cpi	r30, animation + 3 * FRAMES			; t23
		brne	3f						; t24
		ldi	r30, animation					; t25
3:
		mov	r13, r30					; t26

		ldi	r16, 59 << 1					; t27
		tst	r14						; t28
		breq	6f						; t29
		sub	r16, r29					; t30
6:
		lsr	r16						; t31
		sbrc	r26, GF_INJ_ROLL				; t32
		mov	r16, r21					; t33
		cpi	r29, 20						; t34
		brsh	5f						; t35
		ldi	r16, 59						; t36
5:
		tst	r28						; t37
		brne	4f						; t38
		mov	r1, r16						; t39
4:
		ldi	r16, VSYNC					; t40
		mov	r2, r16						; t41
		ldi	r16, HVSYNC					; t42
		rcall	soundvsync					; t43

		; Line 490: set inject flags - r18 non-null if so
		ldi	r18, GF_INJ_MASK				; t23
		cpi	r29, 28						; t24
		brsh	1f						; t25
		and	r18, r21					; t26
1:
		and	r18, r29					; t27
		mov	r17, r28					; t28
		com	r17						; t29
		sbrc	r20, 7						; t30
		lsr	r17						; t31
		sbrc	r20, 6						; t32
		lsr	r17						; t33
		cpi	r17, 0x0f					; t34
		breq	1f						; t35
		ldi	r18, 0						; t36
1:
		cpi	r29, 5						; t37
		brsh	1f						; t38
		ldi	r18, 0						; t39
1:
		or	r26, r18					; t40
		DE1							; t41
		ldi	r16, HVSYNC					; t42
		rcall	soundvsync					; t43

		; Line 491: do further one-off glitches
		tst	r18						; t23
		breq	1f						; t24

		ldi	r16, COLOURMASK					; t25
		and	r16, r20					; t26
		eor	r22, r16					; t27

		mov	r16, r21					; t28
		andi	r16, 7						; t29
		ori	r16, 1						; t30
		cpi	r29, 22						; t31
		brlo	3f						; t32
		out	TCCR0, r16					; t33
3:
		rjmp	2f						; t36
1:
		DE10							; t26
2:
		sbrc	r29, 5						; t36
		rjmp	lftfield_entry					; t37

		DE2							; t38
		ldi	r16, BLACK					; t40
		mov	r2, r16						; t41
		rcall	sound						; t42

		; Line 492: prevent flicker injection too early
		cpi	r29, 18						; t23
		brsh	1f						; t24
		andi	r26, ~(1 << GF_INJ_FLICKER)			; t25
1:
		DE16							; t26
		rcall	sound						; t42

		; Line 493: channel 2 frequency
		DE1							; t23
		mov	r16, r28					; t24
		andi	r16, 0x06					; t25
		ldi	r30, lo8(arps)					; t26
		add	r30, r16					; t27
		sbrc	r28, 6						; t28
		subi	r30, 256-8					; t29
		sbrc	r28, 7						; t30
		subi	r30, 256-16					; t31
		lpm							; t32
		mov	r10, r0						; t35
		inc	r30						; t36
		lpm							; t37
		mov	r11, r0						; t40
		DE1							; t41
		rcall	sound						; t42

		; Line 494: channel 2 pwm
		mov	r16, r28					; t23
		and	r16, r21					; t24
		sbrc	r16, 0						; t25
		inc	r3						; t26
		sbrs	r29, 2						; t27
		eor	r3, r3						; t28
		cpi	r29, 26						; t29
		brlo	1f						; t30
		mov	r3, r21						; t31
1:
		DE10							; t32
		rcall	sound						; t42

		; Line 495
		DE18							; t23

		ldi	r18, 27						; t41
1:
		rcall	sound						; t42
		; Line 496-522
		DE16							; t23
		subi	r18, 1						; t39
		brne	1b						; t40
		nop							; t41

		rcall	sound						; t42
		; Line 523
		DE19							; t23

		rcall	sound						; t42
		; Line 524: prepare for field
		DE22							; t23
		subi	r28, 0xff					; t45
		sbci	r29, 0xff					; t46
		ldi	r16, HSYNC					; t47
		out	PORTB, r16					; t48

field:
		ldi	r18, 0xdf					; t49
		ldi	r19, 0x01					; t50

		; Lines 0-479
displine:
						add	r6, r10		; t0
		out	PORTB, r2					; t1
						adc	r7, r11		; t2
		mov	r17, r22					; t3
		sbrc	r24, 7						; t4
		eor	r17, r23					; t5
		out	PORTB, r17					; t6
						add	r4, r8		; t7
						adc	r5, r9		; t8
						brcc	1f		; t9
						neg	r12		; t10
1:
						lsl	r20		; t11
						rol	r21		; t12
						brvc	1f		; t13
						subi	r20, 0xfe	; t14
1:
						in	r16, TCNT0	; t15
		sbrc	r25, 7						; t16
		eor	r17, r23					; t17
		out	PORTB, r17					; t18
						and	r16, r21	; t19
						sbrc	r16, 0		; t20
						neg	r14		; t21

						ldi	r16, ARPVOL	; t22
						cp	r7, r3		; t23
						brcs	1f		; t24
						neg	r16		; t25
1:
						add	r16, r12	; t26
						add	r16, r14	; t27
		sbrc	r15, 7						; t28
		eor	r17, r23					; t29
		out	PORTB, r17					; t30
						subi	r16, 256-102	; t31
						out	OCR1A, r16	; t32
		subi	r27, 1						; t33
		brcc	noadvance					; t34
		lsl	r24						; t35
		adc	r24, r31					; t36
		lsl	r25						; t37
		adc	r25, r31					; t38
		lsl	r15						; t39
		adc	r15, r31					; t40
		mov	r27, r1						; t41
displine_back:
		out	PORTB, r2					; t42
		sbrc	r26, GF_INJ_FLICKER				; t43
		subi	r23, COLOURLSB					; t44
		ldi	r16, HSYNC					; t45
		out	PORTB, r16					; t46
		subi	r18, 1						; t47
		sbc	r19, r31					; t48
		brcc	displine					; t49

		rjmp	blanking					; t50

noadvance:
		DE4							; t36
		rjmp	displine_back					; t40

sound:
		ldi	r16, HSYNC					; t45
soundvsync:
		out	PORTB, r16					; t46
soundnosync:
						add	r4, r8		; t47
						adc	r5, r9		; t48
						brcc	1f		; t49
						neg	r12		; t50
1:
						add	r6, r10		; t0
		out	PORTB, r2					; t1
						adc	r7, r11		; t2

						lsl	r20		; t3
						rol	r21		; t4
						brvc	1f		; t5
						subi	r20, 0xfe	; t6
1:
						in	r16, TCNT0	; t7
						and	r16, r21	; t8
						sbrc	r16, 0		; t9
						neg	r14		; t10

						ldi	r16, ARPVOL	; t11
						cp	r7, r3		; t12
						brcs	1f		; t13
						neg	r16		; t14
1:
						add	r16, r12	; t15
						add	r16, r14	; t16

						subi	r16, 256-102	; t17
						out	OCR1A, r16	; t18

		ret							; t19

delay3xplus8:
		nop
delay3xplus7:
		nop
delay3xplus6:
1:
		subi	r16, 1		; 1
		brne	1b		; 1/2
		ret			; 4

delay9:		rjmp .
delay7:		ret

lftfield:
		ldi	r23, 0xc8					; t41
		ldi	r18, 16		; 224..239			; t42
		rcall	lftlines					; t43

		ldi	r23, 0xa8					; t39
		ldi	r18, 16		; 240..255			; t40
		rcall	de2lftlines					; t41

		ldi	r23, 0x48					; t39
		ldi	r18, 16		; 256..271			; t40
		rcall	de2lftlines					; t41

		ldi	r22, BLACK					; t39
		ldi	r18, 218	; 272..489			; t40
		rcall	de2lftlines					; t41

		ldi	r20, HVSYNC					; t39
		ldi	r21, VSYNC					; t40
		ldi	r22, VSYNC					; t41
		ldi	r18, 2		; 490..491			; t42
		rcall	lftlines					; t43
lftfield_entry:
		ldi	r20, HSYNC					; t39
		ldi	r21, BLACK					; t40
		ldi	r22, BLACK					; t41
		ldi	r18, 241	; 492..524, 0..207		; t42
		rcall	lftlines					; t43

		ldi	r22, SOLID					; t39
		ldi	r23, 0x90					; t40
		ldi	r18, 16		; 208..223			; t41
		DE1							; t42
		rcall	lftlines					; t43

		rjmp	lftfield					; t39

lftlines_loop:
		DE8							; t36
de2lftlines:
		DE2							; t44
lftlines:
		out	PORTB, r20					; t46
		DE5							; t47

		out	PORTB, r21					; t1
		DE8							; t3
		out	PORTB, r22					; t10
		sbrc	r23, 7						; t11
		out	PORTB, r21					; t12
		DE3							; t13
		out	PORTB, r21					; t16
		DE1							; t17
		out	PORTB, r22					; t18
		sbrc	r23, 6						; t19
		out	PORTB, r21					; t20
		sbrc	r23, 5						; t21
		out	PORTB, r21					; t22
		DE1							; t23
		out	PORTB, r21					; t24
		sbrc	r23, 4						; t25
		out	PORTB, r22					; t26
		DE1							; t27
		out	PORTB, r22					; t28
		sbrc	r23, 3						; t29
		out	PORTB, r21					; t30
		DE1							; t31
		out	PORTB, r21					; t32

		subi	r18, 1						; t33
		brne	lftlines_loop					; t34
		ret							; t35

