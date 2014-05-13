#include "main.h"
#include "datatrans.h"
#include "radiolink.h"
#include "audio.h"
#include "motor.h"
#include "ultrasonic.h"
#include "uart.h"

int motor_state = 0;

void trans_get_mic_data() {
	unsigned char buff[16];
	int timeout;

	timeout = global_timer + 40000;

	while (global_timer < timeout) {
		if (radiolink_recv_timeout(16, buff, 2000) == 0xFF)
			return;
		uart_send_raw(buff, 16);
	}

	uart_printf("Done\n");
	return;
}

void trans_send_mic_data() {
	#ifdef MOTHERSHIP
	int i;

	for (i = 0; i < 2500; i++) {
		uart_printf("loop\n");
		microphone_send();
	}
	#endif
	return;
}


void trans_master_loop() {
	unsigned char buff[16];
	
	for (;;) {
		uart_printf("\r\n> ");
		uart_recv_raw(buff, 1);
		uart_printf("\n");
		if (radiolink_send(16, buff) == 0xFF);
		else if (!buff[0]) {
			uart_printf("Getting microphone\n");
			trans_get_mic_data();
		}
	}
}


void trans_slave_loop() {
	unsigned char buff[16];

	us_trig();

	for (;;) {
		uart_printf("Waiting for data\n");
		if (radiolink_recv_timeout(16, buff, 800) == 0xFF);
		else if (!buff[0]) {
			motor_state = 0;
			motor_logic();
			trans_send_mic_data();
		} else
			motor_state = buff[0] & 0x7;
		motor_logic();
	}

	return;
}

void protocol_get_motor(int *left, int *right, int *run) {
	#ifdef MOTHERSHIP
	*left = (motor_state & 01);
	*right = (motor_state & 02) >> 01;
	*run = (motor_state & 04) >> 02;
	#endif
	return;
}
