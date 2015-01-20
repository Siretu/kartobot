#include "delays.i"
PORTC	=	0x08

		.global	flower_jumptable

flower_r1d2r22r:
		out PORTC, r1
		DE2
		out PORTC, r22
		ret
		
flower_r1r22d1r:
		out PORTC, r1
		out PORTC, r22
		DE1
		ret

flower_d6r1r:	nop
flower_d5r1r:	nop
flower_d4r1r:	nop
flower_d3r1r:	DE3
		out PORTC, r1
		ret

flower_d3r22r:	nop
flower_d2r22r:	nop
flower_d1r22r:	nop
flower_r22r:	out PORTC, r22
		ret

flower_line0:
		DE10
		rcall flower_r22r
		out PORTC, r1
		DE10ret
flower_line1:
		DE13
		out PORTC, r22
		DE3
		rjmp flower_r1de9r
flower_line2:
		DE9
		rcall flower_r22r
		rjmp flower_r1de9r
flower_line3:
		DE9
		rcall flower_r22r
		DE2
		out PORTC, r1
		DE4
		out PORTC, r22
		DE1
		out PORTC, r1
		DE2ret
flower_line4:
		rcall flower_d1r22r
		out PORTC, r1
		DE2
		out PORTC, r22
		DE6
		out PORTC, r1
		DE2
		out PORTC, r22
		rjmp flower_d3r1r
flower_line5:
		rcall flower_r22r
		DE1
		out PORTC, r1
		DE2
		out PORTC, r22
		DE6
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d4r1r
flower_line6:
		rcall flower_r22r
		DE2
		out PORTC, r1
		DE1
		out PORTC, r22
		DE6
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d4r1r
flower_line7:
		rcall flower_r22r
		rcall flower_r1r22d1r
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d5r1r
flower_line8:
		rcall flower_d1r22r
		DE2
		out PORTC, r1
		out PORTC, r22
		DE5
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d5r1r
flower_line9:
		rcall flower_d1r22r
		DE2
		out PORTC, r1
		DE1
		out PORTC, r22
		DE1
		rcall flower_r1r22d1r
		DE2
		out PORTC, r1
		DE1ret
flower_line10:
		rcall flower_d2r22r
		DE2
		out PORTC, r1
		DE1
		out PORTC, r22
		DE2
		out PORTC, r1
		DE1
		out PORTC, r22
		DE7
		out PORTC, r1
		DE1ret
flower_line11:
		rcall flower_d3r22r
		DE2
		out PORTC, r1
		out PORTC, r22
		DE2
		out PORTC, r1
		DE1
		out PORTC, r22
		DE5
		out PORTC, r1
		DE3ret
flower_line12:
		DE9
		out PORTC, r22
		DE2
		out PORTC, r1
		rcall flower_d3r22r
		out PORTC, r1
		DE4ret
flower_line13:
		DE1
		out PORTC, r22
		DE6
		out PORTC, r1
		DE4
		out PORTC, r23
		DE3
		out PORTC, r1
		DE1
		out PORTC, r22
		DE1
		out PORTC, r1
		DE7ret
flower_line14:
		DE1
		out PORTC, r22
		DE7
		out PORTC, r1
		DE2
		out PORTC, r23
		DE5
		out PORTC, r1
		DE10ret
flower_line15:
		out PORTC, r22
		DE9
		out PORTC, r1
		DE1
		out PORTC, r23
		DE5
		out PORTC, r1
		DE1
		out PORTC, r22
		DE4
		out PORTC, r1
		DE3ret
flower_line16:
		out PORTC, r22
		DE10
		out PORTC, r1
		out PORTC, r23
		DE4
		out PORTC, r1
		DE1
		out PORTC, r22
		DE6
		out PORTC, r1
		DE2ret
flower_line17:
		DE1
		out PORTC, r22
		DE8
		out PORTC, r1
		DE2
		out PORTC, r23
		out PORTC, r1
		rcall flower_r22r
		DE4
		out PORTC, r1
		DE1ret
flower_line18:
		DE1
		out PORTC, r22
		DE6
		out PORTC, r1
		DE1
		out PORTC, r22
		DE1
		out PORTC, r1
		DE2
		out PORTC, r22
		out PORTC, r1
		DE2
		out PORTC, r22
		rjmp flower_d6r1r
flower_line19:
		DE3
		out PORTC, r22
		DE1
		out PORTC, r1
		DE3
		out PORTC, r22
		DE3
		out PORTC, r1
		out PORTC, r22
		DE3
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d5r1r
flower_line20:
		DE2
		rcall flower_d2r22r
		DE1
		out PORTC, r1
		out PORTC, r22
		DE4
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d4r1r
flower_line21:
		rcall flower_d3r22r
		DE2
		out PORTC, r1
		out PORTC, r22
		DE5
		out PORTC, r1
		DE1
		out PORTC, r22
		rjmp flower_d3r1r
flower_line22:
		rcall flower_d2r22r
		rcall flower_r1r22d1r
		out PORTC, r1
		DE3
		out PORTC, r22
		DE2
		out PORTC, r1
		DE1ret
flower_line23:
		rcall flower_d2r22r
		DE3
		out PORTC, r1
		DE1
		out PORTC, r22
		DE5
		out PORTC, r1
		DE7ret
flower_line24:
		rcall flower_d1r22r
		rcall flower_r1d2r22r
		DE1
		out PORTC, r1
		DE7ret
flower_line25:
		rcall flower_d1r22r
		rcall flower_r1d2r22r
		DE2
		out PORTC, r1
		DE6ret
flower_line26:
		DE1
		rcall flower_r22r
		DE2
		out PORTC, r1
		rcall flower_r22r
		DE2
		out PORTC, r1
		DE6ret
flower_line27:
		DE5
		out PORTC, r22
		DE2
		out PORTC, r1
		DE1
		rcall flower_d3r22r
		out PORTC, r1
		DE7ret

flower_jumptable:
		rjmp	flower_line0
		rjmp	flower_line1
		rjmp	flower_line2
		rjmp	flower_line3
		rjmp	flower_line4
		rjmp	flower_line5
		rjmp	flower_line6
		rjmp	flower_line7
		rjmp	flower_line8
		rjmp	flower_line9
		rjmp	flower_line10
		rjmp	flower_line11
		rjmp	flower_line12
		rjmp	flower_line13
		rjmp	flower_line14
		rjmp	flower_line15
		rjmp	flower_line16
		rjmp	flower_line17
		rjmp	flower_line18
		rjmp	flower_line19
		rjmp	flower_line20
		rjmp	flower_line21
		rjmp	flower_line22
		rjmp	flower_line23
		rjmp	flower_line24
		rjmp	flower_line25
		rjmp	flower_line26
		rjmp	flower_line27
		;rjmp	flower_line28

flower_line28:
		DE20	; was 18
		out PORTC, r22
flower_r1de9r:	out PORTC, r1
		DE9ret
