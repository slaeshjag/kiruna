#ifndef __UART_H__
#define	__UART_H__

void uart_send_raw(unsigned char *data, int bytes);
void uart_send_char(char c);
void uart_send_string(char *s);
void uart_send_hex(unsigned int data);
int uart_printf(char *format, ...);
uint8_t uart_recv_char(void);
void uart_loop();

#endif
