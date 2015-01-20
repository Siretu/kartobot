all:		mksine mkfreq mkstory convertflower mkdelays mktitle space mkfont

mksine:		mksine.c
		gcc -o $@ $^ -D_GNU_SOURCE -std=c99 -lm

mkfreq:		mkfreq.c
		gcc -o $@ $^ -D_GNU_SOURCE -std=c99 -lm
