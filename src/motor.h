#ifndef __MOTOR_H__
#define	__MOTOR_H__

#define	MOTOR_PORT		LPC_GPIO1
#define	MOTOR_MASK		(1<<0)|(1<<2)	// 0b00000000.0101

#define	MOTOR_LEFT_FORWARD	0x001	// 0b00000000.0001
#define	MOTOR_LEFT_BACKWARD	0x000	// 0b00000000.0000
#define	MOTOR_RIGHT_FORWARD	0x004	// 0b00000000.0100
#define	MOTOR_RIGHT_BACKWARD	0x000	// 0b00000000.0000

#define MOTOR_TIME_90		100000
#define	MOTOR_TIME_45		 50000

#define MOTOR_TIME_SHORT	1000000
#define MOTOR_TIME_LONG		10000000

#define	PWM_CYCLE_LENGTH	48000



enum motor_direction {
	MOTOR_DIR_FORWARD,
	MOTOR_DIR_LEFT,
	MOTOR_DIR_RIGHT,
	MOTOR_DIR_BACKWARD,
	MOTOR_DIR_STAHP,
};

void motor_init(void);
void motor_go(enum motor_direction dir, unsigned int speed);

#endif
