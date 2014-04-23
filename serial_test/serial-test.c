/* serial-test.c - Steven Arnow <s@rdw.se>,  2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define	PACKET_SIZE		8


struct timeval time_d;

int delta_time() {
	struct timeval time_now;
	int delta;

	gettimeofday(&time_now, NULL);
	delta = time_now.tv_usec - time_d.tv_usec;
	delta += (time_now.tv_sec - time_d.tv_sec) * 1000000;
	time_d = time_now;

	return delta;
}
	

int main(int argc, char **argv) {
	FILE *serial = fopen("/dev/ttyUSB0", "r+");
	FILE *file_in = fopen(argv[1], "r");
	char buff[256];

	if (!serial) {
		fprintf(stderr, "Unable to open serial port\n");
		return -1;
	}

	if (!file_in) {
		fprintf(stderr, "Unable to open input file %s\n", argv[1]);
		return -1;
	}

	gettimeofday(&time_d, NULL);

	for (;;) {
		fread(buff, 1, PACKET_SIZE, file_in);
		fwrite(buff, 1, PACKET_SIZE, serial);
		fread(buff, 1, PACKET_SIZE, serial);
		fwrite(buff, 1, PACKET_SIZE, stdout);
		usleep((1000000 / (8000 / PACKET_SIZE)) - delta_time());
	}
		

	return 0;
}
