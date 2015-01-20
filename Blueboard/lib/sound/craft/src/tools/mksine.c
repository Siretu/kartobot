#include <stdio.h>
#include <math.h>

int main() {
	int i;

	for(i = 0; i < 256; i++) {
		if(i & 15) {
			printf(", ");
		} else {
			printf("\n\t.byte\t");
		}
		printf("0x%02x", (256 + (int) round(127 * sin(i * M_PI * 2 / 256))) & 255);
	}
	return 0;
}
