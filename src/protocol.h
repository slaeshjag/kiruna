#ifndef __PROTOCOL_H__
#define	__PROTOCOL_H__


#ifdef MOTHERSHIP
#define	PROTOCOL_MAX_TIMESLICE  	8000
#else
#define	PROTOCOL_MAX_TIMESLICE		4
#endif
#define	PROTOCOL_PACKET_SIZE		16
#define	PROTOCOL_MOTOR_KILL_DELAY	3


enum protocol_command {
	PROTOCOL_CMD_MIC	= 0,
	PROTOCOL_CMD_SPEAKER	= 1,
	PROTOCOL_CMD_CAMERA	= 2,
	PROTOCOL_CMD_NACK	= 6,
	PROTOCOL_CMD_SYNC	= 7,
};


enum protocol_state {
	PROTOCOL_STATE_SLAVE_WAIT,
	PROTOCOL_STATE_MASTER_WAIT,
	PROTOCOL_STATE_SLAVE_SEND_MIC,
	PROTOCOL_STATE_SLAVE_GET_SPEAK,
	PROTOCOL_STATE_MASTER_GET_MIC,
	PROTOCOL_STATE_MASTER_SEND_SPEAK,
	PROTOCOL_STATE_SLAVE_SEND_CAMERA,
	PROTOCOL_STATE_MASTER_GET_CAMERA,
};


struct protocol_cmd_header {
	unsigned int		cmd		: 3;
	unsigned int		length		: 4;
	unsigned int		go_direction	: 3;
	/* 17 more bits to fill */
};


void protocol_init();
void protocol_loop();
void protocol_get_motor(int *left, int *right, int *run);


#endif
