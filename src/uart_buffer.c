#include "uart.h"
#include "uart_buffer.h"


#ifndef MOTHERSHIP

unsigned char uart_buff[UART_BUFFER_SIZE];
int uart_buff_next = 0;
int uart_buff_cur = 0;

static inline int uart_fifo_next(int fifo) {
	return (fifo + 1 >= UART_BUFFER_SIZE) ? 0 : fifo + 1;
}


void uart_buff_loop() {
	unsigned short data;

	for (; uart_fifo_next(uart_buff_next) != uart_buff_cur;) {
		if (!(data = uart_recv_try()))
			return;
		uart_buff[uart_buff_next++] = data;
		if (uart_buff_next == UART_BUFFER_SIZE)
			uart_buff_next = 0;
	}

	return;
}


void uart_get_data(unsigned char *buff, int len) {
	int i;

	for (i = 0; i < len; i++) {
		while (uart_buff_next == uart_buff_cur)
			uart_buff_loop();
		buff[i] = uart_buff[uart_buff_cur];
		uart_buff_cur = uart_fifo_next(uart_buff_cur);
	}

	return;
}

#endif
