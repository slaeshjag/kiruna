/* AutoKorg™ v.1.0
 * Copyright 2014 Steven Arnow <s@rdw.se>
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

#include "system/LPC11xx.h"
#include "spi.h"
#include <stdint.h>

uint8_t spi_send_recv(uint8_t data) {
	while(!(LPC_SSP0->SR & 0x2));
	LPC_SSP0->DR = data;
	while(!(LPC_SSP0->SR & 0x4));
	return LPC_SSP0->DR;
}
