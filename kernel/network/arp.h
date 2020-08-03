
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

#include "ethernet.h"

#include <sys/types.h>

// DEBUG LEVEL
// #define ARP_DEBUG

// OPCODES
#define ARP_REQUEST 0x0001
#define ARP_REPLY 0x0002

// HARDWARE TYPE
#define HTYPE_ETHERNET 1

// https://wiki.osdev.org/ARP
typedef struct __attribute__((packed))
{
  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_addr_len;    // ethernet = 6
  uint8_t protocol_addr_len;    // ipv4 = 4
  uint16_t opcode;              // ARP OP Code: see above
  mac_address_t src_hardware_addr;
  uint32_t src_ip_address;
  mac_address_t dest_hardware_addr;
  uint32_t dest_ip_address;
} arp_frame_t;

void arp_receive(ethernet_frame_t * frame);
void arp_send_request(uint32_t ip);
