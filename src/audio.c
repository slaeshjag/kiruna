#include "system/LPC11xx.h"
#include "uart.h"

unsigned char mic_buffer[1024];
unsigned char spk_buffer[1024];
int mic_buffer_index = 0;
int spk_buffer_index = 0;

void speaker_output() {
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x0;
	spi_send_recv(spk_buffer[spk_buffer_index++]);
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x80;
	spk_buffer_index &= 0x3FF
}

unsigned char microphone_sample() {
	unsigned char val;
	
	uart_printf("setting up microphone\n");
	/*48 MHz / 12 = 4 MHz*/
	LPC_ADC->INTEN = 0;
	LPC_SYSCON->PDRUNCFG &= ~(0x1 << 4);
	LPC_ADC->CR = 0x1 | (12 << 8) | (1 << 24);
	for(;;) {
		while(!(LPC_ADC->STAT & 0x1));
		uart_send_char(LPC_ADC->DR[0] >> 7);
		//uart_printf("\rADC: %i", (LPC_ADC->DR[0] >> 5) & 0x3FF);
		LPC_ADC->CR |= (1 << 24);
		util_delay(60);
	}
}