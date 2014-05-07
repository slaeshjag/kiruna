#include "system/LPC11xx.h"
#include "motor.h"

#define	MOTOR_REGISTER	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK]	// Masked changes only the bits specified in brackets

void motor_init(void)
{
	LPC_IOCON->R_PIO1_0	|= 0x1;		// Selects function PIO1_0 (7.4.29)
	LPC_IOCON->R_PIO1_2	|= 0x1;		// Selects function PIO1_2 (7.4.31)

	LPC_GPIO1->DIR				|= (MOTOR_MASK);	// Set direction to output for PIO1_0 and PIO1_2 (12.3.2)	
	LPC_GPIO1->MASKED_ACCESS[MOTOR_MASK]	|= (MOTOR_MASK);	// Set PIO1_0 and PIO1_2 to 1
	
	pwm_init();
}

void pwm_init(void)
{
	LPC_IOCON->R_PIO1_1		= 0x3|(1<<7);		//CT32B1_MAT0, Digital functional mode (7.4.30)
	LPC_IOCON->SWDIO_PIO1_3		= 0x3|(1<<7);		//CT32B1_MAT2, Digital functional mode (7.4.35)
	
	LPC_SYSCON->SYSAHBCLKCTRL	|= (1<<10);		//enable clock signal for 32 bit timer1 (3.5.14)
	LPC_TMR32B1->PR			= 0x0;			//set prescaler max value, not used here (20.7.4)
	LPC_TMR32B1->CCR		= 0;			//Timer Mode: every rising PCLK edge (20.7.11)

	LPC_TMR32B1->MCR		= (1<<10);		//TC (only) reset on match for MR3. No interrupts. (20.7.6)
	LPC_TMR32B1->MR3		= PWM_CYCLE_LENGTH;	//set value for period (20.7.7)

	LPC_TMR32B1->PWMC		= (1<<0)|(1<<2);	//PWM mode is enabled for CT32Bn_MAT0 &  CT32Bn_MAT2 (20.7.12)
	LPC_TMR32B1->EMR		|= (1<<5)|(1<<9);	//Set output high on match for EM0 & EM2 (20.7.10)
	LPC_TMR32B1->MR0		= 0;			//Set PWM to run at 100% high (20.7.7)
	LPC_TMR32B1->MR2		= 0;			//Set PWM to run at 100% high (20.7.7)

	LPC_TMR32B1->TCR		|= 0x3;			//enable and reset counter (20.7.2)
	LPC_TMR32B1->TCR		&= ~(0x2);		//clear reset bit (sec 20.7.2)
}

void set_pulsewidth(unsigned int l_speed, unsigned int r_speed)
{
	LPC_TMR32B1->MR0		= (PWM_CYCLE_LENGTH * (100-l_speed))/100;	//Set PWM for LEFT motor
	LPC_TMR32B1->MR2		= (PWM_CYCLE_LENGTH * (100-r_speed))/100;	//Set PWM for RIGHT motor
}

void motor_go(enum motor_direction dir, unsigned int speed)
{
	int l_speed = speed-30;			// Adjusting that our motors are not equal
	if(l_speed < 0) l_speed = 0;		// If we remove more than we are allowed to
	if (speed>100)	speed = 100;		// Cannot be over 100% obviously
	
	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK] &= (~MOTOR_MASK);
	switch (dir) {
		case MOTOR_DIR_FORWARD:
			set_pulsewidth((100-l_speed), (100-speed));
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_LEFT:
			set_pulsewidth(l_speed, (100-speed));
			MOTOR_REGISTER |= MOTOR_LEFT_BACKWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_RIGHT:
			set_pulsewidth((100-l_speed), speed);
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_BACKWARD;
			break;
		case MOTOR_DIR_BACKWARD:
			set_pulsewidth(l_speed, speed);
			MOTOR_REGISTER |= MOTOR_LEFT_BACKWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_BACKWARD;
			break;
		default:
			set_pulsewidth(100, 100);
			MOTOR_REGISTER |= (MOTOR_MASK);	// Both sides of H-bridge is turned on. No dif = no rotation
			break;
	}
	return;
}
