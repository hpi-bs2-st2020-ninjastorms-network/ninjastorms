
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

#define INTTOHEXCHAR(val) ((val) > 9 ? (val)+'a'-10 : (val) + '0')

void
send_ethernet(mac_address_t dest_mac, ether_type eth_type, void *payload, size_t len)
{
  // make sure ethernet frame is at least 60 bytes long
  if (len < 46)
    len = 46;
  ethernet_frame_t *eth_frame = (ethernet_frame_t *) malloc(sizeof(ethernet_frame_t) + len);
  eth_frame->dest_mac = hton_mac(dest_mac);
  eth_frame->src_mac = my_mac(); // mac address is stored in big endian on nc, so no swap
  eth_frame->ether_type = htons(eth_type);
  memcpy(eth_frame->payload, payload, len);

  send_packet(eth_frame, sizeof(ethernet_frame_t) + len);

  free(eth_frame);
}

const char *
mac_to_str(mac_address_t mac)
{
  // ab:cd:ef:gh:ij:kl
  char *tmp = "00:11:22:33:44:55";
  int i = 0;

  tmp[0] = INTTOHEXCHAR(mac.address[0] >> 4);
  tmp[1] = INTTOHEXCHAR(mac.address[0] & 0x0F);
  for(int i = 1; i < 6; i++)
    {
      int j = i*3;
      tmp[j-1] = ':';
      tmp[j] = INTTOHEXCHAR(mac.address[i] >> 4);
      tmp[j+1] = INTTOHEXCHAR(mac.address[i] & 0x0F);
    }
  return tmp;
}

uint8_t
mac_address_equal(mac_address_t mac1, mac_address_t mac2)
{
  for(uint8_t i = 0; i < ETH_MAC_ADDRESS_LENGTH; i++)
    {
      if(mac1.address[i] != mac2.address[i])
        return 0;
    }
  return 1;
}
