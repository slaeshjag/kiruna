#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"
#include "audio.h"


/*
 * Presuming that the LPC1114 runs on 48 MHz (PCLK) and an interrupt should occur each 128 us;
 * 
 * 48*10^6 iterations/s * 128*10^-6 s = 6144 iterations = 0x1800
 */

//#define INTER_TIME	0x1800
#define INTER_TIME 0xFFFF
#define DATA_SIZE	32


void interrupter_timer16_init(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL 	|= (1<<7);	//enable clock signal to 16 bit timer0 (3.5.14)
	LPC_TMR16B0->PR			= 0x0;		//set prescaler max value, not used here (18.7.4)
	
	LPC_TMR16B0->CTCR 		= 0x0;		//Timer Mode: every rising PCLK edge
	
	LPC_TMR16B0->MR0		= INTER_TIME;	//Set what timer should be to cause interrupt
	LPC_TMR16B0->MCR		|= 0x1;		//Interrupt when MR0 matches Timer Counter
	
	NVIC_EnableIRQ(TIMER_16_0_IRQn);		//enable interrupt
	LPC_TMR16B0->TCR		|= 0x3;		//reset and enable counter (18.7.2)
	LPC_TMR16B0->TCR		&= ~(0x2);	//clear reset bit (18.7.2)
	
	uart_printf("interrupter init done!\n");
}


unsigned char data[DATA_SIZE];

void TIMER16_0_IRQHandler(void)
{
	if(LPC_TMR16B0->IR & 0x1)			//Interrupt on MR0
	{
		uart_printf("MR0!\n");
		
		radiolink_send(DATA_SIZE, data);
		
		LPC_TMR16B0->TC 		= 0x0;	//reset timer
		LPC_TMR16B0->IR 		|= 0x1;	//reset interrupt
	}
}
