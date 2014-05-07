#include <stdint.h>
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include "spi.h"
#include "radiolink.h"
#include "motor.h"
#include "audio.h"
#include "ultrasonic.h"
#include "microswitch.h"

#define STOPPING_DISTANCE	15000



void initialize(void) {
	unsigned int regval;
	/* TODO: Set CPU clock etc. */
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);
	LPC_SYSCON->SYSAHBCLKDIV = 0x1;
	LPC_SYSCON->PDRUNCFG &= ~0x80;
	LPC_SYSCON->SYSPLLCTRL = 0x23;
	LPC_SYSCON->SYSPLLCLKSEL = 0x0;
	LPC_SYSCON->SYSPLLCLKUEN = 0x0;
	LPC_SYSCON->SYSPLLCLKUEN = 0x1;
	while(!(LPC_SYSCON->SYSPLLSTAT & 1));
	LPC_SYSCON->MAINCLKSEL = 0x3;
	LPC_SYSCON->MAINCLKUEN = 0x1;
	
	/*LPC_IOCON->PIO0_1 &= ~0x7;
	LPC_IOCON->PIO0_1 |= 0x1;
	LPC_SYSCON->CLKOUTDIV = 1;
	LPC_SYSCON->CLKOUTCLKSEL = 0x3;
	LPC_SYSCON->CLKOUTUEN = 0x1;*/
	
	/*********** Enable UART0 **********/
	
	LPC_SYSCON->UARTCLKDIV = 1;
	/* Enable RXD, TXD on the IO pins */
	LPC_IOCON->PIO1_6 &= ~0x7;
	LPC_IOCON->PIO1_6 |= 1;
	LPC_IOCON->PIO1_7 &= ~0x7;
	LPC_IOCON->PIO1_7 |= 1;

	/* Enable UART clock bit */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);
	/* Set up FIFO */
	LPC_UART->FCR = 0x7;
	/* Set line control (stop bits etc.) */
	LPC_UART->LCR = 0x83;
	regval = ((SYSTEM_CLOCK/LPC_SYSCON->SYSAHBCLKDIV)/16/UART_BAUD_RATE);
	LPC_UART->FDR = 0x10;
	LPC_UART->DLL = regval & 0xFF;
	LPC_UART->DLM = (regval >> 8) & 0xFF;
	LPC_UART->LCR = 0x3;
	
	/********* Enable SPI0 ************/
	LPC_IOCON->SCK_LOC  = 0x0;
	//LPC_IOCON->PIO0_6 &= ~0x7;
	LPC_IOCON->SWCLK_PIO0_10 = 0x2;
	LPC_IOCON->PIO0_8 &= ~0x7;
	LPC_IOCON->PIO0_8 |= 0x1;
	LPC_IOCON->PIO0_9 &= ~0x7;
	LPC_IOCON->PIO0_9 |= 0x1;
	//LPC_IOCON->SCK_LOC  = 2;
	//LPC_SYSCON->PRESETCTRL |= 0x5;
	//LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 11);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);
	LPC_SYSCON->SSP0CLKDIV = 4;
	LPC_SYSCON->PRESETCTRL |= 0x1;
	
	LPC_SSP0->CPSR = 0x2;
	LPC_SSP0->CR0 = 0x107;
	LPC_SSP0->CR1 = 0x2;
	
	/*Enable ADC*/
	LPC_IOCON->R_PIO0_11 = 0x2;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
	/*48 MHz / 12 = 4 MHz*/
	LPC_ADC->INTEN = 0;
	LPC_SYSCON->PDRUNCFG &= ~(0x1 << 4);
	LPC_ADC->CR = 0x1 | (12 << 8) | (1 << 24);
	
	/*Enable DAC*/
	LPC_GPIO0->DIR |= 0x80;
	
	/*Disable systick*/
	SysTick->CTRL = 0;
	
	/* Enable clock for tmr16B1 (used by util_delay) */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);
}

void systick_irq() {
	microphone_sample();
	/*LPC_GPIO0->MASKED_ACCESS[0x80] = 0x0;
	spi_send_recv(~data);
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x80;*/
	//data += 5;
}

void systick_enable() {
	/* Trig 8000 times per second */
	SysTick->LOAD = SYSTEM_CLOCK / 8000;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x1 | 0x2 | 0x4;
}

int main(int ram, char **argv) {
	int i;
	
	initialize();
	uart_printf("initialize() done\n");
	motor_init();
	uart_printf("motor_init() done\n");
	us_init();
	uart_printf("us_init() done\n");
	ms_init();
	uart_printf("ms_init() done\n");
	ov7670_init();
	uart_printf("i2c_init() done\n");
	
	util_delay(1000000);
	
	uart_printf("AutoKorg™ 0980 READY TO WRECK SOME HAVOC!\n");

	
	/*****************************************/
	
	#if 0
	while(1) {
		unsigned char data[32];
		radiolink_recv(32, data);
		uart_send_raw(data, 32);
	}

	//speaker_prebuffer();
	systick_enable();
	
	while(1)
		microphone_send();
	#endif
	
	/************ CAMERA TEST ****************/

	while(1);
	
	/*************** US TEST *****************/
	/*
	while (1)
	{
		//uart_printf("US is %i\n", us());
	}
	*/
	/************** MAIN CODE ***************/


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



			/*motor_go(MOTOR_DIR_STAHP, 100);
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
			
			

			/*motor_go(MOTOR_DIR_STAHP, 100);
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
	
	/*****************************************/
	/*
	int us_time;
	
	while(1)
	{
		radiolink_recv(4, &us_time);
		uart_printf("US: %i\n", us_time);
	}
	*/
	
	return 0;
}
