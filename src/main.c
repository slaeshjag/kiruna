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

	/* Enable timers */
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 10);
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 9);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);
	
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
	i2c_init();
	uart_printf("i2c_init() done\n");
	radiolink_init(32);
	uart_printf("radiolink_init() done\n");

	uart_printf("AutoKorg™ READY TO WRECK SOME HAVOC!\n");
	
	util_delay(1000000);

	
	/*****************************************/
	
	while(1) {
		unsigned char data[32];
		radiolink_recv(32, data);
		uart_send_raw(data, 32);
	}

	//speaker_prebuffer();
	systick_enable();
	
	while(1)
		microphone_send();
	
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
