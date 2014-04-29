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
	
	// 32bit B1
	//Set up 16 bit timer for PWM operation
	/*LPC_IOCON->PIO0_8		= 0x3|(1<<7);	//Selects function CT32B1_MAT0, Digital functional mode (7.4.30)
	LPC_SYSCON->SYSAHBCLKCTRL	|= (1<<10);	//enable clock signal to 16 bit timer0 (3.5.14)
	LPC_TMR16B0->PR			= 0x0;		//set prescaler max value, not used here (18.7.4)
	LPC_TMR16B0->MCR		= 0x10;		//set for reset on counter match (sec 18.7.6)
	LPC_TMR16B0->EMR		|= 0x20;	//set pin 27 to 1 on match (sec 18.7.10)
	LPC_TMR16B0->CCR		= 0;		//set to timer mode (sec 18.7.11)
	LPC_TMR16B0->PWMC		= 0x1;		//set channel zero to PWM mode (sec 18.7.12)
	LPC_TMR16B0->MR1		= 0x32;		//set value for period (sec 18.7.7)
	LPC_TMR16B0->MR0		= 0xC;		//set value for duty cycle (sec 18.7.7)
	LPC_TMR16B0->TCR		|= 0x3;		//enable and reset counter (sec 18.7.2)
	LPC_TMR16B0->TCR		&= ~(0x2);	//clear reset bit (sec 18.7.2)*/
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

