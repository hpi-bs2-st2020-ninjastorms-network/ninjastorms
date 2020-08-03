
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

#include "kernel/time.h"
#include "kernel/logger/logger.h"
#include "kernel/network/ethernet.h"
#include "kernel/network/arp.h"
#include "kernel/network/ipv4.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

static arp_table_entry_t *arp_table = NULL;

/*
 * Checks if the arp table contains the ip. Removes expired entries.
 * Returns the index on success, otherwise -1.
 */
uint32_t
arp_table_find(uint32_t ip)
{
  if (arp_table == NULL)
    return -1;

  uint32_t res = -1;
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      if (entry.entry_time
          && clock_seconds() - entry.entry_time > MAX_ARP_TABLE_AGE)
        {
          // entry is too old, make it invalid
#ifdef ROUTING_DEBUG
          LOG_DEBUG("Entry at %i is too old with time %i", i,
                    (uint32_t) arp_table[i].entry_time);
#endif
          arp_table[i] = ARP_NULL_ENTRY;
        }
      else if (entry.ip == ip)
        {
          res = i;
          arp_table[i].entry_time = clock_seconds();
#ifdef ROUTING_DEBUG
          LOG_DEBUG("IP %x found in arp table at %i new time %i", ip, i,
                    (uint32_t) arp_table[i].entry_time);
#endif
        }
    }
  return res;
}

/*
 * Checks if the arp table contains the ip and returns the corresponding mac on success.
 * Returns the NULL_MAC otherwise.
 */
mac_address_t
arp_table_lookup(uint32_t ip)
{
  if (arp_table == NULL)
    return NULL_MAC;

  uint32_t position = arp_table_find(ip);
  if (position != -1)
    return arp_table[position].mac;
  else
    return NULL_MAC;
}

/*
 * Checks if the arp table contains the ip.
 * Returns the corresponding mac on success, otherwise it sends an arp request for the ip.
 */
mac_address_t
arp_table_get_mac(uint32_t ip)
{
  if (arp_table == NULL)
    return NULL_MAC;

  mac_address_t mac = arp_table_lookup(ip);
  if (mac_address_equal(NULL_MAC, mac))
    {
      arp_send_request(ip);
      return NULL_MAC;
    }
  else
    return mac;
}

/*
 * Example: 
 *  Time   |       MAC         |     IP
 * --------+-------------------+------------------
 *   100   | AB:CD:EF:00:11:22 | 10.10.10.10
 */
void
arp_table_print(void)
{
  LOG_DEBUG("Arp Table:");
  printf(" Time   |       MAC         |     IP\n");
  printf("--------+-------------------+------------------\n");
  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      arp_table_entry_t entry = arp_table[i];
      printf(" %i\t| %s | ", (uint32_t) entry.entry_time,
             mac_to_str(entry.mac));
      ipv4_print(entry.ip);
      printf("\n");
    }
}

/*
 * Adds a new entry in the arp table at a free slot if existing,
 * otherwise overwrites the oldest entry.
 */
void
arp_table_add_entry(mac_address_t mac, uint32_t ip)
{
  if (arp_table == NULL)
    return;

  uint32_t slot = 0;

  for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
    {
      if (!arp_table[i].entry_time)
        {
          slot = i;
          break;
        }
      else if (arp_table[i].entry_time < arp_table[slot].entry_time)
        slot = i;
    }
  /* *INDENT-OFF* */
  arp_table[slot] = (arp_table_entry_t) {ip, mac, clock_seconds()};
  /* *INDENT-ON* */

#ifdef ROUTING_DEBUG
  LOG_DEBUG("Added ip %x in arp table at slot %i with time %i", ip, slot,
            (uint32_t) arp_table[slot].entry_time);
  arp_table_print();
#endif
}

/*
 * Updates an arp table entry if existing.
 * Returns true iff updated.
 */
bool
arp_table_update(mac_address_t mac, uint32_t ip)
{
  if (arp_table == NULL)
    return false;

  uint32_t position = arp_table_find(ip);
  if (position != -1)
    {
      arp_table[position].mac = mac;
#ifdef ROUTING_DEBUG
      LOG_DEBUG("Updated ip %x in arp table", ip);
      arp_table_print();
#endif
      return true;
    }
#ifdef ROUTING_DEBUG
  LOG_DEBUG("IP %x not found in arp table", ip);
#endif
  return false;
}

/*
 * Initializes the arp table and sets all entries to ARP_NULL_ENTRY.
 */
void
routing_init()
{
  if (arp_table == NULL)
    {
      size_t size = MAX_ARP_TABLE_ENTRIES * sizeof(arp_table_entry_t);
      arp_table = (arp_table_entry_t *) malloc(size);
      if (arp_table == NULL)
        {
          LOG_ERROR("Allocating memory for arp table failed!");
          return;
        }
      for (uint32_t i = 0; i < MAX_ARP_TABLE_ENTRIES; i++)
        arp_table[i] = ARP_NULL_ENTRY;
    }
}
