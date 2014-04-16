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
#include <stdint.h>

/* FIXME: I should probably move the overhead over to recieve. We'll be
	recieving data a lot less frequently than we'll transmit it */


void radiolink_init() {
	/* Power on */
	spi_send_recv(0x20);
	spi_send_recv(0x0B);

	/* The device starts up in Enhanced ShockBurst™ mode, which is fine */
	spi_send_recv(0x20 + RADIOLINK_CHANNEL);

	/* Set address length */
	spi_send_recv(0x23);
	spi_send_recv(0x01);

	/* Set up device network address */
	spi_send_recv(0x2B);
	spi_send_recv(RADIOLINK_ADDR_SELF_0);
	spi_send_recv(RADIOLINK_ADDR_SELF_1);
	spi_send_recv(RADIOLINK_ADDR_SELF_2);

	/* Set up device transmit address */
	spi_send_recv(0x2B);
	spi_send_recv(RADIOLINK_ADDR_SELF_0);
	spi_send_recv(RADIOLINK_ADDR_SELF_1);
	spi_send_recv(RADIOLINK_ADDR_SELF_2);

	return;
}


static void radiolink_cmd_enter_transmit() {
	/* FIXME: Pull CSE low */
	/* Enter transmit mode by turning off recieve mode */
	spi_send_recv(0x20);
	spi_send_recv(0x0B);

	/* FIXME: Pull CSE high */
}	


static void radiolink_cmd_enter_receive() {
	/* FIXME: Pull CSE low */
	/* Enter receive mode by turning on recieve mode */
	spi_send_recv(0x20);
	spi_send_recv(0x0A);

	/* FIXME: Pull CSE high */
}	


static void radiolink_cmd_flush_tx() {
	/* FIXME: Pull CSE low */

	spi_send_recv(0xE1);

	/* FIXME: Pull CSE high */
}


static void radiolink_cmd_flush_rx() {
	/* FIXME: Pull CSE low */

	spi_send_recv(0xE1);

	/* FIXME: Pull CSE high */
}


static void radiolink_cmd_addr_self() {
	/* FIXME: Pull CSE low */

	spi_send_recv(0x2B);
	spi_send_recv(RADIOLINK_ADDR_SELF_0);
	spi_send_recv(RADIOLINK_ADDR_SELF_1);
	spi_send_recv(RADIOLINK_ADDR_SELF_2);

	/* FIXME: Pull CSE high */
}


static void radiolink_cmd_addr_targ() {
	/* FIXME: Pull CSE low */

	spi_send_recv(0x2B);
	spi_send_recv(RADIOLINK_ADDR_TARG_0);
	spi_send_recv(RADIOLINK_ADDR_TARG_1);
	spi_send_recv(RADIOLINK_ADDR_TARG_2);

	/* FIXME: Pull CSE high */
}


int radiolink_receive(uint8_t *data) {
	uint8_t bytes;
	int i;

	/* Get amount of byte waiting for us */
	spi_send_recv(0xB);
	
	/* Fetch bytes */
	spi_send_recv(0x60);
	for (i = 0; i < bytes; i++)
		data[i] = spi_send_recv(0xFF);
	
	/* Flush RX */
	spi_send_recv(0xE2);

	return;
}


void radiolink_transmit_begin() {
	/* FIXME: Hold CSN pin low */
	/* Must be low for wireless module to care about SPI */
	
	/* TODO: Check if module is ready to transmit */

	radiolink_cmd_flush_rx();
	radiolink_cmd_enter_transmit();
	radiolink_cmd_flush_tx();
	radiolink_cmd_addr_targ();
	
	return;
}


void radiolink_receive_begin() {
	/* FIXME: Hold CSN pin low */
	/* Must be low for wireless module to care about SPI */
	
	/* TODO: Check if module is ready to transmit */

	radiolink_cmd_enter_receive();
	radiolink_cmd_addr_self();
	
	return;
}


void radiolink_transmit(uint8_t *data, int data_len) {
	int i;

	radiolink_transmit_begin();
	
	/* FIXME: Pull CSE low */
	/* Write payload */
	spi_send_recv(0xA0);

	for (i = 0; i < data_len; i++)
		spi_send_recv(data[i]);
	/* FIXME: Pull CSE high */

	radiolink_receive_begin();
	
	return;
}


void radiolink_test() {
	uint8_t status;
	int i;
	
	LPC_GPIO1->DIR |= (1 << 9);
	LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1;
	util_delay(500000);

	do {
		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1;
		util_delay(500000);
		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 0;
		util_delay(20);
		status = spi_send_recv(0x06);
		uart_printf("Status: 0x%X\n", (unsigned int) status);
		status = spi_send_recv(0x07);
		uart_printf("Config: 0x%X\n", (unsigned int) status);

		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1;
		util_delay(500000);
		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 0;
		util_delay(20);
		spi_send_recv(0x20);
		spi_send_recv(0x0A);
	} while (status);



//	radiolink_init();

//	for (;;) {
		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 1;
		util_delay(500000);
		LPC_GPIO1->MASKED_ACCESS[(1 << 9)] = 0;
		status = spi_send_recv(0x0A);
		for (i = 0; i <3; i++)
			uart_printf("0x%X ", spi_send_recv(0xFF));
		uart_printf("\n");
//	}
	for (;;);

}
