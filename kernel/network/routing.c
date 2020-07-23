
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
#include "kernel/network/ethernet.h"
#include "kernel/network/routing.h"

static uint32_t next_slot = 0;
static arp_table_entry_t arp_table[MAX_ARP_TABLE_ENTRIES] = {0};

mac_address_t
get_mac_for_ip(uint32_t ip)
{
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      if (entry.ip == ip)
        {
#ifdef ROUTING_DEBUG
          log_debug("ip %x found in arp table", ip)
#endif
          return entry.mac;
        }
    }

  return NULL_MAC;
}

void
add_arp_table_entry(mac_address_t mac, uint32_t ip) {
#ifdef ROUTING_DEBUG
  log_debug("added ip %x in arp table", ip)
#endif
  arp_table_entry_t entry = {ip, mac};
  arp_table[next_slot] = entry;
  log_debug("added ip %x in arp table at slot %i", arp_table[next_slot].ip, next_slot)

  next_slot = (next_slot + 1) % MAX_ARP_TABLE_ENTRIES;
}

void
update_arp_table(mac_address_t mac, uint32_t ip) {
#ifdef ROUTING_DEBUG
  log_debug("Looking for ip %x in arp table", ip)
  log_debug("Using MAC %s in arp table", mac_to_str(mac))
#endif
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      log_debug("entry ip %x in arp table", entry.ip)
      if (entry.ip == ip)
        {
#ifdef ROUTING_DEBUG
          log_debug("updated ip %x in arp table", ip)
#endif
          entry.mac = mac;
          return;
        }
    }
  add_arp_table_entry(mac, ip);
}
