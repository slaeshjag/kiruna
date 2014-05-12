#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"
#include "motor.h"
#include "ultrasonic.h"
#include "microswitch.h"
#include "ov7670.h"
#include "protocol.h"


#define	MOTOR_REGISTER	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK]	// Masked changes only the bits specified in brackets

#define STOPPING_DISTANCE	20
#define PAUSING_DISTANCE	5


void pwm_init(void);
void motor_go(enum motor_direction dir, unsigned int speed);


void motor_init(void)
{
	LPC_IOCON->R_PIO1_0	|= 0x1;		// Selects function PIO1_0 (7.4.29)
	LPC_IOCON->R_PIO1_2	|= 0x1;		// Selects function PIO1_2 (7.4.31)

	LPC_GPIO1->DIR				|= (MOTOR_MASK);	// Set direction to output for PIO1_0 and PIO1_2 (12.3.2)	
	LPC_GPIO1->MASKED_ACCESS[MOTOR_MASK]	|= (MOTOR_MASK);	// Set PIO1_0 and PIO1_2 to 1
	
	pwm_init();
	motor_go(MOTOR_DIR_STAHP, 100);
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
	if (speed>100)	speed = 100;		// Cannot be over 100% obviously
	
	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK] &= (~MOTOR_MASK);
	switch (dir) {
		case MOTOR_DIR_FORWARD:
			set_pulsewidth((100-speed), (100-speed));
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_LEFT:
			set_pulsewidth(speed, (100-speed));
			MOTOR_REGISTER &= (~MOTOR_LEFT_FORWARD);
			MOTOR_REGISTER |= MOTOR_RIGHT_FORWARD;
			break;
		case MOTOR_DIR_RIGHT:
			set_pulsewidth((100-speed), speed);
			MOTOR_REGISTER |= MOTOR_LEFT_FORWARD;
			MOTOR_REGISTER &= (~MOTOR_RIGHT_FORWARD);
			break;
		case MOTOR_DIR_BACKWARD:
			set_pulsewidth(speed, speed);
			MOTOR_REGISTER &= (~MOTOR_LEFT_FORWARD);
			MOTOR_REGISTER &= (~MOTOR_RIGHT_FORWARD);
			break;
		default:
			set_pulsewidth(100, 100);
			MOTOR_REGISTER |= (MOTOR_MASK);	// Both sides of H-bridge is turned on. No dif = no rotation
			break;
	}
	return;
}

	int value;
	int is_paused = 1;
	int normalize_stop_counter = 0;
	int normalize_stop_loop = 0;
	int is_stopped = 0;
	int normalize_start_counter = 0;
	int normalize_start_loop = 0;
	
	int radio_motor_left;
	int radio_motor_right;
	int radio_motor_speed;

void motor_logic(void)
{
	value = us_read_nonblock();
	//if (value < 0) return;
	
	protocol_get_motor(&radio_motor_left, &radio_motor_right, &radio_motor_speed);	// Getting radio commands
	
	radio_motor_speed *= 100;	// Because received value is either 0 or 1 (for now), not in percentage.
	
	if (is_paused)
	{
		if (ms_left_pressed() || ms_right_pressed())
		{
			uart_printf("is paused and left/right is pressed - unpausing!\n");
			util_delay(MOTOR_TIME_SHORT);
			is_paused = 0;
			normalize_stop_loop = 0;	// if pause was done while we were normalizing, reset values
			normalize_start_counter = 0;
		}
		else if(radio_motor_speed)	// We've received some form of command via radio to go forward, so un-pause
		{
			uart_printf("is paused and radio sent moving command - unpausing!\n");
			is_paused = 0;
			normalize_stop_loop = 0;	// if pause was done while we were normalizing, reset values
			normalize_start_counter = 0;
		}
	}
	/****************************/
	else if(ms_left_pressed())
	{
		uart_printf("Left pressed, pausing!\n");
			
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_SHORT);
		is_paused = 1;
		
		
		/*motor_go(MOTOR_DIR_LEFT, 100);
		util_delay(MOTOR_TIME_SHORT);

		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_SHORT);


		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_LEFT, 100);
		util_delay(MOTOR_TIME_45);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_BACKWARD, 100);
		util_delay(MOTOR_TIME_SHORT);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_RIGHT, 100);
		util_delay(MOTOR_TIME_45);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_FORWARD, 100);*/
	}
	else if(ms_right_pressed())
	{
		uart_printf("Right pressed, pausing!\n");
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_SHORT);
		is_paused = 1;
		
		
		/*motor_go(MOTOR_DIR_RIGHT, 100);
		util_delay(MOTOR_TIME_SHORT);

		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_SHORT);
		
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_RIGHT, 100);
		util_delay(MOTOR_TIME_45);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_BACKWARD, 100);
		util_delay(MOTOR_TIME_SHORT);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_LEFT, 100);
		util_delay(MOTOR_TIME_45);
		
		motor_go(MOTOR_DIR_STAHP, 100);
		util_delay(MOTOR_TIME_LONG);
		
		motor_go(MOTOR_DIR_FORWARD, 100);*/
	}
	else if (!radio_motor_speed)	// If radio sends stop, or radiolink is dropped value is 0
	{
		uart_printf("Radiolink 'sends' stop, stopping!\n");
		
		motor_go(MOTOR_DIR_STAHP, 100);
		is_paused = 1;
	}
	/****************************/
	else if (normalize_stop_loop > 0)	// we are here checking US values multiple times to ensure we need to stop
	{
		if (value < STOPPING_DISTANCE) normalize_stop_counter++;
		
		if (normalize_stop_loop == 1)
		{
			if (normalize_stop_counter > 2)
			{
				uart_printf("Under 20 3/5 times, stopping!\n");
				motor_go(MOTOR_DIR_STAHP, 100);
				is_stopped = 1;
			}
			normalize_stop_loop = 0;
			normalize_stop_counter = 0;
		}
		else normalize_stop_loop--;
	}
	else if (value < STOPPING_DISTANCE)	// to ensure its not just >one< bogus value, we check multiple times
	{
		normalize_stop_loop = 5;
		uart_printf("Under 20, checking for more bogus values.\n");
	}
	/****************************/
	else if (normalize_start_loop > 0)
	{
		normalize_start_counter++;
		
		if (normalize_start_loop == 1)
		{
			if (normalize_start_counter > 2)
			{
				uart_printf("Over 20 3/5 times, starting!\n");
				motor_go(MOTOR_DIR_FORWARD, 100);
				is_stopped = 0;
			}
			normalize_start_loop = 0;
			normalize_start_counter = 0;
		}
		else normalize_start_loop--;
	}
	else if (is_stopped)	// if is stopped and over stopping distance
	{
		normalize_start_loop = 5;
		uart_printf("Over 20 and stopped, checking for more bogus values.\n");

	}
	/****************************/
	else if (radio_motor_left && radio_motor_right)
	{
		uart_printf("Received going forward from radio\n");
		motor_go(MOTOR_DIR_FORWARD, radio_motor_speed);
	}
	else if (radio_motor_left)
	{
		uart_printf("Received turning left from radio\n");
		motor_go(MOTOR_DIR_LEFT, radio_motor_speed);
	}
	else if (radio_motor_right)
	{
		uart_printf("Received going forward from radio\n");
		motor_go(MOTOR_DIR_RIGHT, radio_motor_speed);
	}
	else
	{
		uart_printf("Received going backward from radio\n");
		motor_go(MOTOR_DIR_BACKWARD, radio_motor_speed);
	}
	
	us_trig();
}
