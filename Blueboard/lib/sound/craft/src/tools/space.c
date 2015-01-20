#include <stdio.h>

char buf[1024];
char flash[8192];

int item(int n) {
	char p[4];

	p[0] = buf[1 + 2 * n + 0];
	p[1] = buf[1 + 2 * n + 1];
	p[2] = 0;
	return strtol(p, 0, 16);
}

void touch(int addr) {
	if(addr >= 0 && addr < 8192) {
		flash[addr] = 1;
	} else {
		printf("%08x\n", addr);
	}
}

int main() {
	int len;
	int base = 0;
	int addr;
	int type;
	int i;

	while(fgets(buf, sizeof(buf), stdin)) {
		if(buf[0] == ':') {
			len = item(0);
			addr = (item(1) << 8) | item(2);
			type = item(3);
			switch(type) {
				case 0x00:
					for(i = 0; i < len; i++) touch(base + addr + i);
					break;
				case 0x01:
					break;
				case 0x02:
					printf("S");
					break;
				case 0x04:
					base = ((item(4) << 8) | item(5)) << 16;
					break;
			}
		}
	}

	len = 0;
	for(i = 0; i < 8192; i++) {
		if(flash[i]) len++;
	}

	printf("Used %d of 8192 bytes (%f%%). %d bytes left.\n", len, (double) len * 100 / 8192, 8192 - len);

	return 0;
}
