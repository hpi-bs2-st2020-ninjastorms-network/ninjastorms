
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

//#define ROUTING_DEBUG

#define MAX_ARP_TABLE_ENTRIES 10
#define MAX_ARP_TABLE_AGE 5 // seconds

#define NULL_ENTRY (arp_table_entry_t){0, {0}, 0}

struct __arp_table_entry {
  uint32_t ip;
  mac_address_t mac;
  clock_t entry_time;
};
typedef struct __arp_table_entry arp_table_entry_t;

mac_address_t arp_table_lookup(uint32_t ip);
mac_address_t arp_get_mac(uint32_t ip);
void add_arp_table_entry(mac_address_t mac, uint32_t ip);
void update_arp_table(mac_address_t mac, uint32_t ip);
void initialize_routing();
uint32_t arp_table_find(uint32_t ip);
