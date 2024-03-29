/* AutoKorg™ v.1.0
 * Copyright 2014 Axel Isaksson <s@rdw.se>
 * 
 * 'spi.c' - This file is part of AutoKorg™ v.1.0.
 * 
 * AutoKorg™ v.1.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * AutoKorg™ v.1.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AutoKorg™ v.1.0.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdint.h>
#include <limits.h>
#include "system/LPC11xx.h"
#include "spi.h"
#include "uart.h"
#include "main.h"
#include "util.h"
#include "audio.h"

#define CSN_PORT LPC_GPIO1
#define CE_PORT LPC_GPIO1
#define CSN_PIN (1 << 9)
#define CE_PIN (1 << 8)

#define DELAY 5
static int radio_used_flag = 0;

enum Cmd {
	CMD_REG_READ = 0x0,
	CMD_REG_WRITE = 0x20,
	CMD_RECV_PAYLOAD = 0x61,
	CMD_SEND_PAYLOAD = 0xA0,
	CMD_FLUSH_TX = 0xE1,
	CMD_FLUSH_RX = 0xE2,
	CMD_REUSE_TX = 0xE3,
	CMD_PAYLOAD_WIDTH = 0x60,
	CMD_SEND_ACK_PAYLOAD = 0xA8,
	CMD_SEND_PAYLOAD_NOACK = 0xB0,
	CMD_NOP = 0xFF,
};

enum Reg {
	REG_CONFIG = 0x0,
	REG_EN_AA = 0x1,
	REG_EN_RXADDR = 0x2,
	REG_SETUP_AW = 0x3,
	REG_SETUP_RETR = 0x4,
	REG_RF_CH = 0x5,
	REG_RF_SETUP = 0x6,
	REG_STATUS = 0x7,
	REG_OBSERVE_TX = 0x8,
	REG_RPD = 0x9,
	REG_RX_ADDR_P0 = 0xA,
	REG_RX_ADDR_P1 = 0xB,
	REG_RX_ADDR_P2 = 0xC,
	REG_RX_ADDR_P3 = 0xD,
	REG_RX_ADDR_P4 = 0xE,
	REG_RX_ADDR_P5 = 0xF,
	REG_TX_ADDR = 0x10,
	REG_RX_PW_P0 = 0x11,
	REG_RX_PW_P1 = 0x12,
	REG_RX_PW_P2 = 0x13,
	REG_RX_PW_P3 = 0x14,
	REG_RX_PW_P4 = 0x15,
	REG_RX_PW_P5 = 0x16,
	REG_FIFO_STATUS = 0x17,
	REG_DYNPD = 0x1C,
	REG_FEATURE = 0x1D,
};

static char packet_size = 0;

static inline void cmd_start() {
	radio_used_flag = 1;
	CSN_PORT->MASKED_ACCESS[CSN_PIN] = 0;
	util_delay(DELAY);
}

static inline void cmd_end() {
	radio_used_flag = 0;
	CSN_PORT->MASKED_ACCESS[CSN_PIN] = ~0;
	util_delay(DELAY);
	if (audio_late_dac()) {
		#ifdef MOTHERSHIP
		speaker_output();
		#endif
	}
}

static inline void ce_on() {
	CE_PORT->MASKED_ACCESS[CE_PIN] = ~0;
	util_delay(20);
}

static inline void ce_off() {
	CE_PORT->MASKED_ACCESS[CE_PIN] = 0;
	util_delay(DELAY);
}

unsigned char radiolink_read_reg(enum Reg reg, int size, unsigned char *buf) {
	int i;
	unsigned char status;
	
	cmd_start();
	status = spi_send_recv(CMD_REG_READ | reg);
	for(i = 0; i < size; i++)
		buf[i] = spi_send_recv(CMD_NOP);
	cmd_end();
	
	return status;
}

unsigned char radiolink_write_reg(enum Reg reg, int size, unsigned char *buf) {
	int i;
	unsigned char status;
	
	cmd_start();
	status = spi_send_recv(CMD_REG_WRITE | reg);
	for(i = 0; i < size; i++)
		spi_send_recv(buf[i]);
	cmd_end();
	
	return status;
}

unsigned char radiolink_status() {
	unsigned char status;
	
	cmd_start();
	status = spi_send_recv(CMD_NOP);
	cmd_end();
	
	return status;
}

unsigned char radiolink_flush() {
	unsigned char status;
	cmd_start();
	spi_send_recv(CMD_FLUSH_TX);
	cmd_end();
	cmd_start();
	status = spi_send_recv(CMD_FLUSH_RX);
	cmd_end();
	
	return status;
}

unsigned char radiolink_send(int size, unsigned char *data) {
	unsigned char status;
	int i, last_timer = global_timer;
	
	ce_on();
	util_delay(10);
	
	for(; size > 0; size -= packet_size) {
		do {
			status = radiolink_status();
			/*uart_printf("arne 0x%x\n", status);*/
			if((status & 0x10) || global_timer - last_timer >= 800) {
				last_timer = global_timer;
				radiolink_flush();
				goto error;
			}
		} while(status & 0x1);
		
		cmd_start();
		status = spi_send_recv(CMD_SEND_PAYLOAD);
		for(i = 0; i < packet_size; i++) {
			spi_send_recv(i < size ? data[i] : 0xFF);
		}
		cmd_end();
		
		do {
			status = radiolink_status();
			/*uart_printf("arne 0x%x\n", status);*/
			if((status & 0x10) || global_timer - last_timer >= 800) {
				
				radiolink_flush();
				goto error;
			}
		} while(!(status & 0x20));
	}
	
	
	error:
	ce_off();
	status = radiolink_status();
	radiolink_write_reg(REG_STATUS, 1, &status);
	
	return status;
}

unsigned char radiolink_send_unreliable(int size, unsigned char *data) {
	unsigned char status;
	int last_timer = global_timer;
	int i;
	
	ce_on();
	util_delay(10);
	
	for(; size > 0; size -= packet_size) {
		do {
			status = radiolink_status();
			/*uart_printf("arne 0x%x\n", status);*/
			if((status & 0x10) || global_timer - last_timer >= 800) {
				last_timer = global_timer;
				radiolink_flush();
				status = radiolink_status();
				radiolink_write_reg(REG_STATUS, 1, &status);
			}
		} while(status & 0x1);
		
		cmd_start();
		status = spi_send_recv(CMD_SEND_PAYLOAD_NOACK);
		for(i = 0; i < packet_size; i++) {
			spi_send_recv(i < size ? data[i] : 0xFF);
		}
		cmd_end();
	}
	
	if((status = radiolink_status()) & 0x10)
		radiolink_flush();
	
	ce_off();
	radiolink_write_reg(REG_STATUS, 1, &status);
	
	return status;
}


int radiolink_send_stubborn(int size, unsigned char *data, int timeout) {
	int status, time_now;
	time_now = global_timer;
	
	for (;;)
		if ((status = radiolink_send(size, data)) & 0x10) {
			if (global_timer - time_now >= timeout)
				return 0;
		} else
			return 1;

}


unsigned char radiolink_recv_timeout(int size, unsigned char *data, int timeout) {
	unsigned char status = 0xFF, config, tmp;
	int i, time_now, n = 0;
	
	if(!size)
		return 0x0;

	time_now = global_timer;
	//uart_printf("0x%X -- \n", (unsigned int) data);
	radiolink_read_reg(REG_FIFO_STATUS, 1, &status);
	//uart_printf("0x%X 0x%X\n", status, radiolink_status());

	radiolink_read_reg(REG_CONFIG, 1, &config);
	config |= 0x1;
	radiolink_write_reg(REG_CONFIG, 1, &config);
	
	ce_on();
	//util_delay(10);

	for(; size > 0; size -= packet_size) {
		while (global_timer - time_now < timeout && !((status = radiolink_status()) & 0x40));
		if ((global_timer - time_now >= timeout)) {
			n = 1;
			goto arne;
		}

		if (!(status & 0x40)) {
			n = 1;
			goto arne;
		}

		cmd_start();
		spi_send_recv(CMD_RECV_PAYLOAD);
		for(i = 0; i < packet_size; i++) {
			tmp = spi_send_recv(CMD_NOP);
			if(i < size)
				data[i] = tmp;
		}
		cmd_end();
	}
	
	arne:
	ce_off();
	
	config &= ~0x1;
	radiolink_write_reg(REG_CONFIG, 1, &config);
	
	radiolink_write_reg(REG_STATUS, 1, &status);
	return n ? 0xFF: status;
}

unsigned char radiolink_recv(int size, unsigned char *data) {
	unsigned char status = 0xFF, config, tmp;
	int i;

	if(!size)
		return 0x0;

	radiolink_read_reg(REG_CONFIG, 1, &config);
	config |= 0x1;
	radiolink_write_reg(REG_CONFIG, 1, &config);

	ce_on();
	//util_delay(10);

	for(; size > 0; size -= packet_size) {
		while(!((status = radiolink_status()) & 0x40));
		cmd_start();
		spi_send_recv(CMD_RECV_PAYLOAD);
		for(i = 0; i < packet_size; i++) {
			tmp = spi_send_recv(CMD_NOP);
			if(i < size)
				data[i] = tmp;
			}
		cmd_end();
	}

	ce_off();

	config &= ~0x1;
	radiolink_write_reg(REG_CONFIG, 1, &config);

	radiolink_write_reg(REG_STATUS, 1, &status);
	return status;
}


int radiolink_init(char _packet_size) {
	unsigned char reg[5];
	unsigned char status, config;
	
	if(_packet_size > 32)
		return -1;
	
	packet_size = _packet_size;
	CSN_PORT->DIR |= CSN_PIN;
	CE_PORT->DIR |= CE_PIN;
	CSN_PORT->MASKED_ACCESS[CSN_PIN] = ~0;
	CE_PORT->MASKED_ACCESS[CE_PIN] = 0;
	
	util_delay(DELAY*10000);
	
	radiolink_status();
	
	reg[0] = 0x00;
	radiolink_write_reg(REG_CONFIG, 1, reg);
	util_delay(150000);
	reg[0] = 0x0A;
	radiolink_write_reg(REG_CONFIG, 1, reg);
	util_delay(150000);
	
	/*RF channel*/
	reg[0] = 125;
	radiolink_write_reg(REG_RF_CH, 1, reg);
	util_delay(100000);
	
	radiolink_read_reg(REG_CONFIG, 1, reg);
	config = reg[0];
	
	//uart_printf("config 0x%x\n", reg[0]);
	/*Fifo size, 0-32*/
	reg[0] = _packet_size;
	radiolink_write_reg(REG_RX_PW_P0, 1, reg);
	status = radiolink_status();
	
	/*Clear fifo flags*/
	reg[0] = 0xFF;
	radiolink_write_reg(REG_STATUS, 1, reg);
	
	radiolink_flush();
	radiolink_read_reg(REG_FIFO_STATUS, 1, reg);
	
	uart_printf("radiolink init: status 0x%x config 0x%x fifo 0x%x\n", status, config, reg[0]);
	//TODO: check for error status, etc
	
	/*for(;;) {
		uart_printf("status 0x%x\n", radiolink_send(4, data));
		status = radiolink_read_reg(REG_FIFO_STATUS, 1, reg);
		uart_printf("fifo 0x%x status 0x%x\n", reg[0], status);
		util_delay(50000);
	}*/
	
	/*for(;;) {
		radiolink_recv(4, data);
		uart_printf("got data: %s\n", data);
	}
	
	for(;;);*/
	
	return 0;
}


int radio_used() {
	return radio_used_flag;
}
