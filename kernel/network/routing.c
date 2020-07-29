
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

#include "routing.h"
#include "kernel/logger/logger.h"
#include "kernel/network/ethernet.h"
#include "kernel/network/arp.h"

#include <stdlib.h>

uint32_t next_slot;
arp_table_entry_t *arp_table;

mac_address_t
arp_table_lookup(uint32_t ip)
{
  uint32_t position = arp_table_find(ip);
  if (position != -1)
    return arp_table[position].mac;
  else
    return NULL_MAC;
}

mac_address_t
arp_get_mac(uint32_t ip)
{
  uint32_t position = arp_table_find(ip);
  if (position != -1)
    return arp_table[position].mac;
  else
    arp_send_request(ip);
    return NULL_MAC;
}

void
add_arp_table_entry(mac_address_t mac, uint32_t ip) {
  arp_table_entry_t entry = {ip, mac};
  arp_table[next_slot] = entry;

#ifdef ROUTING_DEBUG
  log_debug("added ip %x in arp table at slot %i", ip, next_slot)
#endif

  next_slot = (next_slot + 1) % MAX_ARP_TABLE_ENTRIES;
}

void
update_arp_table(mac_address_t mac, uint32_t ip) {
  uint32_t position = arp_table_find(ip);
  if (position != -1)
    {
      arp_table[position].mac = mac;
#ifdef ROUTING_DEBUG
      log_debug("updated ip %x in arp table", ip)
#endif
      return;
    }

  add_arp_table_entry(mac, ip);
}

uint32_t
arp_table_find(uint32_t ip)
{
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      if (entry.ip == ip)
        {
#ifdef ROUTING_DEBUG
          log_debug("ip %x found in arp table at %i", ip, i)
#endif
          return i;
        }
    }
  return -1;
}

void
initialize_routing()
{
  next_slot = 0;
  size_t size = MAX_ARP_TABLE_ENTRIES * sizeof(arp_table_entry_t);
  arp_table = (arp_table_entry_t *) malloc(size);
}
