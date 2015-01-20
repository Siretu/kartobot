// Basic code from ChaN, minor modifications by Martin Thomas

#include <stdarg.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "term_io.h"
#include <stdio.h>
#include "main.h"

#define IN_BUFFER_SIZE 32U
static uint8_t inBuffer[IN_BUFFER_SIZE];
static volatile uint16_t inW;
static volatile uint16_t inR;

#define OUT_BUFFER_SIZE 256U
static uint8_t outBuffer[OUT_BUFFER_SIZE];
static volatile uint16_t outW;
static volatile uint16_t outR;

static xSemaphoreHandle in_lock, out_lock;

void term_io_Init(){
	in_lock = xSemaphoreCreateRecursiveMutex();
	out_lock = xSemaphoreCreateRecursiveMutex();
}

void term_io_put(char c){
	while((uint16_t)(inW-inR) == IN_BUFFER_SIZE){
//		portYIELD();
		vTaskDelay(1);
	}
	inBuffer[inW++ % IN_BUFFER_SIZE] = c;
}

uint16_t term_io_available(){
	return outW - outR;
}

uint16_t term_io_get(char *buff, uint16_t len){
	uint16_t read = 0;

	while((uint16_t)(outW - outR) != 0 && read < len){
		*buff++ = outBuffer[outR++ % OUT_BUFFER_SIZE];
		read++;
	}

	return read;
}

//Fault version
uint16_t term_io_getF(char *buff, uint16_t len){
	uint16_t read = 0;

	while((uint16_t)(outW - outR) != 0 && read < len){
		*buff++ = outBuffer[outR++ % OUT_BUFFER_SIZE];
		read++;
	}

	return read;
}

//Fault version
void xputsF (const char* str){
	while (*str){
		if(outW-outR == OUT_BUFFER_SIZE){
			break;
		}
		outBuffer[outW++ % OUT_BUFFER_SIZE] = *str++;
	}

}

static char xgetc(){
	while((uint16_t)(inW-inR) == 0){
//		portYIELD();
		vTaskDelay(1);
	}

	return inBuffer[inR++ % IN_BUFFER_SIZE];
}

uint16_t xavail(){
	xSemaphoreTakeRecursive(in_lock, portMAX_DELAY);

	uint16_t available = inW - inR;

	xSemaphoreGiveRecursive(in_lock);
	return available;
}

int xatoi (char **str, long *res)
{
	uint32_t val;
	uint8_t c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return 1;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return 0;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return 0;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;
		}
		if (c >= radix) return 0;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return 1;
}

static void xputc (char c)
{
	while(outW-outR == OUT_BUFFER_SIZE){
		vTaskDelay(1 / portTICK_RATE_MS);
	}
	outBuffer[outW++ % OUT_BUFFER_SIZE] = c;
}

void xputs (const char* str)
{
	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);

	while (*str){
		xputc(*str++);
	}

	xSemaphoreGiveRecursive(out_lock);
}

void xitoa (long val, int radix, int len)
{
	uint8_t c, r, sgn = 0, pad = ' ';
	uint8_t s[20], i = 0;
	uint32_t v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;

	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);

	do
		xputc(s[--i]);
	while (i);

	xSemaphoreGiveRecursive(out_lock);
}



void xprintf (const char* str, ...){
	va_list arp;
	int d, r, w, s, l;

	va_start(arp, str);

	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);

	while ((d = *str++) != 0) {
		if (d != '%') {
			xputc(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			xputs(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			xputc((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas to increase compatibility
		if (d == 'b') r = 2;
		if (!r) break;
		if (l) {
			xitoa((long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				xitoa((unsigned long)va_arg(arp, int), r, w);
			else
				xitoa((long)va_arg(arp, int), r, w);
		}
	}

	xSemaphoreGiveRecursive(out_lock);

	va_end(arp);
}

void xprintfD (const char* str, ...){
	va_list arp;

	va_start(arp, str);

	int size = vsnprintf(NULL, 0, str, arp);
	char * buf = pvPortMalloc(size + 2);
	if(buf != NULL){
		vsprintf (buf, str, arp);
		buf[size] = '\n';
		buf[size+1] = 0;
		xputsDelayed(buf);
	}

	va_end(arp);
}

void put_dump (const uint8_t *buff, uint32_t ofs, int cnt)
{
	uint8_t n;

	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);

	xprintf("%08lX ", ofs);
	for(n = 0; n < cnt; n++)
		xprintf(" %02X", buff[n]);
	xputc(' ');
	for(n = 0; n < cnt; n++) {
		if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
			xputc('.');
		else
			xputc(buff[n]);
	}
	xputc('\n');

	xSemaphoreGiveRecursive(out_lock);
}

void get_line (char *buff, int len)
{
//	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);
	xSemaphoreTakeRecursive(in_lock, portMAX_DELAY);

	char c;
	int idx = 0;

	for (;;) {
		c = xgetc();
		if (c == '\r') break;
		if ((c == '\b') && idx) {
			idx--; //xputc(c);
			//xputc(' '); xputc(c); // added by mthomas for Eclipse Terminal plug-in
		}
		if (((uint8_t)c >= ' ') && (idx < len - 1)) {
				buff[idx++] = c; //xputc(c);
		}
	}
	buff[idx] = 0;
	//xputc('\n');

	xSemaphoreGiveRecursive(in_lock);
	xSemaphoreGiveRecursive(out_lock);
}

// function added by mthomas:
int get_line_r (char *buff, int len, int* idx)
{
	portBASE_TYPE xRunningPrivileged = prvRaisePrivilege();
//	xSemaphoreTakeRecursive(out_lock, portMAX_DELAY);
	xSemaphoreTakeRecursive(in_lock, portMAX_DELAY);

	char c;
	int retval = 0;
	int myidx;

	if ( xavail() ) {
		myidx = *idx;
		c = xgetc();
		if (c == '\r') {
			buff[myidx] = 0;
			//xputc('\n');
			retval = 1;
		} else {
			if ((c == '\b') && myidx) {
				myidx--; //xputc(c);
				//xputc(' '); xputc(c); // added by mthomas for Eclipse Terminal plug-in
			}
			if (((uint8_t)c >= ' ') && (myidx < len - 1)) {
					buff[myidx++] = c; //xputc(c);
			}
		}
		*idx = myidx;
	}

	xSemaphoreGiveRecursive(in_lock);
//	xSemaphoreGiveRecursive(out_lock);
	portRESET_PRIVILEGE(xRunningPrivileged);

	return retval;
}

