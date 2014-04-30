#ifndef __RADIOLINK_H_
#define __RADIOLINK_H_

unsigned char radiolink_send(int size, unsigned char *data);
unsigned char radiolink_recv(int size, unsigned char *data);
void radiolink_init(unsigned char rx_packet_size);

#endif