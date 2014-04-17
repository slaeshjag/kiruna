#include <stdint.h>
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include "motor.h"

extern asm_test();

static void scsi_init_pinp(volatile uint32_t *p) {
//	(*p) &= (~0x1F);
//	(*p) |= 0x2;
}


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

static unsigned char *ascii_filter(unsigned char *s) {
	static unsigned char ret[17];
	int i = 0;
	while(i < 16) {
		ret[i] = (*s < 32 || *s > 126)? '.' : (*s);
		i++;
		s++;
	}
	return ret;
}


static int64_t prompt_read_number(void) {
	uint32_t sector = 0;
	uint8_t base = 10;
	char c;
	
	uart_send_string(" (Args: sector number) ");
	for(;;) {
		c = uart_recv_char();
		switch(c) {
			case 'x':
				if(base == 8 && !sector) {
					base = 16;
					uart_send_char(c);
				}
				break;
			case '0':
				if(base == 10 && !sector) {
					base = 8;
					uart_send_char(c);
					break;
				}
			case '1': case '2': case '3': case '4': case '5': case '6': case '7':
				uart_send_char(c);
				sector *= base;
				sector += c - '0';
				break;
			case '8': case '9': 
				if(base < 10)
					break;
				uart_send_char(c);
				sector *= base;
				sector += c - '0';
				break;
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				if(base < 16)
					break;
				uart_send_char(c);
				sector *= base;
				sector += c - 'A';
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
				if(base < 16)
					break;
				uart_send_char(c);
				sector *= base;
				sector += c - 'a';
				break;
			case 127:
				uart_send_string("\b \b");
				if(sector) {
					sector /= base;
				} else {
					if(base == 16)
						base = 8;
					else if(base == 8)
						base = 10;
					else
						return -1;
				}
				break;
			case ' ': case '\n':
				return sector;
		}
	}
}

static void prompt() {
	char c;
	int64_t arg;
	uart_send_string("\n> ");
	c = uart_recv_char();
	uart_send_char(c);
	switch(c) {
		case 'r':
			break;
		default:
			uart_send_char('?');
		case '?':
		case 'h':
			uart_send_string("\nValid commands are [r - read sector, h/? - help]");
	}
}

int main(int ram, char **argv) {
	//int i;
	
	
	initialize();
	util_delay(200);
	uart_printf("System booted up, %u bytes of RAM used\n", ram);
	LPC_GPIO2->DIR |= MOTOR_MASK;
	scsi_init_pinp(&LPC_IOCON->PIO2_8);
	scsi_init_pinp(&LPC_IOCON->PIO2_9);
	scsi_init_pinp(&LPC_IOCON->PIO2_10);
	scsi_init_pinp(&LPC_IOCON->PIO2_11);

	//radiolink_test();
	microphone_sample();

	
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
	for(;;);
	util_delay(2000000);
	uart_printf("Stopping\n");
	motor_go(MOTOR_DIR_STAHP);

	for (;;) {
		
	}

	return 0;
}
