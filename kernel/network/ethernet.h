
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
#include <stdbool.h>

// #define ETHERNET_DEBUG
#define ETH_MAC_ADDRESS_LENGTH 6
#define MINIMUM_PAYLOAD_LENGTH 46

#define NULL_MAC      ((mac_address_t) {{0x00,0x00,0x00,0x00,0x00,0x00}})
#define BROADCAST_MAC ((mac_address_t) {{0xff,0xff,0xff,0xff,0xff,0xff}})

typedef enum
{
  TYPE_ARP = 0x0806,
  TYPE_IPv4 = 0x0800,
  TYPE_IPv6 = 0x86dd
} ether_type;

typedef struct __attribute__((packed))
{
  uint8_t address[6];
} mac_address_t;

typedef struct __attribute__((packed))
{
  mac_address_t dest_mac;
  mac_address_t src_mac;
  ether_type ether_type;
  uint8_t payload[];
} ethernet_frame_t;

void ethernet_send(mac_address_t dest_mac, ether_type eth_type, void *payload,
                   size_t len_payload);
const char *mac_to_str(mac_address_t mac);
bool mac_address_equal(mac_address_t mac1, mac_address_t mac2);
