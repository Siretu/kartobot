#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main() {
	int i;

	for(i = 0; i < 84; i++) {
		double freq = 480 * pow(pow(2, 1.0/12), i - 9 - 36);
		if(i & 7) {
			printf(", ");
		} else {
			printf("\n\t.word\t");
		}
		printf("0x%04x", (uint16_t) (freq * 65536 / 31496));
	}
	return 0;
}
