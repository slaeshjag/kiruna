#ifndef __OV7670_H__
#define	__OV7670_H__

void ov7670_init(void);
void ov7670_test(void);

void ov7670_fifo_reset();
void ov7670_fifo_unreset();
void ov7670_vsync_reset();
void ov7670_get_data_packet(unsigned char *buf);
void ov7670_test();

#endif
