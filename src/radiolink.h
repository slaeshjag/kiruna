#ifndef __RADIOLINK_H_
#define __RADIOLINK_H_

unsigned char radiolink_send(int size, unsigned char *data);
unsigned char radiolink_send_unreliable(int size, unsigned char *data);
unsigned char radiolink_recv(int size, unsigned char *data);
int radiolink_init(char _packet_size);

#endif
