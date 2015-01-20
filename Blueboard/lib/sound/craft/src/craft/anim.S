
		.global	animvudown
		.global	animvuright
		.global	preparevu

preparevu:
		sts	copperlist+0, r3
		sts	copperlist+2, r3
		ret

animvudown:
		lds	r16, copperlist+1
		subi	r16, 256-4
		brcc	2f
		sts	copperlist+1, r16
		sts	copperlist+3, r16
		rjmp	vbl_column

animvuright:
		lds	r16, xoffset
		subi	r16, 256-1
		cpi	r16, 48
		brlt	1f
		ldi	r16, 48
1:		sts	xoffset, r16
2:
		ret
