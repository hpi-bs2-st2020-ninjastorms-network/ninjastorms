
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

#include "kernel/network/ethernet.h"
#include "kernel/time.h"

#include <stdbool.h>

// #define ROUTING_DEBUG

#define MAX_ARP_TABLE_ENTRIES 	10
#define MAX_ARP_TABLE_AGE 		300     // seconds

#define ARP_NULL_ENTRY ((arp_table_entry_t){0, {{0}}, 0})

typedef struct
{
  uint32_t ip;
  mac_address_t mac;
  clock_t entry_time;
} arp_table_entry_t;

mac_address_t arp_table_lookup(uint32_t ip);
mac_address_t arp_table_get_mac(uint32_t ip);
bool arp_table_update(mac_address_t mac, uint32_t ip);
void arp_table_add_entry(mac_address_t mac, uint32_t ip);
void routing_init();
