/* AutoKorg™ v.1.0
 * Copyright 2014 Steven Arnow <s@rdw.se>
 * 
 * 'spi.h' - This file is part of AutoKorg™ v.1.0.
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

#ifndef __SPI_H__
#define	__SPI_H__


void spi_send(uint8_t data);
uint8_t spi_recv();


#endif
