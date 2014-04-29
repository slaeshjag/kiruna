#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"

volatile inline int *gpio_reg_addr(unsigned int port, unsigned int offset) {
	return (volatile int *) (0x50000000 + port * 0x10000 + offset);
}


inline void gpio_set_io(int port, int pin, int status) {
	*gpio_reg_addr(port, 0x8000) |= (1 << pin);
	*gpio_reg_addr(port, 0x8000) ^= (1 << pin);
	*gpio_reg_addr(port, 0x8000) |= (status << pin);
}


inline void gpio_set_pin(int port, int pin, int data) {
	*gpio_reg_addr(port, 0x4 << pin) = data;
}


void util_delay_small(int us) {
	int n;

	LPC_TMR16B0->TCR |= 0x3;
	LPC_TMR16B0->PR = 0;
	n = SYSTEM_CLOCK / 1000000 * us;
	LPC_TMR16B0->TCR &= (~0x2);
	LPC_TMR16B0->CTCR = 0x0;
	while ((LPC_TMR16B0->TC) < n);
	LPC_TMR16B0->TCR = 0x3;

	return;
}


void util_delay(int us) {
	int us_out;

	for (; us > 0;) {
		if (us > 1000)
			us_out = 1000, us -= 1000;
		else
			us_out = us, us = 0;
		util_delay_small(us_out);
	}
}


int util_char_to_nibble(unsigned char n) {
	if (n <= '9')
		return n - '0';
	return n - 'A' + 0xA;
}


char util_nibble_to_char(int n) {
	if (n <= 9)
		return n + '0';
	return n + ('A' - 0xA);
}


void util_bin_to_str(unsigned char *str, char *target, int bytes) {
	int i;

	for (i = 0; i < bytes; i++) {
		target[(i << 1) + 1] = util_nibble_to_char(str[i] >> 4);
		target[(i << 1)] = util_nibble_to_char(str[i] & 0xF);
	}
	
	return;
}


void util_str_to_bin(char *str, int chars) {
	int i, t;

	for (i = 0; i < chars; i++) {
		t =  util_char_to_nibble((unsigned) str[i]) << (i & 1 ? 0 : 4);
		if (!(i & 1))
			str[i >> 1] = 0;
		str[i >> 1] |= t;
	}

	return;
}


void *memcpy(void *dest, void *src, int bytes) {
	char *dst, *sr;
	
	dst = dest;
	sr = src;
	dest += bytes;
	while (dst != dest)
		*(dst++) = *(sr++);
	return dest;
}


void system_reset() {
	uart_send_string("Anarki på skassibussen\n");
	while (!(LPC_GPIO3->DATA & 0x2));
	while(!(LPC_UART->LSR & 0x40));
	NVIC_SystemReset();

	for (;;);
	return;
}
