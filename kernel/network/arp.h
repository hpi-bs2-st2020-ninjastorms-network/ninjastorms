
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
#include "ethernet_frame.h"

// OPCODES
#define ARP_REQUEST = 0x0001;
#define ARP_RESPONSE = 0x0002;

// https://wiki.osdev.org/ARP
struct arp {
  volatile uint16_t hardware_type;
  volatile uint16_t protocol_type;
  volatile uint16_t opcode; // ARP OP Code: see above

  volatile uint8_t src_mac_addr[6];
  volatile uint8_t dest_mac_addr[6];

  volatile uint32_t src_ip_address[4];
  volatile uint32_t dest_ip_address[4];
} __attribute__((packed));

void handle_arp(ethernet_frame_t *frame);
