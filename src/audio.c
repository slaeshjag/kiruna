#include "system/LPC11xx.h"
#include "uart.h"
#include "spi.h"

unsigned char mic_buffer[1024];
unsigned char spk_buffer[1024];
int mic_buffer_index = 0;
int mic_buffer_send = 0;
int mic_buffer_looped = 0;
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

unsigned char microphone_sample() {
	unsigned char data;
	while(!(LPC_ADC->STAT & 0x1));
	/*mic_buffer[mic_buffer_index++] = LPC_ADC->DR[0] >> 7;
	if(mic_buffer_index >= 1024) {
		mic_buffer_index = 0;
		mic_buffer_looped = 1;
	}*/
	uart_send_char(data = (LPC_ADC->DR[0] >> 7));
	//uart_printf("0x%x\n", (LPC_ADC->DR[0] >> 7) & 0xFF);
	//uart_printf("\rADC: %i", (LPC_ADC->DR[0] >> 5) & 0x3FF);
	LPC_ADC->CR |= (1 << 24);
	return data;
}

void microphone_send() {
	if(mic_buffer_send < mic_buffer_index || mic_buffer_looped) {
		uart_send_char(mic_buffer[mic_buffer_send++]);
		if(mic_buffer_send >= 1024) {
			mic_buffer_send = 0;
			mic_buffer_looped = 0;
		}
	}
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
