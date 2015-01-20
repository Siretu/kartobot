#include <stdio.h>

int main() {
	FILE *f = fopen("../gfx/flower.ppm", "rb");
	char buf[3];
	int x, y;
	int wcount = 0;
	char last = -1;

	fseek(f, 58 + 32 * 3, SEEK_SET);
	for(y = 0; y < 29; y++) {
		/*for(x = 0; x < 32; x++) {
			fread(buf, 3, 1, f);
			if(y == 6 && x == 28) buf[0] = -1;
			printf("%s", buf[0]? " " : "*");
		}
		printf("\n");*/

		printf("flower_line%d:\n", y);
		for(x = 0; x < 32; x++) {
			fread(buf, 3, 1, f);
			if(buf[0] != last) {
				if(wcount) {
					printf("\t\tDE%d\n", wcount);
				}
				printf("\t\tout PORTC, r%d\n", buf[0]? 1 : 22);
				last = buf[0];
				wcount = 0;
			} else {
				wcount++;
			}
		}
		if(wcount) {
			printf("\t\tDE%d\n", wcount);
		}
		wcount = 0;
		printf("\t\tret\n");
	}

	return 0;
}
