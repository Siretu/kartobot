SRC=boot.S asm.S vblank.S plasma.S mainloop.S fire.S music.S vector.S flower.S tunnel.S julia.S title.S anim.S craft.S songee.S story.S song.S data.S vars.S
all:		flash.hex eeprom.raw

flash.o:	${SRC} delays.i stuff.i
		avr-gcc \
			-Xlinker -Tdata -Xlinker 0x800100 \
			-Xlinker -M \
			-nostdlib \
			-O2 -B/usr/avr/lib -I/usr/local/avr/include -Wall -mmcu=avr4 -D__AVR_ATmega88__ \
			-o $@ ${SRC} >mapfile

flash.hex:	flash.o
		avr-ld --oformat ihex -o $@ $^
		../tools/space <$@

eeprom.raw:	flash.o
		avr-objdump -s -j .eeprom $^ | \
			awk '/^ 81/ {print substr($$0, 9, 8) substr($$0, 18, 8) substr($$0, 27, 8) substr($$0, 36, 8)}' | \
			xxd -r -p >$@

story.S:	storyboard
		../tools/mkstory <storyboard >$@
