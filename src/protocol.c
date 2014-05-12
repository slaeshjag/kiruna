#include <stdint.h>
#include "protocol.h"
#include "radiolink.h"
#include "uart.h"
#include "main.h"
#include "audio.h"
#include "uart_buffer.h"
#include "protocol.h"

static enum protocol_state state;

#ifdef MOTHERSHIP
static struct {
	int		motor_state;
	int		no_update_count;
} motor_state;
#endif

void protocol_init() {
	#ifdef MOTHERSHIP
		state = PROTOCOL_STATE_SLAVE_WAIT;
	#else
		state = PROTOCOL_STATE_MASTER_WAIT;
	#endif
}


void protocol_send_sync(unsigned char *buff) {
	int i;

	for (i = 0; i < PROTOCOL_PACKET_SIZE - 1; i++)
		buff[i] = 0xFF;
	buff[i] = 0;
	radiolink_send_stubborn(PROTOCOL_PACKET_SIZE, buff, PROTOCOL_MAX_TIMESLICE / 2);
}


int protocol_is_sync(unsigned char *data) {
	return (*data == 0xFF && data[1] == 0xFF) ? 1 : 0;
}


void protocol_get_motor(int *left, int *right, int *run) {
	#ifdef MOTHERSHIP
	*left = (motor_state.motor_state & 01);
	*right = (motor_state.motor_state & 02) >> 01;
	*run = (motor_state.motor_state & 04) >> 02;
	#endif
	return;
}


void protocol_loop() {
	unsigned char cmd_packet[16];
	struct protocol_cmd_header *cmd = (void *) cmd_packet;
	int last_timer;
	static int len = 0, resume = 0;
	last_timer = global_timer;
	
	#ifdef MOTHERSHIP
	if (++motor_state.no_update_count >= PROTOCOL_MOTOR_KILL_DELAY)
		motor_state.motor_state = 0;
	#endif

	/* State-maskinen Gösta */
	for (;;) {
		uart_printf("In state %i %i\n", state, len);
		switch (state) {
			case PROTOCOL_STATE_SLAVE_WAIT:
				
				if (!radiolink_recv_timeout(16, cmd_packet, PROTOCOL_MAX_TIMESLICE - (global_timer - last_timer)))
					return;

				#ifdef MOTHERSHIP
				motor_state.motor_state = cmd->go_direction;
				motor_state.no_update_count = 0;
				#endif

				len = cmd->length;
				switch (cmd->cmd) {
					case PROTOCOL_CMD_MIC:
						state = PROTOCOL_STATE_SLAVE_SEND_MIC;
						break;
					case PROTOCOL_CMD_SPEAKER:
						state = PROTOCOL_STATE_SLAVE_GET_SPEAK;
						break;
					case PROTOCOL_CMD_CAMERA:
						state = PROTOCOL_STATE_SLAVE_SEND_CAMERA;
						break;
					case PROTOCOL_CMD_SYNC:
						break;
				}

				break;
			case PROTOCOL_STATE_MASTER_WAIT:
				if (!resume)
					uart_get_data(cmd_packet, 16);
				len = cmd->length;
				switch (cmd->cmd) {
					case PROTOCOL_CMD_MIC:
						state = PROTOCOL_STATE_MASTER_GET_MIC;
						break;
					case PROTOCOL_CMD_SPEAKER:
						state = PROTOCOL_STATE_MASTER_SEND_SPEAK;
						break;
					case PROTOCOL_CMD_CAMERA:
						state = PROTOCOL_STATE_MASTER_GET_CAMERA;
						break;
					default:
						break;
				}

				if (!radiolink_send_stubborn(PROTOCOL_PACKET_SIZE, cmd_packet, PROTOCOL_MAX_TIMESLICE - (global_timer - last_timer))) {
					resume = 1;
					return;
				} else
					resume = 0;
					
				break;
			case PROTOCOL_STATE_SLAVE_GET_SPEAK:
				if (!radiolink_recv_timeout(PROTOCOL_PACKET_SIZE, cmd_packet, PROTOCOL_MAX_TIMESLICE - (global_timer - last_timer)))
					return;
				len--;
				if (protocol_is_sync(cmd_packet)) {
					state = PROTOCOL_STATE_SLAVE_WAIT;
					break;
				}
				#ifdef MOTHERSHIP
				speaker_fill(cmd_packet, PROTOCOL_PACKET_SIZE);
				#endif
				if (!len)
					state = PROTOCOL_STATE_SLAVE_WAIT;
				break;
			case PROTOCOL_STATE_SLAVE_SEND_MIC:
				#ifdef MOTHERSHIP
				if (!microphone_send())
					break;
				#endif
				len--;
				if (!len)
					state = PROTOCOL_STATE_SLAVE_WAIT;
				protocol_send_sync(cmd_packet);
				break;
			case PROTOCOL_STATE_MASTER_GET_MIC:
				if (!radiolink_recv_timeout(PROTOCOL_PACKET_SIZE, cmd_packet, PROTOCOL_MAX_TIMESLICE - (global_timer - last_timer)))
					return;
				if (protocol_is_sync(cmd_packet))
					state = PROTOCOL_STATE_MASTER_WAIT;
				len--;
				if (!len)
					state = PROTOCOL_STATE_MASTER_WAIT;
				uart_send_raw(cmd_packet, PROTOCOL_PACKET_SIZE);
				break;
			case PROTOCOL_STATE_MASTER_SEND_SPEAK:
				uart_get_data(cmd_packet, PROTOCOL_PACKET_SIZE);
				radiolink_send_unreliable(PROTOCOL_PACKET_SIZE, cmd_packet);
				len--;
				if (!len)
					state = PROTOCOL_STATE_MASTER_WAIT;
				break;
			case PROTOCOL_STATE_SLAVE_SEND_CAMERA:
				ov7670_get_data_packet(cmd_packet);
				radiolink_send_unreliable(PROTOCOL_PACKET_SIZE, cmd_packet);
				len--;
				if (!len)
					state = PROTOCOL_STATE_SLAVE_WAIT;
				break;
			case PROTOCOL_STATE_MASTER_GET_CAMERA:
				if (!radiolink_recv_timeout(PROTOCOL_PACKET_SIZE, cmd_packet, PROTOCOL_MAX_TIMESLICE - (global_timer - last_timer)))
					break;
				len--;
				if (!len)
					state = PROTOCOL_STATE_MASTER_WAIT;
				break;
			default:
				uart_printf("Unhandled state %i\n", state);
				/* TODO: Handle */
				break;
		}

		if (global_timer - last_timer >= PROTOCOL_MAX_TIMESLICE)
			return;
	}
}

#include "motor.h"
void protocol_fulhakk() {
	unsigned char buf[16];
	
	for(;;) {
		radiolink_recv(16, buf);
		uart_printf("got shit %s\n", buf);
		switch(buf[0]) {
			case 'w':
				motor_go(MOTOR_DIR_FORWARD, 100);
				break;
			case 'a':
				motor_go(MOTOR_DIR_LEFT, 100);
				break;
			case 's':
				motor_go(MOTOR_DIR_BACKWARD, 100);
				break;
			case 'd':
				motor_go(MOTOR_DIR_RIGHT, 100);
				break;
			case 'x':
				motor_go(MOTOR_DIR_STAHP, 100);
				break;
		}
	}
}

void protocol_fulhakk_computer() {
	unsigned char buf[16] = {0};
	
	for(;;) {
		buf[0] = uart_recv_char();
		uart_printf("lol\n");
		radiolink_send(16, buf);
	}
}
