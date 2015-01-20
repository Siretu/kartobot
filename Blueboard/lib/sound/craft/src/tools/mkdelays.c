#include <stdio.h>

int main() {
	int i;

	for(i = 10; i < 608; i++) {
		printf("#define DE%d ldi r16, %d $ rcall delay3xplus%d\n",
			i,
			(i - 10) / 3,
			6 + (i - 10) % 3);
	}

	return 0;
}
