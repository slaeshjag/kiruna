#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"
#include "motor.h"
#include "ultrasonic.h"
#include "microswitch.h"
#include "i2c.h"

#define	MOTOR_REGISTER	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK]	// Masked changes only the bits specified in brackets

#define STOPPING_DISTANCE	15000


void pwm_init(void);


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

void motor_logic(void)
{
	int us_time;
	int is_paused = 1;
	int normalize_stop_counter = 0;
	int normalize_stop_loop = 0;
	int is_stopped = 0;
	int normalize_start_counter = 0;
	int normalize_start_loop = 0;
	
	motor_go(MOTOR_DIR_STAHP, 100);
	
	while(1)
	{
		us_time = us();
		uart_printf("US is %i\n", us_time);
		
		if (is_paused)	// MÅSTE VARA FÖRST
		{
			if (us_time < 2000)
			{
				is_paused = 0;
				util_delay(MOTOR_TIME_SHORT);	// Time to remove "hand", so it doesn't pause quickly again
				motor_go(MOTOR_DIR_FORWARD, 100);
			}
		}
		else if (us_time < 2000)	// "Hand" appears, pause
		{
			//uart_printf("Under 2000, pausas!\n");
			is_paused = 1;
			motor_go(MOTOR_DIR_STAHP, 100);
			util_delay(MOTOR_TIME_SHORT);	// Time to remove "hand", so it doesn't unpause
			
			normalize_stop_counter = 0;	// we clear our normalizer in case it was counting before we paused
			normalize_stop_loop = 0;
		}
		
		else if(ms_left_pressed())
		{
			uart_printf("Vänster intryckt!\n");
			
			motor_go(MOTOR_DIR_STAHP, 100);
			util_delay(MOTOR_TIME_SHORT);
			
			
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
			uart_printf("Höger intryckt!\n");
			
			motor_go(MOTOR_DIR_STAHP, 100);
			util_delay(MOTOR_TIME_SHORT);
			
			
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

		else if (normalize_stop_loop > 0)	// we are here checking multiple times to ensure we need to stop
		{
			if (us_time < STOPPING_DISTANCE) normalize_stop_counter++;
			
			if (normalize_stop_loop == 1)
			{
				if (normalize_stop_counter > 2)
				{
					//uart_printf("Under 20 000 3/5 times, stopping!\n");
					motor_go(MOTOR_DIR_STAHP, 100);
					is_stopped = 1;
				}
				normalize_stop_loop = 0;
				normalize_stop_counter = 0;
			}
			else normalize_stop_loop--;
		}
		else if (us_time < STOPPING_DISTANCE)	// to ensure its not just >one< bogus value, we check multiple times
		{
			normalize_stop_loop = 5;
			//uart_printf("Under 20000, checking for more bogus values.\n");
			//util_delay(MOTOR_TIME_SHORT);	// only to catch the uart_printf, should be removed
		}
		else if (normalize_start_loop > 0)
		{
			normalize_start_counter++;
			
			if (normalize_start_loop == 1)
			{
				if (normalize_start_counter > 5)
				{
					//uart_printf("Over 20 000 5/10 times, starting!\n");
					motor_go(MOTOR_DIR_FORWARD, 100);
					is_stopped = 0;
				}
				normalize_start_loop = 0;
				normalize_start_counter = 0;
			}
			else normalize_start_loop--;
		}
		else if (is_stopped)
		{
			normalize_start_loop = 10;	// is stopped and over stopping distance
			//uart_printf("Over 20 000 and stopped, checking for more bogus values.\n");
			//util_delay(MOTOR_TIME_SHORT);	// only to catch the uart_printf, should be removed

		}
		else motor_go(MOTOR_DIR_FORWARD, 100);
	}
	
}
