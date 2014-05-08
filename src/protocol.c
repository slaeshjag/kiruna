#include <stdint.h>
#include "protocol.h"
#include "radiolink.h"
#include "uart.h"
#include "main.h"

static enum protocol_state state;

void protocol_init() {
	#ifdef MOTHERSHIP
		state = PROTOCOL_STATE_SLAVE_WAIT;
	#else
		state = PROTOCOL_STATE_MASTER_WAIT;
	#endif
}


void protocol_send_cmd(enum protocol_command cmd, int len) {


}


void protocol_loop() {
	unsigned char cmd_packet[16];
	struct protocol_cmd_header *cmd = (void *) cmd_packet;
	int last_timer;
	static int length;
	last_timer = global_timer;

	/* State-maskinen Gösta */
	for (;;) {
		switch (state) {
			case PROTOCOL_STATE_SLAVE_WAIT:
				/* TODO: Enter recv mode */
				radiolink_recv(16, cmd_packet);
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
						uart_printf("Got invalid packet CMD_SYNC\n");
						break;
				}

				/* TODO: Wait for new command */
				break;
			case PROTOCOL_STATE_MASTER_WAIT:
				/* TODO: Enter send mode */
				/* TODO: Read a packet of data from UART */
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

				radiolink_send(PROTOCOL_PACKET_SIZE, cmd_packet);
				break;
			case PROTOCOL_STATE_SLAVE_GET_SPEAK:
				/* TODO: Get speaker data */
				break;
			case PROTOCOL_STATE_SLAVE_SEND_MIC:
				/* TODO: Send mic data */
				break;
			default:
				/* TODO: Handle */
				break;
		}

		if (global_timer - last_timer >= PROTOCOL_MAX_TIMESLICE)
			return;
	}
}
