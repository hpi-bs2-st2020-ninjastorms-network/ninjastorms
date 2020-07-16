
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

// https://developer.arm.com/documentation/dui0224/i/programmer-s-reference/status-and-system-control-registers/configuration-registers-sys-cfgdatax
// Register storing e.g. endianess
#define SYS_CFGDATA2 0x1000002C

// Convert int from host byte order to network byte order
uint32_t
htonl(uint32_t hostlong)
{
  return __builtin_bswap32(hostlong);
}

// Convert short from host byte order to network byte order
uint16_t
htons(uint16_t hostshort)
{
  return __builtin_bswap16(hostshort);
}

// Convert int from network byte order to host byte order
uint32_t
ntohl(uint32_t netlong)
{
  return __builtin_bswap32(netlong);
}

// Convert short from network byte order to host byte order
uint16_t
ntohs(uint16_t netshort)
{
  return __builtin_bswap16(netshort);
}

// Convert mac address from host byte order to network byte order
mac_address_t
hton_mac(mac_address_t mac)
{
  mac_address_t new;
  for(int i = 0; i < 6; i++)
    new.address[5-i] = mac.address[i];
  return new;
}

// Convert mac address from network byte order to host byte order
mac_address_t
ntoh_mac(mac_address_t mac)
{
  mac_address_t new;
  for(int i = 0; i < 6; i++)
    new.address[5-i] = mac.address[i];
  return new;
}

// Check whether current running mode is set to little or big endian
uint8_t
is_little_endian()
{
  return read32(SYS_CFGDATA2) & (1 << 1);
}
