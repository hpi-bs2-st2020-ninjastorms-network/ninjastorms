
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

#include "network_io.h"

#include "kernel/mmio.h"
#include "kernel/network/ethernet.h"

#include <stdbool.h>

/*
 * Converts int from host byte order to network byte order.
 */
uint32_t
htonl(uint32_t hostlong)
{
  if (is_big_endian())
    return hostlong;
  else
    return __builtin_bswap32(hostlong);
}

/*
 * Converts short from host byte order to network byte order.
 */
uint16_t
htons(uint16_t hostshort)
{
  if (is_big_endian())
    return hostshort;
  else
    return __builtin_bswap16(hostshort);
}

/*
 * Converts int from network byte order to host byte order.
 */
uint32_t
ntohl(uint32_t netlong)
{
  if (is_big_endian())
    return netlong;
  else
    return __builtin_bswap32(netlong);
}

/*
 * Converts short from network byte order to host byte order.
 */
uint16_t
ntohs(uint16_t netshort)
{
  if (is_big_endian())
    return netshort;
  else
    return __builtin_bswap16(netshort);
}

/*
 * Converts mac address from host byte order to network byte order.
 */
mac_address_t
hton_mac(mac_address_t mac)
{
  if (is_big_endian())
    return mac;
  else
    {
      mac_address_t new;
      for (int i = 0; i < 6; i++)
        new.address[5 - i] = mac.address[i];
      return new;
    }

}

/*
 * Converts mac address from network byte order to host byte order.
 */
mac_address_t
ntoh_mac(mac_address_t mac)
{
  if (is_big_endian())
    return mac;
  else
    {
      mac_address_t new;
      for (int i = 0; i < 6; i++)
        new.address[5 - i] = mac.address[i];
      return new;
    }
}

/*
 * Check whether current running mode is set to little or big endian
 * via looking at the system register.
 */
bool
is_big_endian()
{
  return (bool) (read32(SYS_CFGDATA2) & (1 << 1));
}
