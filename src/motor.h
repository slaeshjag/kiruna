#ifndef __MOTOR_H__
#define	__MOTOR_H__

#define	MOTOR_PORT		LPC_GPIO1
#define	MOTOR_MASK		0x00F	// 0b00000000.1111	set PIO1 pin 0,1,2,3 as output

#define	MOTOR_LEFT_FORWARD	0x001	// 0b00000000.0001
#define	MOTOR_LEFT_BACKWARD	0x002	// 0b00000000.0010
#define	MOTOR_RIGHT_FORWARD	0x004	// 0b00000000.0100
#define	MOTOR_RIGHT_BACKWARD	0x008	// 0b00000000.1000

#define MOTOR_TIME_90		100000
#define	MOTOR_TIME_45		 50000

#define MOTOR_TIME_SHORT	100000
#define MOTOR_TIME_LONG		10000000



enum motor_direction {
	MOTOR_DIR_FORWARD,
	MOTOR_DIR_LEFT,
	MOTOR_DIR_RIGHT,
	MOTOR_DIR_BACKWARD,
	MOTOR_DIR_STAHP,
};

void motor_init(void);
void motor_go(enum motor_direction dir);

#endif
