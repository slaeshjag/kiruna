/* serial-test.c - Steven Arnow <s@rdw.se>,  2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <asm/termios.h>

#define	PACKET_SIZE		8
#define	BAUDRATE	115200


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

int initiate_serial_port(const char *file){
	struct termios2 config;
	int serial_port;
	
	serial_port = open(file, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
	//Open serial port as ReadWrite.
	if(serial_port < 0) {
		printf("nope.jpg\n");
		exit(1);
	}
	
	ioctl(serial_port, TCGETS2, &config);
	
	config.c_cflag &= ~CBAUD;
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= BOTHER;
	config.c_cflag |= CS8;
	//These options will let us set our own baudrate,
	//, no parity and a bytesize of 8 bits.
	
	config.c_iflag = 0;
	//No input processing.
	
	config.c_oflag = 0;
	//No output processing.
	
	config.c_lflag = 0;
	//No echoing and stuff.
	
	config.c_ispeed = BAUDRATE;
	config.c_ospeed = BAUDRATE;
	//Set our baudrates.
	
	ioctl(serial_port, TCSETS2, &config);
	//Sets the serial_port to work as we want it to,
	//with the custom baudrate BAUDRATE. 
	
	tcflush(serial_port, TCOFLUSH);
	tcflush(serial_port, TCIFLUSH);
	
	return serial_port;
}

int main(int argc, char **argv) {
	FILE *file_in = fopen(argv[1], "r");
	char buff[256];
	int i, serial_fd;
	int waittime;

	serial_fd = initiate_serial_port("/dev/ttyUSB0");

	if (!file_in) {
		fprintf(stderr, "Unable to open input file %s\n", argv[1]);
		return -1;
	}

	gettimeofday(&time_d, NULL);
	
	for (i = 0; i < 63; i++) {
		fread(buff, 1, PACKET_SIZE, file_in);
		write(serial_fd, buff, PACKET_SIZE);
		if((waittime = ((1000000 / (8000 / PACKET_SIZE)) - delta_time())) > 0)
			usleep(waittime);
		fprintf(stderr, "sending some data\n");
	}

	for (;;) {
		fread(buff, 1, PACKET_SIZE, file_in);
		write(serial_fd, buff, PACKET_SIZE);
		read(serial_fd, buff, PACKET_SIZE);
		fwrite(buff, 1, PACKET_SIZE, stdout);
		fflush(stdout);
		if((waittime = ((1000000 / (8000 / PACKET_SIZE)) - delta_time())) > 0)
			usleep(waittime);
	}
		

	return 0;
}
