#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"

#define US_TRIG		2		// PIO0_2
#define US_ECHO		3		// PIO0_3
#define US_PORT		LPC_GPIO0


void us_init(void)
{
	US_PORT->DIR	|= (1<<US_TRIG);	//set PIO0_2 to output
	US_PORT->DIR	&= ~(1<<US_ECHO);	//set PIO0_3 to input
}

void us_trig(void)		// Ultrasonic trigger
{
	US_PORT->DATA		|= (1<<US_TRIG);	//set PIO0_2 high
	util_delay(15);					//"sleep" for 15 us (Pulse cannot be <10us!)
	US_PORT->DATA		&= ~(1<<US_TRIG);	//set PIO0_2 low
}

int us_read(void)	// blocking
{
	int pulseWidth = 0;
	
	while ((US_PORT->DATA & (1<<US_ECHO)) == 0);		//while pulse is travelling
	
	while ((US_PORT->DATA & (1<<US_ECHO))) pulseWidth++;	//measuring the distance of pulse
		
	return pulseWidth;
}


int us(void)		// not used
{
	us_trig();
	return us_read();
}

