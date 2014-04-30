#include <stdint.h>
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "radiolink.h"
#include "motor.h"
#include "audio.h"
#include "ultrasonic.h"
#include "microswitch.h"



void initialize(void) {
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
	uart_init();
	
	/********* Enable SPI0 ************/
	spi_init();

	/* Enable timers */
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 10);
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 9);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);

	audio_init();
	
	/*Disable systick*/
	SysTick->CTRL = 0;

	motor_init();
	uart_printf("motor_init() done\n");
	us_init();
	uart_printf("us_init() done\n");
	ms_init();
	uart_printf("ms_init() done\n");
	i2c_init();
	uart_printf("i2c_init() done\n");
	radiolink_init(32);
	uart_printf("radiolink_init() done\n");
}

void systick_irq() {
	//microphone_sample();
	speaker_output();
}

void systick_enable() {
	/* Trig 8000 times per second */
	SysTick->LOAD = SYSTEM_CLOCK / 8000;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x1 | 0x2 | 0x4;
}

int main(int ram, char **argv) {
	initialize();
	uart_printf("AutoKorg™ READY TO WRECK SOME HAVOC!\n");
	
	util_delay(1000000);

	
	/*****************************************/
	
	/*while(1) {
		unsigned char data[32];
		radiolink_recv(32, data);
		uart_send_raw(data, 32);
	}*/
	
	while(1) {
		unsigned char data[32];
		uart_recv_raw(data, 32);
		radiolink_send_unreliable(32, data);
	}
	
	//speaker_prebuffer();
	systick_enable();
	
	while(1) {
		//microphone_send();
		audio_loop();
	}
	
	/************ CAMERA TEST ****************/
	
		// QVGA RGB16
	char sub_adr = 0x0B;
	char cam_test = ov7670_test(sub_adr);
	uart_printf("CAM at 0x0B is %x\n", cam_test);
	while(1);

	/*****************************************/
	while (1)
	{
		uart_printf("US is %i\n", us());
	}
	
	/*****************************************/

	motor_logic();
	
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
