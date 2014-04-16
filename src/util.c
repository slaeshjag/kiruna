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


void util_delay(int us) {
	SysTick->CTRL = 0;
	SysTick->LOAD = us * (SYSTEM_CLOCK / 1000000);
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	while (!(SysTick->CTRL & (1 << 16)));

	return;
}


/* Tiondels mikrosekund */
void util_delay_tus(int tus) {
	SysTick->CTRL = 0;
	SysTick->LOAD = tus * (SYSTEM_CLOCK / 10000000);
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	while (!(SysTick->CTRL & (1 << 16)));
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
