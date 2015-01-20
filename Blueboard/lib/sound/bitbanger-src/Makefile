CC=gcc
LDFLAGS=-lm
CFLAGS=-std=c99 -D_GNU_SOURCE

all:		flash.hex
		./printsize

flash.hex:	flash.o
		avr-ld --oformat ihex -o $@ $^

flash.o:	main.S delays.i
		avr-gcc -nostdlib -Xlinker -M -Os -B/usr/avr/lib -Wall \
			-mmcu=avr1 -o $@ $< >mapfile
