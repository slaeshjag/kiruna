#include "system/LPC11xx.h"
#include "uart.h"
#include "spi.h"

unsigned char mic_buffer[1024];
unsigned char spk_buffer[1024];
int mic_buffer_index = 0;
int spk_buffer_index = 0;
int spk_buffer_next = 0;

void speaker_output() {
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x0;
	spi_send_recv(spk_buffer[spk_buffer_index++]);
	if (spk_buffer_index == 1024)
		spk_buffer_index = 0;
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x80;
	spk_buffer_index &= 0x3FF;
}

void microphone_sample() {
	
	while(!(LPC_ADC->STAT & 0x1));
	uart_send_char(LPC_ADC->DR[0] >> 7);
	//uart_printf("\rADC: %i", (LPC_ADC->DR[0] >> 5) & 0x3FF);
	LPC_ADC->CR |= (1 << 24);
	
}

void speaker_prebuffer() {
	uint16_t sample;

	for (;;) {
		if (!(sample = uart_recv_try()))
			continue;
		sample &= 0xFF;
		spk_buffer[spk_buffer_next++] = sample;
		uart_printf("%i ", spk_buffer_next);
		if (spk_buffer_next == 20)
			break;
	}
}


void audio_loop() {
	uint16_t sample;

	microphone_sample();
	if (spk_buffer_next != spk_buffer_index)
		speaker_output();
	
	for (;;) {
		if (!(sample = uart_recv_try()))
			break;
		sample &= 0xFF;
		spk_buffer[spk_buffer_next++] = sample;
		if (spk_buffer_next == 1024)
			spk_buffer_next = 0;
	}

	return;
}
