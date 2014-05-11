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

enum {
	STATE_NONE,
	STATE_COUNTING,
	STATE_DONE,
} us_state = STATE_NONE;

int us_counter = 0;
int us_hasUnreadValue = 0;
int us_value = 0;


void us_trig(void)		// Ultrasonic trigger, BLOCKING
{
	US_PORT->DATA		|= (1<<US_TRIG);	//set PIO0_2 high
	util_delay(15);					//"sleep" for 15 us (Pulse cannot be <10us!)
	US_PORT->DATA		&= ~(1<<US_TRIG);	//set PIO0_2 low
	
	while ((US_PORT->DATA & (1<<US_ECHO)) == 0);	//while pulse is travelling
	us_counter = 0;					// pulse is incoming, so we add 1
	us_state = STATE_COUNTING;
}

int us_read(void)	// blocking, not used
{
	int pulseWidth = 0;
	
	while ((US_PORT->DATA & (1<<US_ECHO)) == 0);		//while pulse is travelling
	
	while ((US_PORT->DATA & (1<<US_ECHO))) pulseWidth++;	//measuring the distance of pulse
		
	return pulseWidth;
}


int us(void)		// not used (anymore)
{
	us_trig();
	return us_read();
}


void us_handler(void)
{
	if (us_state == STATE_COUNTING)	// when 1 we want to read the pulse (if there is one anymore)
	{
		if (US_PORT->DATA & (1<<US_ECHO)) us_counter++;	// measuring length of pulse
		else {						// here pulse has ended
			us_state = STATE_DONE;
		}
	}
}

int us_read_nonblock() {
	if(us_state != STATE_DONE)
		return -1;
	
	us_state = STATE_NONE;
	
	return us_counter;
}

int us_read_block()
{
	us_trig();
	uart_printf("Done trigging\n");
	
	util_delay(10000);
	
	
	while (!us_hasUnreadValue);
	
	uart_printf("Value available\n");
	
	int temp = us_value;
	us_hasUnreadValue = 0;	// when we read the value we trig it to get the coming one
	
	return temp;
}


