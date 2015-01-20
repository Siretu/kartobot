#include "delays.i"
#include "stuff.i"

		.global	__vector_11
		.global asminit
		.global __stack
		.global	delay15
		.global	delay14
		.global	delay12
		.global	delay11
		.global	delay10
		.global	delay9
		.global	delay8
		.global	delay7
		.global	delay3xplus8
		.global	delay3xplus7
		.global	delay3xplus6
		.global	eefromz

		.global	lh_endless
		.global	lh_black
		.global	lh_white
		.global	lh_f15
		.global	lh_f20
		.global	lh_fill
		.global	lh_text4
		.global	lh_vu
		.global	lh_check
		.global	lh_2bit
		.global	lh_2bitref
		.global	lh_column
		.global	lh_dots
		.global	lh_dotsvu
		.global	lh_stars2d
		.global	lh_flower
		.global	lh_julia1
		.global	lh_julia2
		.global	lh_dblbuf
		.global	lh_midpurple
		.global	lh_midgrey
		.global	lh_midgrey2
		.global	lh_bars
		.global	lh_2bit48

__stack		=	0x2ff

SREG		=	0x3f
PORTB		=	0x05
PORTC		=	0x08
PORTD		=	0x0b
TIFR1		=	0x16
TCNT1L		=	0x84
UDR0		=	0xc6
UBRR0H		=	0xc5
UBRR0L		=	0xc4
SPDR		=	0x2e
TCNT2		=	0xb2
OCR2A		=	0xb3
OCR2B		=	0xb4
GTCCR		=	0x23
SPSR		=	0x2d
SPCR		=	0x2c
DDRB		=	0x04

eefromz:
		out	EEARH, r31
		out	EEARL, r30
		out	EECR, r2
		ret

delay15:	nop
delay14:	rjmp	.
delay12:	rjmp	.
delay10:	nop
delay9:		nop
delay8:		nop
delay7:		ret

delay11:	rjmp	delay9

delay3xplus8:	nop
delay3xplus7:	nop
delay3xplus6:
1:		subi	r16, 1			;				1
		brsh	1b			;				2 in loop, 1 otherwise
		ret				;				4


;		3 + 54 = 57 cycles
;		Expects global constants:
;						r1 = 0
;						r2 = 1
;		Expects global variables:
;						r8:r9 = osc[0].phase
;						r10:r11 = osc[1].phase
;						r12:r13 = osc[2].phase
;						r4:r5 = osc[3].phase
;		Trashes r16, r17

soundroutine:

		lds	r16, ch+0*c_size+c_freq		;			2
		lds	r17, ch+0*c_size+c_freq+1	;			2
		add	r8, r16			;				1
		adc	r9, r17			;				1

		lds	r16, ch+1*c_size+c_freq		;			2
		lds	r17, ch+1*c_size+c_freq+1	;			2
		add	r10, r16		;				1
		adc	r11, r17		;				1

		lds	r16, ch+2*c_size+c_freq		;			2
		lds	r17, ch+2*c_size+c_freq+1	;			2
		add	r12, r16		;				1
		adc	r13, r17		;				1

                add     r4, r4			;				1
                adc     r5, r5			;				1
                sbrc    r5, 7			;				1/2 \_ 2
                eor     r4, r2			;				1   /
                sbrc    r5, 6			;				1/2 \_ 2
                eor     r4, r2			;				1   /

		mov	r17, r9			;				1
		lsl	r17			;				1
		and	r17, r9			;				1
		lds	r16, ch+0*c_size+c_vol	;				2
		sbrc	r17, 7			;				1/2 \_ 2
		neg	r16			;				1   /

		lds	r17, ch+1*c_size+c_vol	;				2
		sbrc	r11, 7			;				1/2 \_ 2
		neg	r17			;				1   /
		add	r16, r17		;				1

		mov	r17, r13		;				1
		sbrc	r17, 7			; skip if rising half		1/2 \_ 2
		com	r17			; map [-128, -1] to [127, 0]	1   /
		lsr	r17			; range is now [0, 63]		1
		add	r16, r17		;				1

		lds	r17, noisevolume	;				2
		sbrc	r5, 7			;				1/2 \_ 2
		neg	r17			;				1   /
		add	r16, r17		;				1

		subi	r16, 32+128		; 32 for triangle, 128 for d/a	1
		bst	r16, 3			; this pin is needed for oc2b	1
		bld	r16, 1			; so we move the bit		1
		out	PORTD, r16		;				1
		ret				;				4

__vector_11:	; Timer interrupt OC1A

		; The timer will try to generate an interrupt at the beginning of every
		; rasterline. However, during the entire visible part of each video frame,
		; the interrupt routine doesn't return.

		; So if we're here, we must be at a rasterline in the range [481,524]
		; which can be expressed as 481 + [0,43]. This offset is in vbltime.

		; Interrupt latency is:
		;	[0,3] cycles for finishing the currently executing instruction.
		;	4 cycles for pushing pc on stack and jumping to the interrupt vector table.
		;	2 cycles for the rjmp in the interrupt vector table.
		; Then we add a +/- 2 cycle margin for good measure.

		; At t4 + latency where latency is in [0,7].

		push	r0			;				2
		in	r0, SREG		; read status register		1
		push	r0			;				2

		push	r1			;				2
		push	r16			;				2
		push	r17			;				2
		eor	r1, r1			;				1

		; At t16 + latency.

		rcall	soundroutine		;				57

		; At t73 + latency.

		lds	r16, vbltime		;				2

		cpi	r16, 524-481		; video frame at line 524	1
		breq	syncframe		;				1/2

		cpi	r16, 490-481		; if equal, we should turn on /vsync
		brne	.no_v_on
		cbi	PORTB, 1		; set /vsync low

.no_v_on:	cpi	r16, 492-481		; if equal, we should turn off /vsync
		brne	.no_v_off
		sbi	PORTB, 1		; set /vsync high

.no_v_off:	inc	r16
		sts	vbltime, r16		; save new vbltime value

retafterframe:	pop	r17
		pop	r16
		pop	r1

		pop	r0			;				2
		out	SREG, r0		; restore status register	1
		pop	r0			;				2
		reti				;				4

syncframe:
		; At t78 + latency.

		; Now, we must get rid of the interrupt latency by synching up to timer 1.

		lds	r17, TCNT1L		;				2

		; Now r17 is in the range [78,85], indicating the current clock time.
		; We want to end up at a well-known time, so if r17 is low, we have to delay execution.

		ldi	r16, 120		;				1
		sub	r16, r17		; r16 = 120 - latency - 78	1

		rcall	delayxplus16		;				58 - latency

		; At t140

		push	r31			;				2
		push	r30			;				2

		DE270

		; At t414

		DE6
		out	GTCCR, r3		;				1
		ldi	r16, 105		; start counting at 105		1
		sts	TCNT2, r16		;				2

		DE148

		push	r29			;				2
		push	r28			;				2
		push	r27			;				2
		push	r26			;				2
		push	r25			;				2
		push	r24			;				2
		push	r23			;				2
		push	r22			;				2
		push	r21			;				2
		push	r20			;				2
		push	r19			;				2
		push	r18			;				2
		push	r15			;				2
		push	r14			;				2
		push	r7			;				2

		; At t602

		; Prepare a new video frame.

		lds	r16, framecount		;				2
		lds	r17, framecount+1	;				2
		add	r16, r2			;				1
		adc	r17, r1			;				1
		sts	framecount, r16		;				2
		sts	framecount+1, r17	;				2

		ldi	r16, 109		; enable here			1
		sts	OCR2B, r16		;				2
		ldi	r16, 161		; disable here			1
		sts	OCR2A, r16		;				2

		eor	r6, r6			; restart copperlist		1
		ldi	r16, lo8(copperlist)	;				2
		sts	copperptr, r16		;				2
		ldi	r16, hi8(copperlist)	;				2
		sts	copperptr+1, r16	;				2

		sbic	GPIOR0, GFLAG_BLANK	;				1/2 \_ 2
		inc	r6			;				1   /

		sts	linehandler, r1		;				2

		eor	r20, r20		; current y position		1
		eor	r21, r21		;				1

		; At t631

		lds	r26, copperptr		; X = copperptr			2
		lds	r27, copperptr+1	;				2

		; At t635, i.e. t0 of first line.

vline:
		; Now we are at t0 of a video line

		rcall	soundroutine		;				57

		; At t57

		and	r6, r6			;				1
		brne	nocopperaction		; 				1/2

		ld	r17, X+			; get new linehandler		2
		sts	linehandler, r17	;				2
		DE4
		ld	r6, X+			; get new timer			2
		sts	copperptr, r26		; store updated copperlist ptr	2
		sts	copperptr+1, r27	;				2
		mov	r7, r1			; clear raster line counter	1

dosamecopper:
		ldi	r30, lo8(pm(jumptable))	;				1
		ldi	r31, hi8(pm(jumptable))	;				1
		add	r30, r17		;				1
		adc	r31, r1			;				1

		out	GTCCR, r3		;				1
		ldi	r16, 100		; start counting at 100		1
		sts	TCNT2, r16		;				2

		ijmp				;				2

		; (followed by 2 cycles of rjmp in the table)
		; At t86 the line handler starts executing.

nocopperaction:
		; At t60

		dec	r6			;				1

		DE8

		lds	r17, linehandler	;				2
		inc	r7			;				1
		rjmp	dosamecopper		;				2

;----------------------------------------------------------------------------------------------

; The line handler interface looks like this:

;	+-------+----------- - - - ------------+--------+
;	t86     t128                           t608     t622
;	Setup   Visible line                   Clean up Leave

;	The line handler starts executing at t86. The display will be off.
;	42 cycles are available for preparations.
;	At t128, the first pixel should be emitted.
;	One pixel per cycle gives 480 pixels.
;	At t608, turn off the display with e.g. "out PORTC, r1".
;	At t622, jump to endofline (2 cycles).

; Registers:
;	r0	?
;	r1	Always 0
;	r2	Always 1
;	r3	Always 2
;	r4	Used by soundroutine. Don't touch.
;	r5	Used by soundroutine. Don't touch.
;	r6	Internal copper timer. Don't touch.
;	r7	Raster line within current copperlist segment. Don't modify.
;	r8	Used by soundroutine. Don't touch.
;	r9	Used by soundroutine. Don't touch.
;	r10	Used by soundroutine. Don't touch.
;	r11	Used by soundroutine. Don't touch.
;	r12	Used by soundroutine. Don't touch.
;	r13	Used by soundroutine. Don't touch.
;	r14	?
;	r15	?
;	r16	Free to use. Not preserved between lines. Trashed by delay macros.
;	r17	Free to use. Not preserved between lines.
;	r18	Free to use. Preserved between lines.
;	r19	Free to use. Preserved between lines.
;	r20	LSB of raster line, [0,479]. Don't modify.
;	r21	MSB of raster line, [0,479]. Don't modify.
;	r22	Free to use. Preserved between lines.
;	r23	Free to use. Preserved between lines.
;	r24	Free to use. Preserved between lines.
;	r25	Free to use. Preserved between lines.
;	r26 X	Free to use. Not preserved between lines.
;	r27 X	Free to use. Not preserved between lines.
;	r28 Y	Free to use. Preserved between lines.
;	r29 Y	Free to use. Preserved between lines.
;	r30 Z	Free to use. Not preserved between lines.
;	r31 Z	Free to use. Not preserved between lines.

; In other words:
;	SCRATCH:	r16, r17, X, Z
;	PRESERVED:	r18, r19, r22, r23, r24, r25, Y

;----------------------------------------------------------------------------------------------

lh_endless:	; At t86

		mov	r6, r2			;				1
		DE535

		; At t622
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_black:	; At t86

		DE536

		; At t622
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_white:	; At t86

		ldi	r18, 0x3f		;				1
		DE41

		; At t128, pixel 0

		out	PORTC, r18		;				1

		DE477

		; At t606
		rjmp	r1d15endofline		;				2

;----------------------------------------------------------------------------------------------

lh_fill:	; At t86

		DE41

		ldi	r16, 0x11		;				1

		; At t128, pixel 0

		out	PORTC, r16		;				1

		DE477

		; At t606

		rjmp	r1d15endofline		;				2

;----------------------------------------------------------------------------------------------

lh_text4:	; At t86

		sbic	GPIOR0, GFLAG_FLOWER3	;				1/2 \_ 2 if no flower
		rjmp	intoflower		;				2   /

		lds	r17, bg			;				2

		ldi	r26, lo8(text)		;				1
		ldi	r27, hi8(text)		;				1
		ldi	r28, lo8(texty)		;				1
		ldi	r29, hi8(texty)		;				1
		ldi	r31, hi8(font)		;				1

		mov	r18, r7			;				1
		lsr	r18			;				1
		andi	r18, 0xfe		;				1
		subi	r18, 8			; [-8,6]			1
		mov	r14, r18		;				1
		ldi	r22, 25			; x counter			1

		lds	r23, scrollx		; delay1			2
		ldi	r24, 5			;				1
		sub	r24, r23		; delay2			1

		; At t105

		DE23

		; At t128

		out	PORTC, r17		;				1

		DE4

1:		subi	r23, 1			;				\_ 3 * r23 + 2
		brsh	1b			;				/

		; At t135 + 3 * delay1 + subdelay1

t4loop:		ld	r16, X+			;				2
		mov	r18, r14		;				1
		ld	r19, Y+			;				2
		add	r18, r19		;				1
		sbrc	r18, 4			;				1/2 \_ 2 if no jump
		rjmp	9f			;				2   /
		swap	r18			;				1
		mov	r30, r18		;				1
		add	r30, r16		;				1
		lpm	r0, Z			;				3
8:		out	SPDR, r0		;				1
		subi	r22, 1			;				1
		brne	t4loop			;				1/2

		; At t584 + 3 * delay1 + subdelay1

2:		subi	r24, 1			;				\_ 3 * r24 + 2
		brsh	2b			;				/

		; At t584 + 3 * (delay1 + delay2) + (subdelay1 + subdelay2) = t607

		DE7

		; At t608

		out	PORTC, r1		;				1

		; At t609
		rjmp	d13endofline		;				2

9:		eor	r0, r0			;				1
		dec	r0			;				1
		DE1
		rjmp	8b			;				2

;----------------------------------------------------------------------------------------------

lh_vu:		; At t86

		lds	r26, xoffset		;				2
		subi	r26, 256-49		;				1
		ldi	r27, 97			;				1
		sub	r27, r26		;				1
		DE39

		; At t130

1:		subi	r26, 1			;				\_ 2 + 3x
		brsh	1b			;				/

		rcall	vu			;				183

1:		subi	r27, 1			;				\_ 2 + 3x
		brsh	1b			;				/

		; At t608, pixel 480, end of visible area

		out	PORTC, r1		;				1

		; At t609
		rjmp	d13endofline		;				2

;----------------------------------------------------------------------------------------------

lh_check:	; At t86

		ldi	r22, 0x17		;				1

		sbis	GPIOR0, GFLAG_COLOUR	;				1/2 \_ 2
		ldi	r22, 0x2a		;				1   /

		mov	r23, r22		;				1

		ldi	r31, hi8(freq)		;				1
		ldi	r30, lo8(freq)		;				1
		mov	r18, r7			;				1
		lsr	r18
		com	r18
		andi	r18, 0xfe		;				1
		subi	r18, 256-167		;				1
		add	r30, r18
		adc	r31, r1
		lpm	r19, Z			;				3

		lds	r16, framecount
		lsl	r16
		sub	r16, r7
		sbrc	r16, 5
		eor	r22, r23

		DE2

		; At t110

		; r19 is colour field frequency. colour field width will be 256 / r19.

		; r17 is field phase. colour changes when r17 wraps.

		ldi	r17, 0
		ldi	r16, 34
		mul	r16, r19
		lds	r16, framecount		;				2
		sub	r0, r16
		sbc	r1, r17
		sub	r0, r16
		sbc	r1, r17
		mov	r17, r0
		sbrs	r1, 0
		eor	r22, r23

		eor	r1, r1

		ldi	r18, 69			;				1

		; At t125

1:
		sub	r17, r19		;				1
		brsh	2f			;				1/2 \_ 2
		eor	r22, r23		;				1   /
2:		out	PORTC, r22		;				1
		dec	r18			;				1
		brne	1b			;				1/2

		; At t607

		DE1

		; At t608, pixel 480, end of visible area

		out	PORTC, r1		;				1

		; At t609
		rjmp	d13endofline		;				2

;----------------------------------------------------------------------------------------------

lh_2bit:	; At t86

		DE17

		mov	r26, r7			;				1
		lsl	r26			;				1
		andi	r26, 0xf0		;				1
		ldi	r27, 0x03		;				1
		sbrc	r7, 7			;				1/2 \_ 2
		ldi	r27, 0x04		;				1   /
		ldi	r28, 15			;				1

		; At t110
		rcall	pixelgfx		;				16 + 32*15 = 496

		; At t606
		rjmp	r1d15endofline

;----------------------------------------------------------------------------------------------

lh_2bitref:	; At t86

		DE16

		mov	r26, r7			;				1
		com	r26			;				1
		ldi	r27, 0x03		;				1
		sbrc	r26, 4			;				1/2 \_ 2
		ldi	r27, 0x04		;				1   /
		swap	r26			;				1
		andi	r26, 0xf0		;				1
		ldi	r28, 15			;				1

		; At t110
		rcall	pixelgfx		;				16 + 32*15 = 496

		; At t606
		rjmp	r1d15endofline

;----------------------------------------------------------------------------------------------

lh_column:	; At t86

		ldi	r22, 0x3c		;				1
		ldi	r23, 0x28		;				1
		mov	r24, r23		;				1
		mov	r25, r22		;				1

		ldi	r31, hi8(sine)		;				1

		lds	r16, twist		;				2
		mov	r18, r21		;				1
		ror	r18			;				1
		mov	r18, r20		;				1
		ror	r18			;				1
		mulsu	r16, r18		;				2
		mov	r18, r1			;				1
		eor	r1, r1			;				1
		lds	r19, framecount		;				2
		add	r18, r19		;				1
		add	r18, r19		;				1
		sbrc	r18, 6			;				1/2 \_ 2
		movw	r22, r24		;				1   /
		andi	r18, 63			;				1

		ldi	r30, 32+64		;				1
		sub	r30, r18		;				1
		lpm	r16, Z			;				3
		asr	r16			;				1
		asr	r16			;				1
		ldi	r17, 75			;				1
		sub	r17, r16		;				1

		subi	r30, 256-64		;				1
		lpm	r16, Z			;				3
		asr	r16			;				1
		asr	r16			;				1
		ldi	r18, 75			;				1
		sub	r18, r16		;				1

		subi	r30, 256-64		;				1
		lpm	r16, Z			;				3
		asr	r16			;				1
		asr	r16			;				1
		ldi	r19, 75			;				1
		sub	r19, r16		;				1

		; At t133

		;lds	r30, framecount
		;lpm	r25, Z
		;asr	r25
		;asr	r25

		lds	r25, xoffset		;				2
		DE5

		;DE2
		
		; Coordinates must satisfy 0 <= r17 <= r18 <= r19 <= 150

		ldi	r24, 150
		sub	r24, r19
		sub	r19, r18
		sub	r18, r17

		add	r17, r25
		sub	r24, r25

		; At t146

1:		subi	r17, 1			;				1   \_ 3 * r17 + 2
		brsh	1b			;				1/2 /
		out	PORTC, r22		;				1
2:		subi	r18, 1			;				1   \_ 3 * r18 + 2
		brsh	2b			;				1/2 /
		out	PORTC, r23		;				1
3:		subi	r19, 1			;				1   \_ 3 * r19 + 2
		brsh	3b			;				1/2 /
		out	PORTC, r1		;				1
4:		subi	r24, 1			;				1   \_ 3 * r24 + 2
		brsh	4b			;				1/2 /

		DE1

		; At t158 + 3 * (r17 + r18 + r19 + r24) = t608

		; At t608, pixel 480, end of visible area

		out	PORTC, r1		;				1

		; At t622
		rjmp	d13endofline		;				2

;----------------------------------------------------------------------------------------------

lh_dots:	; At t86

		movw	r26, r20		;				1
		lsr	r27			;				1
		ror	r26			;				1
		ldi	r27, 0x03		;				1
		ld	r17, X			;				2

		cpi	r17, 157		;				1
		brsh	9f			;				1/2

		ldi	r27, 0x04		;				1
		ld	r22, X			;				2

		rol	r22
		rol	r22
		rol	r22
		andi	r22, 3
		ldi	r30, lo8(palette)
		ldi	r31, hi8(palette)
		add	r30, r22
		adc	r31, r1
		ld	r22, Z

		ldi	r18, 157		;				1
		sub	r18, r17		;				1

		DE19

		; At t128

		out	PORTC, r1		;				1

1:		subi	r17, 1			;				1   \_ 3x + 2
		brsh	1b			;				1/2 /

		DE1
		out	PORTC, r22		;				1
		DE1
		out	PORTC, r1		;				1

2:		subi	r18, 1			;				1   \_ 3x + 2
		brsh	2b			;				1/2 /

		; At t608, pixel 480, end of visible area

		out	PORTC, r1		;				1

		; At t609
		rjmp	d13endofline		;				2

9:		; At t95

		DE511

		; At t606
		rjmp	r1d15endofline

;----------------------------------------------------------------------------------------------

lh_dotsvu:	; At t86

		movw	r26, r20		;				1
		lsr	r27			;				1
		ror	r26			;				1
		ldi	r27, 0x03		;				1
		ld	r17, X			;				2

		cpi	r17, 95			;				1
		brsh	9f			;				1/2

		ldi	r27, 0x04		;				1
		ld	r22, X			;				2

		rol	r22
		rol	r22
		rol	r22
		andi	r22, 3
		ldi	r30, lo8(palette)
		ldi	r31, hi8(palette)
		add	r30, r22
		adc	r31, r1
		ld	r22, Z

		ldi	r18, 95			;				1
		sub	r18, r17		;				1

		DE19

		; At t128

		out	PORTC, r1		;				1

1:		subi	r17, 1			;				1   \_ 3x + 2
		brsh	1b			;				1/2 /

		DE1
		out	PORTC, r22		;				1
		DE1
		out	PORTC, r1		;				1

2:		subi	r18, 1			;				1   \_ 3x + 2
		brsh	2b			;				1/2 /

		DE1

		; At t423
		rcall	vu			;				183

		; At t606
		rjmp	r1d15endofline		;				2

9:		; At t95

		DE328

		; At t423
		rcall	vu

		DE2

		out	PORTC, r1		;				1
		rjmp	d13endofline		;				2

;----------------------------------------------------------------------------------------------

lh_stars2d:	; At t86

		DE20

		mov	r22, r20		;				1
		lsr	r22			;				1

		subi	r22, 97			;				1
		mul	r22, r22		;				2
		mov	r17, r0			;				1
		eor	r17, r1			;				1

		subi	r22, 37			;				1
		com	r22			;				1
		andi	r22, 3			;				1
		inc	r22			;				1
		lds	r16, framecount		;				2
		mul	r16, r22		;				2
		sub	r17, r0			;				1

		eor	r1, r1			;				1

		ldi	r22, 0x3f		;				1

		ldi	r18, 157		;				1
		sub	r18, r17		;				1

		; At t126

		cpi	r17, 157		;				1
		brsh	9f			;				1/2

		; At t128

		out	PORTC, r1		;				1

1:		subi	r17, 1			;				1   \_ 3x + 2
		brsh	1b			;				1/2 /

		DE1
		out	PORTC, r22		;				1
		DE1
		out	PORTC, r1		;				1

2:		subi	r18, 1			;				1   \_ 3x + 2
		brsh	2b			;				1/2 /

		; At t608, pixel 480, end of visible area

		out	PORTC, r1		;				1

		; At t609
		rjmp	d13endofline

9:
		DE493
		rjmp	endofline

;----------------------------------------------------------------------------------------------

lh_flower:	; At t86

		DE3

intoflower:	; At t89

		mov	r18, r7			;				1
		ldi	r22, 0x2b		;				1
		ldi	r23, 0x3f		;				1

		DE327

		rcall	flower			;				46

		DE157

		; At t622
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_julia1:	; At t86

		DE37

		ldi	r18, 0x00		;				1

		mov	r26, r7			;				1
		lsl	r26			;				1
		andi	r26, 0xf0		;				1
		ldi	r27, 0x03		;				1

		; At t128
		out	PORTC, r18		;				1

		DE18

		ldi	r28, 8			;				1

		; At t148

		rcall	pixelgfx		;				16 + 32*8 = 272

		; At t420

		DE2
		out	PORTC, r1		;				1

		; At t423
		rcall	vu			;				183

		; At t606
		rjmp	r1d15endofline		;				2

;----------------------------------------------------------------------------------------------

lh_julia2:	; At t86

		DE27

		ldi	r18, 0x00		;				1
		lds	r22, palette+0		;				2
		lds	r23, palette+1		;				2
		lds	r24, palette+2		;				2
		lds	r25, palette+3		;				2

		mov	r26, r7			;				1
		lsl	r26			;				1
		com	r26			;				1
		andi	r26, 0xf0		;				1
		ldi	r27, 0x03		;				1
		subi	r26, 256-8		;				1

		; At t128
		out	PORTC, r18		;				1

		DE29

		ldi	r28, 8			;				1
		ld	r17, -X			;				2

1:		movw	r18, r22		;				1
		sbrc	r17, 1			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r17, 0			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		nop				;				1
		mov	r29, r17		;				1

		movw	r18, r22		;				1
		sbrc	r29, 3			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 2			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		ld	r17, -X			;				2

		movw	r18, r22		;				1
		sbrc	r29, 5			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 4			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		nop				;				1
		dec	r28			;				1

		movw	r18, r22		;				1
		sbrc	r29, 7			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 6			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		brne	1b			;				1/2

		DE6
		out	PORTC, r1		;				1

		; At t423
		rcall	vu			;				183

		; At t606
		rjmp	r1d15endofline		;				2

;----------------------------------------------------------------------------------------------

lh_dblbuf:	; At t86

		DE34

		mov	r26, r7			;				1
		lsl	r26			;				1
		andi	r26, 0xf0		;				1
		ldi	r27, 0x03		;				1
		sbrc	r7, 7			;				1/2 \_ 2
		ldi	r27, 0x04		;				1   /

		sbis	GPIOR0, GFLAG_DBLBUF	;				1/2 \_ 2
		ori	r26, 0x08		;				1   /

		; At t128
		out	PORTC, r1		;				1

		DE92

		ldi	r28, 8			;				1

		; At t222

		rcall	pixelgfx		;				16 + 32*8 = 272

		; At t494

		DE2
		out	PORTC, r1		;				1

		; At t497

		DE125
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_midpurple:	; At t86

		lds	r18, palette+3		;				2
		DE140

		out	PORTC, r18		;				1
		DE279

		out	PORTC, r1		;				1
		DE113

		; At t622
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_midgrey:	; At t86

		ldi	r18, 0x15		;				1
		DE101

		out	PORTC, r18		;				1
		DE359

		out	PORTC, r1		;				1
		DE73

		; At t622
		rjmp	endofline		;				2

;----------------------------------------------------------------------------------------------

lh_bars:	; At t86

		DE36

		ldi	r27, 0x04		;				1
		mov	r26, r7			;				1
		lsr	r26			;				1

		ldi	r18, 8			;				1

		; At t126

1:		ld	r16, X			;				2
		out	PORTC, r16		;				1
		subi	r26, 256-32		;				1
		DE53
		dec	r18			;				1
		brne	1b			;				1/2

		; At t605

		DE1

		; At t606
		rjmp	r1d15endofline		;				2

;----------------------------------------------------------------------------------------------

lh_2bit48:	; At t86

		mov	r26, r7			;				1
		lsr	r26			;				1
		lsr	r26			;				1
		lsr	r26			;				1
		ldi	r16, 12			;				1
		mul	r16, r26		;				2
		eor	r1, r1			;				1
		ldi	r27, 0x03		;				1
		mov	r26, r0			;				1

		ldi	r28, 12			;				1
		mov	r18, r7			;				1
		subi	r18, 64			;				1
		ldi	r22, 0x2b		;				1
		ldi	r23, 0x3f		;				1
		movw	r14, r22		;				1

		sbis	GPIOR0, GFLAG_FLOWER1	;				1/2 \_ 2
		mov	r22, r1			;				1   /
		sbis	GPIOR0, GFLAG_FLOWER1	;				1/2 \_ 2
		mov	r23, r1			;				1   /
		sbis	GPIOR0, GFLAG_FLOWER2	;				1/2 \_ 2
		mov	r14, r1			;				1   /
		sbis	GPIOR0, GFLAG_FLOWER2	;				1/2 \_ 2
		mov	r15, r1			;				1   /

		DE2

		; At t112
		rcall	flowerpixelgfx		;				46 + 16 + 32*12 = 400

		movw	r22, r14		;				1
		mov	r18, r7			;				1
		out	PORTC, r1		;				1

		; At t561

		rcall	flower			;				46

		; At t607

		DE15

		; At t622
		rjmp	endofline

;----------------------------------------------------------------------------------------------

r1d15endofline:
		out	PORTC, r1		;				1
		DE2
d13endofline:
		DE13

endofline:	
		; At t624

		add	r20, r2			; increment y position		1
		adc	r21, r1			;				1

		; At t626

		cpi	r20, 224		; last rasterline (if 2nd half)	1
		sbrc	r21, 0			; skip if first half		1/2 \_ 2 for a normal video line
		breq	endofframe		;				1/2 /

		; At t629

		lds	r26, copperptr		; X = copperptr			2
		lds	r27, copperptr+1	;				2
		rjmp	vline			;				2

endofframe:	; Now, the clock cycles aren't that important.

		rcall	soundroutine

		; trigger main loop if it is waiting for vblank
		cbi	GPIOR0, GFLAG_VBLANK

		sts	vbltime, r1		; next vbltime will be zero

		ldi	r16, 2			; clear timer flag, disregarding any timer interrupts that
						; we got during the visible frame
		out	TIFR1, r16

		; Now it's time for a vblank interrupt, but there is no hardware support for it.
		; Instead, we enable interrupts here (recursively, since we're still in the timer interrupt).

		sei

		; Now we call the vblank handler directly.
		; It may trash any registers that we've already pushed.

		rcall	vblank

		; In the meantime, timer interrupts may occur and be handled.

		pop	r7			;				2
		pop	r14			;				2
		pop	r15			;				2
		pop	r18			;				2
		pop	r19			;				2
		pop	r20			;				2
		pop	r21			;				2
		pop	r22			;				2
		pop	r23			;				2
		pop	r24			;				2
		pop	r25			;				2
		pop	r26			;				2
		pop	r27			;				2
		pop	r28			;				2
		pop	r29			;				2
		pop	r30
		pop	r31

		rjmp	retafterframe

flowerpixelgfx:
		rcall	flower
pixelgfx:
		; Input: X points to a block of r28 pixel bytes.

		; Timing: 3 (for rcall) + 10 + (5 + 8 + 8 + 8 + 3) * r28 + 3 = 16 + 32 * r28
		; First visible pixel is 3 + 10 + 5 = 18 cycles after rcall.

		lds	r22, palette+0		;				2
		lds	r23, palette+1		;				2
		lds	r24, palette+2		;				2
		lds	r25, palette+3		;				2

		ld	r17, X+			;				2

		; At t124

1:		movw	r18, r22		;				1
		sbrc	r17, 7			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r17, 6			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		nop				;				1
		mov	r29, r17		;				1

		movw	r18, r22		;				1
		sbrc	r29, 5			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 4			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		ld	r17, X+			;				2

		movw	r18, r22		;				1
		sbrc	r29, 3			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 2			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		nop				;				1
		dec	r28			;				1

		movw	r18, r22		;				1
		sbrc	r29, 1			;				1/2 \_ 2
		movw	r18, r24		;				1   /
		sbrc	r29, 0			;				1/2 \_ 2
		mov	r18, r19		;				1   /
		out	PORTC, r18		;				1

		brne	1b			;				1/2

		ret				;				4

vu:
		; Total: 3 + 180

		in	r18, PORTD		;				1
		bld	r18, 1			;				1
		bst	r18, 3			; [0,255]			1
		lsr	r18			;				1

		ldi	r19, 127		;				1
		sub	r19, r18		;				1

		ldi	r23, 0x3f		;				1
		ldi	r24, 0x2b		;				1

		mov	r16, r18		;				1
		rcall	delayxplus16		;				16 + r19
		out	PORTC, r23		;				1
		DE1
		out	PORTC, r24		;				1
		DE5
		ldi	r22, 0x05		;				1
		out	PORTC, r1		;				1
		DE1
		out	PORTC, r22		;				1
		mov	r16, r19		;				1
		rjmp	delayxplus16		;				15 + r18

flower:
		cpi	r18, 29			;				1
		brsh	9f			;				1/2
		ldi	r30, lo8(pm(flower_jumptable))	;			1
		ldi	r31, hi8(pm(flower_jumptable))	;			1
		add	r30, r18		;				1
		adc	r31, r1			;				1
		ijmp				;				37

9:
		DE36
		ret				;				4

