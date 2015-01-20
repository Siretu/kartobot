#include <stdio.h>
#include <stdint.h>

uint8_t bits = 0;
uint8_t nbits = 0;
int xpos = 0;
int bytecount = 0;

void putbit(int b) {
	if(b) bits |= 0x80 >> nbits;
	nbits++;
	if(nbits == 8) {
		if(!xpos) {
			printf("\t.byte\t");
		} else {
			printf(", ");
		}
		printf("0x%02x", bits);
		bytecount++;
		xpos++;
		if(xpos == 12) {
			printf("\n");
			xpos = 0;
		}
		nbits = 0;
		bits = 0;
	}
}

void chunk(int n, int v) {
	while(n) {
		n--;
		putbit(!!(v & (1 << n)));
	}
}

void flushbits() {
	while(nbits) putbit(0);
	printf("\n");
	xpos = 0;
}
int main() {
	FILE *f;
	uint8_t pix[3];
	int col;
	int x, y;
	int i;

	f = fopen("../gfx/craft.ppm", "rb");
	fseek(f, 0x3a, SEEK_SET);
	for(y = 0; y < 14; y++) {
		for(x = 0; x < 48; x++) {
			fread(pix, 3, 1, f);
			if(pix[0] < 0x50) {
				col = 0;
			} else if(pix[0] < 0x80) {
				col = 1;
			} else if(pix[0] < 0xf0) {
				col = 2;
			} else {
				col = 3;
			}
			chunk(2, col);
		}
	}
}
