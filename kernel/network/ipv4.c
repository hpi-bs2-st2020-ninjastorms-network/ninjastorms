
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

#include "ipv4.h"
#include "kernel/time.h"
#include "kernel/network/ethernet.h"
#include "kernel/network/routing.h"
#include "kernel/logger/logger.h"
#include <sys/types.h>

uint32_t
send_ipv4(uint32_t ip, void *payload, size_t len)
{
  // TODO: implement protocol
  mac_address_t dest_mac = arp_get_mac(ip);
  uint64_t start = clock_seconds();
  while(mac_address_equal(dest_mac, NULL_MAC))
    {
      dest_mac = arp_table_lookup(ip);
      if(clock_seconds() - start > WAIT_ON_ARP_TIMEOUT)
        {
#ifdef IPV4_DEBUG
          LOG_DEBUG("Sending to IP %x timed out waiting for arp", ip)
#endif
          return -1;
        }
    }

  send_ethernet(dest_mac, TYPE_IPv4, payload, len);
  return 0;
}
