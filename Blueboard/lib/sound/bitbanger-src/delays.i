
#define DE1 nop
#define DE2 rjmp .
#define DE3 lpm
#define DE4 rjmp . $ rjmp .
#define DE5 rjmp . $ lpm
#define DE6 lpm $ lpm

#define DE7 rcall delay7
#define DE8 nop $ rcall delay7
#define DE9 rcall delay9

#define DE10 ldi r16, 1 $ rcall delay3xplus6
#define DE11 ldi r16, 1 $ rcall delay3xplus7
#define DE12 ldi r16, 1 $ rcall delay3xplus8
#define DE13 ldi r16, 2 $ rcall delay3xplus6
#define DE14 ldi r16, 2 $ rcall delay3xplus7
#define DE15 ldi r16, 2 $ rcall delay3xplus8
#define DE16 ldi r16, 3 $ rcall delay3xplus6
#define DE17 ldi r16, 3 $ rcall delay3xplus7
#define DE18 ldi r16, 3 $ rcall delay3xplus8
#define DE19 ldi r16, 4 $ rcall delay3xplus6
#define DE20 ldi r16, 4 $ rcall delay3xplus7
#define DE21 ldi r16, 4 $ rcall delay3xplus8
#define DE22 ldi r16, 5 $ rcall delay3xplus6
#define DE23 ldi r16, 5 $ rcall delay3xplus7
#define DE24 ldi r16, 5 $ rcall delay3xplus8
#define DE25 ldi r16, 6 $ rcall delay3xplus6
#define DE26 ldi r16, 6 $ rcall delay3xplus7
#define DE27 ldi r16, 6 $ rcall delay3xplus8
#define DE28 ldi r16, 7 $ rcall delay3xplus6
#define DE29 ldi r16, 7 $ rcall delay3xplus7
#define DE30 ldi r16, 7 $ rcall delay3xplus8
#define DE31 ldi r16, 8 $ rcall delay3xplus6
#define DE32 ldi r16, 8 $ rcall delay3xplus7
#define DE33 ldi r16, 8 $ rcall delay3xplus8
#define DE34 ldi r16, 9 $ rcall delay3xplus6
#define DE35 ldi r16, 9 $ rcall delay3xplus7
#define DE36 ldi r16, 9 $ rcall delay3xplus8
#define DE37 ldi r16, 10 $ rcall delay3xplus6
#define DE38 ldi r16, 10 $ rcall delay3xplus7
#define DE39 ldi r16, 10 $ rcall delay3xplus8
#define DE40 ldi r16, 11 $ rcall delay3xplus6
#define DE41 ldi r16, 11 $ rcall delay3xplus7
#define DE42 ldi r16, 11 $ rcall delay3xplus8

