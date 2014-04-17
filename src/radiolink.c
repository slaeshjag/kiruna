/* AutoKorg™ v.1.0
 * Copyright 2014 Steven Arnow <s@rdw.se>
 * 
 * 'radiolink.c' - This file is part of AutoKorg™.
 * 
 * AutoKorg™ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * AutoKorg™ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AutoKorg™.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "system/LPC11xx.h"
#include "radiolink.h"
#include "spi.h"
#include "util.h"
#include "uart.h"
#include <stdint.h>

/* FIXME: I should probably move the overhead over to recieve. We'll be
	recieving data a lot less frequently than we'll transmit it */

#define	RADIO_ATTENTION		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 0;
#define	RADIO_DONE		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1 << 9; util_delay(1);

void radiolink_init() {
	RADIO_DONE;
	util_delay(50000);

	RADIO_ATTENTION;
	/* Power on */
	spi_send_recv(0x20);
	spi_send_recv(0x0B);
	RADIO_DONE;
	
	/* The radio module needs > 100 ms to start up */
	util_delay(150000);

	/* The device starts up in Enhanced ShockBurst™ mode, which is fine */
	RADIO_ATTENTION;
	spi_send_recv(0x20 + RADIOLINK_CHANNEL);
	RADIO_DONE;

	/* Set address length */
	RADIO_ATTENTION;
	spi_send_recv(0x23);
	spi_send_recv(0x01);
	RADIO_DONE;

	/* Set up device network address for receiving acks */
	RADIO_ATTENTION;
	spi_send_recv(0x2B);
	spi_send_recv(RADIOLINK_ADDR_SELF_0);
	spi_send_recv(RADIOLINK_ADDR_SELF_1);
	spi_send_recv(RADIOLINK_ADDR_SELF_2);
	RADIO_DONE;


	/* Set up device network address */
	RADIO_ATTENTION;
	spi_send_recv(0x2A);
	spi_send_recv(RADIOLINK_ADDR_TARG_0);
	spi_send_recv(RADIOLINK_ADDR_TARG_1);
	spi_send_recv(RADIOLINK_ADDR_TARG_2);
	RADIO_DONE;

	/* Set up device transmit address */
	RADIO_ATTENTION;
	spi_send_recv(0x30);
	spi_send_recv(RADIOLINK_ADDR_TARG_0);
	spi_send_recv(RADIOLINK_ADDR_TARG_1);
	spi_send_recv(RADIOLINK_ADDR_TARG_2);
	RADIO_DONE;

	return;
}

static void radiolink_reset_hard() {
	uart_printf("Packet loss!\n");

	for(;;);
}


static void radiolink_wait_for_packet() {
	uint8_t stat;
	
	for (;;) {
		RADIO_ATTENTION;
		stat = spi_send_recv(0xFF);
		RADIO_DONE;
		if ((stat & 0xE) == 0xE)
			break;
	}
	
	return;
}


static void radiolink_wait_until_tx_ready() {
	uint8_t state;

	for (;;) {
		RADIO_ATTENTION;
		state = spi_send_recv(0xFF);
		RADIO_DONE;
		if (state & RADIOLINK_MAX_RETRANSMIT) {	/* BAD */
			radiolink_reset_hard();
			return;
		}

		if (!(state & RADIOLINK_TX_FIFO_FULL))
			break;
	}

	return;
}


static void radiolink_cmd_enter_transmit() {
	RADIO_ATTENTION;
	/* Enter transmit mode by turning off recieve mode */
	spi_send_recv(0x20);
	spi_send_recv(0x0B);
	RADIO_DONE;
}	


static void radiolink_cmd_enter_receive() {
	RADIO_ATTENTION;
	/* Enter receive mode by turning on recieve mode */
	spi_send_recv(0x20);
	spi_send_recv(0x0A);
	RADIO_DONE;
}	


static void radiolink_cmd_flush_tx() {
	RADIO_ATTENTION;

	spi_send_recv(0xE1);
	spi_send_recv(0xFF);

	RADIO_DONE;
}


static void radiolink_cmd_flush_rx() {
	RADIO_ATTENTION;

	spi_send_recv(0xE1);
	spi_send_recv(0xFF);
	
	RADIO_DONE;
}


static void radiolink_cmd_addr_self() {
	RADIO_ATTENTION;

	spi_send_recv(0x2A);
	spi_send_recv(RADIOLINK_ADDR_SELF_0);
	spi_send_recv(RADIOLINK_ADDR_SELF_1);
	spi_send_recv(RADIOLINK_ADDR_SELF_2);
	
	RADIO_DONE;
}


static void radiolink_cmd_addr_targ() {
	RADIO_ATTENTION;

	spi_send_recv(0x2A);
	spi_send_recv(RADIOLINK_ADDR_TARG_0);
	spi_send_recv(RADIOLINK_ADDR_TARG_1);
	spi_send_recv(RADIOLINK_ADDR_TARG_2);

	RADIO_DONE;
}


int radiolink_receive(uint8_t *data) {
	uint8_t bytes;
	int i;
	
	RADIO_ATTENTION;
	/* Get amount of byte waiting for us */
	spi_send_recv(0xB);
	bytes = spi_send_recv(0xF);
	RADIO_DONE;
	
	/* Fetch bytes */
	RADIO_ATTENTION;
	spi_send_recv(0x60);
	for (i = 0; i < bytes; i++)
		data[i] = spi_send_recv(0xFF);
	RADIO_DONE;
	
	radiolink_cmd_flush_rx();

	return bytes;
}


void radiolink_transmit_begin() {
	radiolink_wait_until_tx_ready();
	/* TODO: Check if module is ready to transmit */

	radiolink_cmd_flush_rx();
	radiolink_cmd_enter_transmit();
	radiolink_cmd_flush_tx();
	radiolink_cmd_addr_targ();
	
	return;
}


void radiolink_receive_begin() {
	/* Must be low for wireless module to care about SPI */
	
	radiolink_wait_for_packet();
	radiolink_cmd_enter_receive();
	radiolink_cmd_addr_self();
	
	return;
}


void radiolink_transmit(uint8_t *data, int data_len) {
	int i;

	radiolink_transmit_begin();
	
	/* Write payload */
	RADIO_ATTENTION;
	spi_send_recv(0xA0);

	for (i = 0; i < data_len; i++)
		spi_send_recv(data[i]);
	RADIO_DONE;

	radiolink_receive_begin();
	
	return;
}


void radiolink_test() {
	LPC_GPIO1->DIR |= (1 << 9);
	LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1;

	radiolink_init();

	for (;;);

}
