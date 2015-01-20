#include <stdio.h>

int font[256];

int fontbits(int c, int y) {
	return (c < 32)? font[c * 8 + y] : 0;
}

int main() {
	int c, x, y;
	FILE *pic;
	char buf[3];

	pic = fopen("../gfx/font.ppm", "rb");
	fseek(pic, 0x3a, SEEK_SET);

	for(y = 0; y < 8; y++) {
		for(c = 0; c < 32; c++) {
			font[c * 8 + y] = 0;
			for(x = 0; x < 8; x++) {
				fread(buf, 3, 1, pic);
				if(buf[0]) font[c * 8 + y] |= 0x80 >> x;
			}
		}
	}

	for(y = 0; y < 8; y++) {
		for(c = 0; c < 32; c++) {
			if(c == 0 || c == 16) {
				printf("\n\t.byte\t");
			} else {
				printf(", ");
			}
			printf("0x%02x", fontbits(c, y) ^ 255);
		}
	}
	printf("\n");

	return 0;
}
