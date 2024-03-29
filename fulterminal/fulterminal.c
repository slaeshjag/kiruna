#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

struct termios serial_termios;

int serial_init(const char *device) {
	int fd;
	unsigned int flags;
	struct termios termios;
	
	fd = open(device, O_RDWR);
	if(fd < 0)
		return -1;
	
	//tcgetattr(fd, &termios);
	tcgetattr(fd, &serial_termios);
	
	memset(&termios,0,sizeof(termios));
	termios.c_iflag=0;
	termios.c_oflag=0;
	termios.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	termios.c_lflag=0;
	termios.c_cc[VMIN]=1;
	termios.c_cc[VTIME]=5;
	
	cfmakeraw(&termios);
	cfsetospeed(&termios, B115200);
	cfsetispeed(&termios, B115200);
	
	if (tcsetattr(fd, TCSANOW, &termios) < 0) {
		close(fd);
		return -1;
	}
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		flags = 0;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	
	return fd;
}

int main(int argc, char **argv) {
	int fd, audioout;
	char c;
	unsigned char buf = 0xF8;
	unsigned char readbuf[512];
	struct termios ttystate, ttysave;
	int flags, len = 0;
	
	if((fd = serial_init("/dev/ttyUSB0")) < 0) {
		printf("nope\n");
		return 1;
	}
	
	audioout = open("/dev/audioout", O_RDWR);
	if ((flags = fcntl(audioout, F_GETFL, 0)) == -1)
		flags = 0;
	fcntl(audioout, F_SETFL, flags | O_NONBLOCK);
	
	tcgetattr(STDIN_FILENO, &ttysave);
	tcgetattr(STDIN_FILENO, &ttystate);
	//ttysave=ttystate;
	ttystate.c_lflag&=~(ICANON);
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	
	if ((flags = fcntl(STDIN_FILENO, F_GETFL, 0)) == -1)
		flags = 0;
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	for(;;) {
		if(read(STDIN_FILENO, &c, 1) > 0) {
			switch(c) {
				case 'w':
					buf = 0xF8 | 0x7;
					break;
				case 'a':
					buf = 0xF8 | 0x5;
					break;
				case 's':
					buf = 0xF8 | 0x4;
					break;
				case 'd':
					buf = 0xF8 | 0x6;
					break;
				case 'm':
					buf = 0x0;
					break;
				
				default:
					buf = 0xF8;
					break;
			}
			printf("wrote %i\n", write(fd, &buf, 1));
		}
		
		len = read(fd, readbuf, 512);
		if(len > 0)
			write(STDOUT_FILENO, readbuf, len);
		
		//usleep(50000);
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
	tcsetattr(STDIN_FILENO, TCSANOW, &serial_termios);
	
	return 0;
}
