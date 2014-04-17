/* AutoKorg™ v.1.0
 * Copyright 2014 Steven Arnow <s@rdw.se>
 * 
 * 'radiolink.h' - This file is part of AutoKorg™.
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

#ifndef __RADIOLINK_H__
#define	__RADIOLINK_H__

/* Defines which channel the nRF24L01+ tranceives on */
#define	RADIOLINK_CHANNEL	0
#define	RADIOLINK_ADDR_SELF_0	0xB0
#define	RADIOLINK_ADDR_SELF_1	0x0B
#define	RADIOLINK_ADDR_SELF_2	0x50

#define	RADIOLINK_ADDR_TARG_0	0x42
#define	RADIOLINK_ADDR_TARG_1	0x55
#define	RADIOLINK_ADDR_TARG_2	0x4a

#define	RADIOLINK_MAX_RETRANSMIT	0x10
#define	RADIOLINK_TX_FIFO_FULL	0x1


void radiolink_test();

#endif
