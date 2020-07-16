
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

#include "ethernet.h"
#include "kernel/network/arp.h"
#include "kernel/network/e1000.h"
#include "kernel/network/network_io.h"
#include "kernel/logger/logger.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

void
send_ethernet(mac_address_t dest_mac, ether_type eth_type, void *payload, size_t len)
{
  // make sure ethernet frame is at least 60 bytes long
  if (len < 46)
    len = 46;
  ethernet_frame_t *eth_frame = (ethernet_frame_t *) malloc(sizeof(ethernet_frame_t) + len);
  uint64_t converted_dest_mac = switch_endian64(dest_mac << 16);

  memcpy(eth_frame->dest_mac, &converted_dest_mac, 6);
  copy_my_mac(eth_frame->src_mac);
  eth_frame->ether_type = switch_endian16(eth_type);
  memcpy(eth_frame->payload, payload, len);

  send_packet(eth_frame, sizeof(ethernet_frame_t) + len);

  free(eth_frame);
}