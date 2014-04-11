#ifndef __UTIL_H__
#define	__UTIL_H__

volatile inline int *gpio_reg_addr(unsigned int port, unsigned int offset);
inline void gpio_set_io(int port, int pin, int status);
inline void gpio_set_pin(int port, int pin, int data);
void util_delay(int us);
void util_delay_tus(int tus);
void util_str_to_bin(char *str, int chars);
void util_bin_to_str(unsigned char *str, char *target, int bytes);
void *memcpy(void *dest, void *src, int bytes);

#endif
