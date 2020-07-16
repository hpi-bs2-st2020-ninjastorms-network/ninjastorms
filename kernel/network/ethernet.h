
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

#define ETH_MAC_ADDRESS_LENGTH 6

typedef enum {
  TYPE_ARP = 0x0806,
  TYPE_IPv4 = 0x0800,
  TYPE_IPv6 = 0x86dd
} ether_type;

struct __mac_address {
  uint8_t address[6];
} __attribute__((packed));
typedef struct __mac_address mac_address_t;

struct __ethernet_frame {
  mac_address_t dest_mac;
  mac_address_t src_mac;
  ether_type ether_type;
  uint8_t payload[];
} __attribute__((packed));
typedef struct __ethernet_frame ethernet_frame_t;


void send_ethernet (mac_address_t dest_mac, ether_type ether_type, void *payload, size_t len);
const char *mac_to_str (mac_address_t mac);
