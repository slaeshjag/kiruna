#ifndef __UART_H__
#define	__UART_H__

#include <stdint.h>

void uart_init();
void uart_send_raw(unsigned char *data, int bytes);
void uart_send_char(unsigned char c);
void uart_send_string(char *s);
void uart_send_hex(unsigned int data);
int uart_printf(char *format, ...);
uint8_t uart_recv_char(void);
void uart_recv_raw(unsigned char *buff, int bytes);
uint16_t uart_recv_try(void);
void uart_loop();


#endif
