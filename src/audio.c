#include "system/LPC11xx.h"
#include "uart.h"
#include "spi.h"
#include "radiolink.h"
#include "main.h"

static int late_dac = 0;

#ifdef MOTHERSHIP

unsigned char mic_buffer[2][16];
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
	if (radio_used()) {
		late_dac = 1;
		return;
	}

	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x0;
	spi_send_recv(spk_buffer[spk_buffer_index++]);
	if (spk_buffer_index == 1024)
		spk_buffer_index = 0;
	LPC_GPIO0->MASKED_ACCESS[0x80] = 0x80;
	late_dac = 0;
	//spk_buffer_index &= 0x3FF;
}


void speaker_fill(unsigned char *data, int data_len) {
	int i;
	for (i = 0; i < data_len; i++) {
		spk_buffer[spk_buffer_next++] = data[i];
		if (spk_buffer_next == 1024)
			spk_buffer_next = 0;
	}

	return;
}


unsigned char microphone_sample() {
	unsigned char data;
	void *tmp;
	static int index = 0;
	
	while(!(LPC_ADC->STAT & 0x1));
	data = (LPC_ADC->DR[0] >> 7);
	
	sample_buffer[index++] = data;
	
	if(index == 16) {
		index = 0;
		tmp = sample_buffer;
		sample_buffer = send_buffer;
		send_buffer = tmp;
		send_data = 1;
	}
	
	LPC_ADC->CR |= (1 << 24);
	return data;
}

int microphone_send() {
	if(!send_data)
		return 0;
	if (send_buffer[0] == 0xFF)
		send_buffer[0] = 0xFE;
	radiolink_send_unreliable(16, send_buffer);
//	radiolink_send(16, "ARNEarneARNEarne");

	send_data = 0;
	return 1;
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
	radiolink_recv(16, &spk_buffer[spk_buffer_next]);
	spk_buffer_next += 16;
	if (spk_buffer_next == 1024)
		spk_buffer_next = 0;
	return;
}

#endif

int audio_late_dac() {
	return late_dac;
}

