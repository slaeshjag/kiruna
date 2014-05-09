#ifndef __UART_BUFFER_H__
#define	__UART_BUFFER_H__


#define UART_BUFFER_SIZE	64

void uart_buff_loop();
void uart_get_data(unsigned char *buff, int len);

#endif
