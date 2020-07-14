
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
#include "ethernet.h"

// OPCODES
#define ARP_REQUEST 0x0001
#define ARP_REPLY 0x0002

// HARDWARE TYPE
#define HTYPE_ETHERNET 1

// https://wiki.osdev.org/ARP
struct arp_frame {
  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_addr_len;  // ethernet = 6
  uint8_t protocol_addr_len;  // ipv4 = 4
  uint16_t opcode; // ARP OP Code: see above
  uint8_t data[];
} __attribute__((packed));
typedef struct arp_frame arp_frame_t;

struct arp_frame_ipv4 {
  uint8_t src_hardware_addr[6];
  uint32_t src_ip_address;
  uint8_t dest_hardware_addr[6];
  uint32_t dest_ip_address;
} __attribute__((packed));
typedef struct arp_frame_ipv4 arp_frame_ipv4_t;

// please remove when malloc exists
struct arp_frame_for_send {
  arp_frame_t header;
  arp_frame_ipv4_t body;
} __attribute__((packed));
typedef struct arp_frame_for_send arp_frame_for_send_t;

void arp_receive(ethernet_frame_t *frame);
void arp_handle_request(arp_frame_t *frame);
void arp_handle_reply(arp_frame_t *frame);
arp_frame_for_send_t arp_build_frame(uint16_t opcode, uint64_t dest_hw, uint32_t dest_ip, uint64_t src_hw, uint32_t src_ip);
uint64_t arp_get_src_hw_address(arp_frame_t *frame);
uint64_t arp_get_dest_hw_address(arp_frame_t *frame);
uint16_t arp_get_hw_type(arp_frame_t *frame);
uint16_t arp_get_protocol_type(arp_frame_t *frame);
uint32_t arp_get_src_ip_addr(arp_frame_t *frame);
uint32_t arp_get_dest_ip_addr(arp_frame_t *frame);
uint32_t arp_get_opcode(arp_frame_t *arp_frame);
