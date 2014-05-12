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

void us_handler(void)			// This is run from the systick interrupter
{
	if (us_state == STATE_COUNTING)	// when 1 we want to read the pulse (if there is one anymore)
	{
		if (US_PORT->DATA & (1<<US_ECHO)) us_counter++;	// measuring length of pulse
		else us_state = STATE_DONE;			// here pulse has ended
	}
}

int us_read_nonblock() {
	if(us_state != STATE_DONE) return -1;
	
	us_state = STATE_NONE;
	return us_counter;
}


