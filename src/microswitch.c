#include "system/LPC11xx.h"

#define MS_PORT		LPC_GPIO1
#define MS_LEFT		4
#define MS_RIGHT	5


void ms_init(void)
{
	MS_PORT->DIR	&= ~(1<<MS_LEFT);	//set PIO1_4 to input
	MS_PORT->DIR	&= ~(1<<MS_RIGHT);	//set PIO1_5 to input
}

int ms_left_pressed(void)
{
	return (MS_PORT->DATA & (1<<MS_LEFT));
}

int ms_right_pressed(void)
{
	return (MS_PORT->DATA & (1<<MS_RIGHT));
}
