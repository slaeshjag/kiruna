#include "system/LPC11xx.h"
#include "motor.h"

#define	MOTOR_REGISTER	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK]	// Masked changes only the bits specified in brackets

void motor_init(void)
{
	LPC_IOCON->R_PIO1_0 |= 0x1;
	LPC_IOCON->R_PIO1_1 |= 0x1;
	LPC_IOCON->R_PIO1_2 |= 0x1;
	LPC_IOCON->SWDIO_PIO1_3 |= 0x1;
	MOTOR_PORT->DIR |= MOTOR_MASK;
	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK] |= (MOTOR_MASK);
}

void motor_go(enum motor_direction dir) {
	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK] &= (~MOTOR_MASK);
	switch (dir) {
		case MOTOR_DIR_FORWARD:
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_LEFT:
			MOTOR_REGISTER |= MOTOR_LEFT_BACKWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_RIGHT:
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_BACKWARD;
			break;
		case MOTOR_DIR_BACKWARD:
			MOTOR_REGISTER |= MOTOR_LEFT_BACKWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_BACKWARD;
			break;
		default:
			MOTOR_REGISTER |= (MOTOR_MASK);	// Both sides of H-bridge is turned on. No dif = no rotation
			break;
	}
	return;
}

