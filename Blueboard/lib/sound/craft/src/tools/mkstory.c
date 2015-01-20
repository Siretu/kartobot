#include <stdio.h>
#include <stdint.h>
#include <string.h>

char *segnames[256];
int nseg = 0;

enum {
	C_WAIT,
	C_CALL,
	C_MAINLOOP,
	C_VBLANK,
	C_PALETTE,
	C_COPPER
};

#define CMDBITS 3
#define JUMPBITS 6

uint8_t bits = 0;
uint8_t nbits = 0;
int xpos = 0;
int bytecount = 0;

char scroll1[] = "a piece of microcontroller madness for breakpoint 2oo8";
char scroll2[] = "greetings      adhoc * asd * breakpoint crew * conspiracy * digital devotion * fairlight * kewlers * kryo * mfx * splendence * tbl * thec * traction * virt";

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

int lookup(char *key, int pref) {
	int i;
	char buf[128];

	if(pref) {
		snprintf(buf, sizeof(buf), "lh_%s", key);
		key = buf;
	}

	for(i = 0; i < nseg; i++) {
		if(!strcmp(segnames[i], key)) return i;
	}
	segnames[nseg++] = strdup(key);
	return i;
}

void encodetext(char *s) {
	int i;

	for(i = 0; i < strlen(s); i++) {
		int ch = 0;

		if(s[i] >= 'a' && s[i] <= 'z') ch = s[i] - 'a' + 1;
		if(s[i] == '2') ch = 27;
		if(s[i] == '8') ch = 28;
		if(s[i] == '*') ch = 29;
		if(s[i] == '+') ch = 30;
		chunk(5, ch);
	}
}

void scrolltext(char *s) {
	encodetext(s);
	chunk(5, 31);
}

void title(int n, char *t) {
	printf("title%d:\n", n);
	chunk(4, strlen(t));
	encodetext(t);
	flushbits();
}

int main() {
	char buf[1024], p2[1024];
	int p1, p3, p4, p5;
	int inlist = 0;
	int i;
	int lastat = 0;

	printf("\t.global\tstory\n");
	printf("\t.global\tjumptable\n");
	printf("\t.global\tscroll1\n");
	printf("\t.global\tscroll2\n");
	printf("\t.global\tcredtable\n");

	printf("story:\n");

	lookup("endless", 1);
	lookup("delay7", 0);
	lookup("vu", 1);

	while(fgets(buf, sizeof(buf), stdin)) {
		while(strlen(buf) && buf[strlen(buf) - 1] == 10) buf[strlen(buf) - 1] = 0;
		if(2 == sscanf(buf, "at %d.%d", &p1, &p3)) {
			//printf("wait for %04x\n", p1);
			//fprintf(stderr, "%d\n", (((p1 << 7) | (p3 << 2)) >> 3) - lastat);
			lastat = ((p1 << 7) | (p3 << 2)) >> 3;
			chunk(CMDBITS, C_WAIT);
			chunk(11, ((p1 << 7) | (p3 << 2)) >> 3);
		} else if(1 == sscanf(buf, " call %s", p2)) {
			//printf("call %s (%d)\n", p2, lookup(p2, 0));
			chunk(CMDBITS, C_CALL);
			chunk(JUMPBITS, lookup(p2, 0));
		} else if(1 == sscanf(buf, " mainloop %s", p2)) {
			//printf("set mainloop vector to %s (%d)\n", p2, lookup(p2, 0));
			chunk(CMDBITS, C_MAINLOOP);
			chunk(JUMPBITS, lookup(p2, 0));
		} else if(1 == sscanf(buf, " vblank %s", p2)) {
			//printf("set vblank vector to %s (%d)\n", p2, lookup(p2, 0));
			chunk(CMDBITS, C_VBLANK);
			chunk(JUMPBITS, lookup(p2, 0));
		} else if(4 == sscanf(buf, " palette %i, %i, %i, %i", &p1, &p3, &p4, &p5)) {
			//printf("load palette %x, %x, %x, %x\n", p1, p3, p4, p5);
			chunk(CMDBITS, C_PALETTE);
			chunk(6, p1);
			chunk(6, p3);
			chunk(6, p4);
			chunk(6, p5);
		} else if(inlist && 2 == sscanf(buf, " %s %i", p2, &p1)) {
			//printf("\tsegment %s (%d), %d lines\n", p2, lookup(p2, 1), p1);
			chunk(JUMPBITS, lookup(p2, 1));
			chunk(8, p1 - 1);
		} else if(!strcmp(buf, "\t[")) {
			//printf("load copperlist\n");
			inlist = 1;
			chunk(CMDBITS, C_COPPER);
		} else if(!strcmp(buf, "\t]")) {
			//printf("end of copperlist\n");
			inlist = 0;
			chunk(JUMPBITS, 0);
		}
	}
	flushbits();

	printf("\nscroll1:\n");
	scrolltext(scroll1);
	flushbits();

	printf("\nscroll2:\n");
	scrolltext(scroll2);
	flushbits();

	printf("\n");
	if(bytecount & 1) chunk(8, 0);
	flushbits();

	printf("\n\njumptable:\n");
	for(i = 0; i < nseg; i++) {
		printf("\trjmp\t%s\n", segnames[i]);
	}

	printf("\n\n\t\t.section .eeprom\n");

	title(1, "lft presents");
	title(2, "low     ");
	title(3, "fidelity");
	title(4, "    high");
	title(5, " quality");
	title(6, "lft 2oo8");

	printf("\ncredtable:\n");
	printf("\t\t.word title1\n");
	printf("\t\t.word title2\n");
	printf("\t\t.word title3\n");
	printf("\t\t.word title4\n");
	printf("\t\t.word title5\n");
	printf("\t\t.word title6\n");

	printf("\n\n\t\t.ascii\t\"/lft\"\n");

	return 0;
}
