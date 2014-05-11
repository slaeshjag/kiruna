#ifndef __ULTRASONIC_H_
#define __ULTRASONIC_H_

void us_init(void);
void us_trig(void);
int us_read(void);
int us(void);
void us_handler(void);
int us_read_block(void);

#endif
