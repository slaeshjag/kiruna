#ifndef __MOTOR_H__
#define	__MOTOR_H__

#define	MOTOR_MASK		0xF00

#define	MOTOR_LEFT_FORWARD	0x200
#define	MOTOR_LEFT_BACKWARD	0x100
#define	MOTOR_RIGHT_FORWARD	0x400
#define	MOTOR_RIGHT_BACKWARD	0x800

#define	MOTOR_PORT		LPC_GPIO2


enum motor_direction {
	MOTOR_DIR_FORWARD,
	MOTOR_DIR_LEFT,
	MOTOR_DIR_RIGHT,
	MOTOR_DIR_BACKWARD,
	MOTOR_DIR_STAHP,
};

void motor_go(enum motor_direction dir);

#endif
