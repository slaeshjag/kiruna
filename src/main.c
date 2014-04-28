#include <stdint.h>
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include "spi.h"
#include "motor.h"
#include "audio.h"



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
}





int main(int ram, char **argv) {
	uint16_t sample;
	extern int spk_buffer_next;
	extern int spk_buffer_index;
	extern int spk_buffer[1024];
	int i;
	
	initialize();
	motor_init();
	us_init();
	ms_init();
	util_delay(200);
	
	uart_printf("AutoKorg™ READY TO WRECK SOME HAVOC!\n");
	
	/********************************************/

	/* Attempt to plan the flow */

	SysTick->CTRL = 0;
	/* Trig 8000 times per second */
	SysTick->LOAD = SYSTEM_CLOCK / 8000;
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	
	/********************************************/
	
	int us_state = 0;
	int us_pulse_counter = 0;
	int us_pulse_length = 0;
	
	for (i = 0;; i++)
	{
		while (!(SysTick->CTRL & (1 << 16)));
		audio_loop();
		

		/* 8 tasks ought to be enough for anybody, right? */
		switch (i & 0x7) {
			case 0:	// Ultrasonic sensor
				if (us_state == 0) 
				{
					us_state = 1;
					us_trig();	// blocking
				}
				else if (US_PORT->DATA & (1<<US_ECHO))	// count incoming pulse
				{
					us_state = 2;
					us_pulse_counter++;
				}
				else if (us_state == 2)	// Returning pulse has ended
				{
					us_pulse_length = us_pulse_counter;
					us_pulse_counter = 0;
					us_state = 0;
				}
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				break;
		}
	}

	/************** Old code ***************/
	/*	
	
	motor_go(MOTOR_DIR_FORWARD);
	int us_time;
	
	while(1)
	{
		us_time = us();
		
		if(ms_left_pressed())
		{
			motor_go(MOTOR_DIR_LEFT);
			util_delay(MOTOR_TIME_45);
			
			motor_go(MOTOR_DIR_BACKWARD);
			util_delay(MOTOR_TIME_SHORT);
			
			motor_go(MOTOR_DIR_RIGHT);
			util_delay(MOTOR_TIME_45);
			
			motor_go(MOTOR_DIR_FORWARD);
		}
		else if(ms_right_pressed())
		{
			motor_go(MOTOR_DIR_RIGHT);
			util_delay(MOTOR_TIME_45);
			
			motor_go(MOTOR_DIR_BACKWARD);
			util_delay(MOTOR_TIME_SHORT);
			
			motor_go(MOTOR_DIR_LEFT);
			util_delay(MOTOR_TIME_45);
			
			motor_go(MOTOR_DIR_FORWARD);
		}
		else if (us_time < 9000)	// It's not over 9000!!!! Turn!
		{
			motor_go(MOTOR_DIR_LEFT);
			util_delay(MOTOR_TIME_90);
			
			motor_go(MOTOR_DIR_FORWARD);
			util_delay(MOTOR_TIME_SHORT);
			
			motor_go(MOTOR_DIR_RIGHT);
			util_delay(MOTOR_TIME_90);
		}
		
		//radiolink_send(4, &us_time);	//Print us_time to radio module (int is 4B, sends pointer to value)
	}
	*/
	/********** Code for receiver **********/
	/*
	
	int us_time;
	
	while(1)
	{
		radiolink_recv(4, &us_time);
		uart_printf("US: %i\n", us_time);
	}
	*/
	/***** Steven's old test code *********/
	/*
	MOTOR_PORT->MASKED_ACCESS[MOTOR_MASK] |= (MOTOR_MASK);
	util_delay(2000000);
	uart_printf("Going forward\n");
	motor_go(MOTOR_DIR_FORWARD);
	util_delay(2000000);
	uart_printf("Going left\n");
	motor_go(MOTOR_DIR_LEFT);
	util_delay(2000000);
	uart_printf("Going right\n");
	motor_go(MOTOR_DIR_RIGHT);
	util_delay(2000000);
	uart_printf("Going backwards\n");
	motor_go(MOTOR_DIR_BACKWARD);
	util_delay(2000000);
	uart_printf("Stopping\n");
	motor_go(MOTOR_DIR_STAHP);

	for (;;) {
		
	}
	*/
	/***************************************/
	return 0;
}
