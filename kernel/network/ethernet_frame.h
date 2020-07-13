
/******************************************************************************
 *       ninjastorms - shuriken operating system                              *
 *                                                                            *
 *    Copyright (C) 2013 - 2016  Andreas Grapentin et al.                     *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/

#pragma once

#include <sys/types.h>

typedef enum {
  ARP = 0x0806,
  IPv4 = 0x0800,
  IPv6 = 0x86dd
} ether_type;

struct __ethernet_frame {
  volatile uint8_t dest_mac [6];
  volatile uint8_t source_mac [6];
  volatile ether_type ether_type;
  volatile uint8_t payload[];
  // volatile uint8_t crc[4];
} __attribute__((packed));
typedef struct __ethernet_frame ethernet_frame_t;

uint64_t get_source_mac (ethernet_frame_t *frame);
uint64_t get_dest_mac (ethernet_frame_t *frame);
