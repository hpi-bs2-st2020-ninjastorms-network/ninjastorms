
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

#include "kernel/logger/logger.h"

static arp_table_entry_t arp_table[MAX_ARP_TABLE_ENTRIES];
static uint32_t next_slot = 0;

mac_address_t
get_mac_for_ip(uint32_t ip)
{
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      if (entry.ip == ip)
        log_debug("arp table used for %x", ip)
        return entry.mac;
    }

  // no arp table entry
  mac_address_t mac = 0; // send arp request

  arp_table_entry_t new_entry = { 0 };
  new_entry.ip = ip;
  new_entry.mac = mac;
  arp_table[next_slot] = new_entry;
  next_slot = (next_slot + 1) % MAX_ARP_TABLE_ENTRIES;

  return mac;
}
