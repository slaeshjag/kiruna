#ifndef __ULTRASONIC_H_
#define __ULTRASONIC_H_

void us_init(void);
void us_trig(void);
void us_handler(void);
int us_read_nonblock(void);

#endif
