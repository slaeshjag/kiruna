#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PRINT_TYPE(type, argtype) \
s = int_to_string((type) va_arg(va, argtype), buf + 24, base); \
j = 24 + buf - s; \
if(!width) \
	width = j; \
else \
	while(width > j)  {\
		uart_send_char(pad); \
		width--; \
	} \
uart_send_raw((unsigned char *) s, width)

void uart_send_char(char c) {
	while (!(LPC_UART->LSR & (1 << 5)));
	LPC_UART->THR = c;
}

void uart_send_raw(unsigned char *data, int bytes) {
	int i;
	for (i = 0; i < bytes; i++) {
		while (!(LPC_UART->LSR & (1 << 5)));
		LPC_UART->THR = data[i];
	}

	return;
}

void uart_send_string (char *s) {
	while(*s) {
		while (!(LPC_UART->LSR & (1 << 5)));
		LPC_UART->THR = *s++;
	}
}

void uart_send_hex(unsigned int data) {
	char out[8];
	int i/*, n*/;

	//n = 0;
	uart_send_string("0x");
	util_bin_to_str((void *) &data, out, 4);
	for (i = 7; i >= 0; i--) {
/*		if (out[i] == '0' && !n)
			continue;
		n = 1;*/
		uart_send_raw((unsigned char *) &out[i], 1);
	}

	/*if (out[0] == '0')
		uart_send("0", 1);*/
	uart_send_string(" ");

	return;
}

static char *int_to_string(unsigned long long int n, char *s, int base) {
	char i;
	do {
		s--;
		i = n  % base;
		i += (i > 9 ? 'A' - 10 : '0');
		*s = i;
		n /= base;
	} while(n);
	return s;
}

int uart_printf(char *format, ...) {
	#ifdef DEBUG
	//TODO: handle signed values
	unsigned char pad, c;
	int i;
	unsigned int j;
	va_list va;
	
	enum {
		LENGTH_CHAR,
		LENGTH_SHORT,
		LENGTH_INT,
		LENGTH_LONG,
		LENGTH_LONG_LONG,
		LENGTH_INTMAX_T,
		LENGTH_SIZE_T,
		LENGTH_PTRDIFF_T,
	} length;
	int width;
	char prefix, base, *s;
	char buf[25];
	buf[24] = 0;
	
	va_start(va, format);
	for(i=0; (c = *format++); i++) {
		if(c != '%') {
			uart_send_char(c);
			continue;
		}
		length = LENGTH_INT;
		width = 0;
		prefix = 0;
		pad = ' ';
		
		while(1) {
			switch(c = *format++) {
				case 0:
					goto end;
				case '%':
					uart_send_char(c);
					goto next;
				case '#':
					prefix = 1;
					break;
				case '0':
					if(!width) {
						pad = '0';
						break;
					}
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					width = width*10 + (c - '0');
					break;
				case 'h':
					length = length == LENGTH_SHORT ? LENGTH_CHAR : LENGTH_SHORT;
					break;
				case 'l':
					length = length == LENGTH_LONG ? LENGTH_LONG_LONG : LENGTH_LONG;
					break;
				case 'j':
					length = LENGTH_INTMAX_T;
					break;
				case 'z':
					length = LENGTH_SIZE_T;
					break;
				case 't':
					length = LENGTH_PTRDIFF_T;
					break;
				case 'o':
					base = 8;
					if(prefix)
						uart_send_char('0');
					goto baseconv;
				case 'p':
					length = sizeof(void *);
					prefix = 1;
				case 'x':
				case 'X':
					base = 16;
					if(prefix)
						uart_send_string("0x");
					goto baseconv;
				case 'u':
				case 'd':
				case 'i':
					base = 10;
					baseconv:
					switch(length) {
						case LENGTH_CHAR:
							PRINT_TYPE(unsigned char, unsigned int);
							break;
						case LENGTH_SHORT:
							PRINT_TYPE(unsigned short, unsigned int);
							break;
						case LENGTH_INT:
							PRINT_TYPE(unsigned int, unsigned int);
							break;
						case LENGTH_LONG:
							PRINT_TYPE(unsigned long, unsigned long);
							break;
						case LENGTH_LONG_LONG:
							PRINT_TYPE(unsigned long long, unsigned long long);
							break;
						default:
							break;
					}
					goto next;
				case 's':
					uart_send_string(va_arg(va, char *));
					goto next;
			}
		}
		next:;
	}
	end:
	va_end(va);
	return i;
	#else
	return 0;
	#endif
}

uint8_t uart_recv_char(void) {
	while(!(LPC_UART->LSR & 1));
	return LPC_UART->RBR;
}

void uart_loop(void) {
	char cmd[16]/*, done[17]*/;
	int i, /*j,*/ command, arg;

	uart_send_string(" > ");

	for (i = 0;; ) {
		if (LPC_UART->LSR & 1) {
			cmd[i] = LPC_UART->RBR;
			if (i == 15)
				LPC_UART->THR = '\b';

			LPC_UART->THR = cmd[i];
			if ((cmd[i] >= '0' && cmd[i] <= '9') || (cmd[i] >= 'A' && cmd[i] <= 'F') || cmd[i] == ' ') {
				if (i < 15)
					i++;
			} else if (cmd[i] == '\n')
				break;
			else if (i < 15)
				LPC_UART->THR = '\b';
		}
	}

	if (!i)
		return;
	command = arg = 0;

	util_str_to_bin(cmd, i);
//	util_bin_to_str(cmd, done, i >> 1);
	memcpy(&command, cmd, 1);
	memcpy(&arg, &cmd[1], 4);
	/* TODO: Call SPI function */

	/* TODO: Wait for response */

	uart_send_string("\n");

	return;
}
