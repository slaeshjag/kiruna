#include "system/LPC11xx.h"
#include "uart.h"
#include "spi.h"
#include "radiolink.h"

unsigned char mic_buffer[2][32];
static unsigned char *sample_buffer = mic_buffer[0];
static unsigned char *send_buffer = mic_buffer[1];
int send_data = 0;

unsigned char spk_buffer[1024];
int mic_buffer_index = 0;
int mic_buffer_send = 0;
int mic_buffer_looped = 0;
int spk_buffer_index = 0;
int spk_buffer_next = 0;

void audio_init() {
	/*Enable ADC*/
	LPC_IOCON->R_PIO0_11 = 0x2;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
	/*48 MHz / 12 = 4 MHz*/
	LPC_ADC->INTEN = 0;
	LPC_SYSCON->PDRUNCFG &= ~(0x1 << 4);
	LPC_ADC->CR = 0x1 | (12 << 8) | (1 << 24);
	
	/*Enable DAC*/
	LPC_GPIO0->DIR |= 0x80;
}

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
	void *tmp;
	static int index = 0;
	
	while(!(LPC_ADC->STAT & 0x1));
	data = (LPC_ADC->DR[0] >> 7);
	
	sample_buffer[index++] = data;
	
	if(index == 32) {
		index = 0;
		tmp = sample_buffer;
		sample_buffer = send_buffer;
		send_buffer = tmp;
		send_data = 1;
	}
	
	LPC_ADC->CR |= (1 << 24);
	return data;
}

void microphone_send() {
	if(!send_data)
		return;
	
	radiolink_send(32, send_buffer);
	
	send_data = 0;
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
