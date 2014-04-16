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
	spi_send(0x20);
	spi_send(0x0B);

	/* The device starts up in Enhanced ShockBurst™ mode, which is fine */
	spi_send(0x20 + RADIOLINK_CHANNEL);

	/* Set address length */
	spi_send(0x23);
	spi_send(0x01);

	/* Set up device network address */
	spi_send(0x2B);
	spi_send(RADIOLINK_ADDR_SELF_0);
	spi_send(RADIOLINK_ADDR_SELF_1);
	spi_send(RADIOLINK_ADDR_SELF_2);

	/* Set up device transmit address */
	spi_send(0x2B);
	spi_send(RADIOLINK_ADDR_SELF_0);
	spi_send(RADIOLINK_ADDR_SELF_1);
	spi_send(RADIOLINK_ADDR_SELF_2);

	return;
}


int radiolink_receive(uint8_t *data) {
	uint8_t bytes;
	int i;

	/* Get amount of byte waiting for us */
	spi_send(0xB);
	bytes = spi_recv();
	
	/* Fetch bytes */
	spi_send(0x60);
	for (i = 0; i < bytes; i++)
		data[i] = spi_recv();
	return;
}


void radiolink_transmit(uint8_t *data, int data_len) {
	int i;

	/* TODO: Check if module is ready to transmit */

	/* Enter transmit mode by turning off recieve mode */
	spi_send(0x20);
	spi_send(0x0B);

	/* Recieve address must be the same as transmit address in Enhanced ShockBurst™ mode */
	spi_send(0x2B);
	spi_send(RADIOLINK_ADDR_TARG_0);
	spi_send(RADIOLINK_ADDR_TARG_1);
	spi_send(RADIOLINK_ADDR_TARG_2);
	
	/* FIXME: Hold CSN pin low */

	/* Write payload */
	spi_send(0xA0);

	for (i = 0; i < data_len; i++)
		spi_send(data[i]);
	
	/* FIXME: Pulse CE */

	/* Disable TX */
	spi_send(0x20);
	spi_send(0x0A);
	
	/* Restore receive address to our own */
	spi_send(0x2B);
	spi_send(RADIOLINK_ADDR_SELF_0);
	spi_send(RADIOLINK_ADDR_SELF_1);
	spi_send(RADIOLINK_ADDR_SELF_2);

	return;
}

