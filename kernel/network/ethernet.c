
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
#include <stdbool.h>

#define INTTOHEXCHAR(val) ((val) > 9 ? (val)+'a'-10 : (val) + '0')

/*
 * Wraps the payload of length len_payload into an ethernet frame.
 * Adds 0-padding if necessary and hands the packet to the network card.
 */
void
ethernet_send(mac_address_t dest_mac, ether_type eth_type, void *payload,
              size_t len_payload)
{
  // make sure ethernet frame is at least 60 bytes long
  uint8_t len_padding = len_payload < MINIMUM_PAYLOAD_LENGTH
    ? MINIMUM_PAYLOAD_LENGTH - len_payload : 0;
  uint32_t len = len_padding + len_payload;
  ethernet_frame_t *eth_frame =
    (ethernet_frame_t *) malloc(sizeof(ethernet_frame_t) + len);
  eth_frame->dest_mac = hton_mac(dest_mac);
  eth_frame->src_mac = hton_mac(e1000_get_mac());
  eth_frame->ether_type = htons(eth_type);
  memcpy(eth_frame->payload, payload, len);

#ifdef ETHERNET_DEBUG
  LOG_DEBUG("Payload is %i, adding %i padding", len_payload, len_padding);
#endif
  if (len_padding != 0)
    {
      memset(eth_frame->payload + len_payload, 0, len_padding);
    }

  e1000_send_packet(eth_frame, sizeof(ethernet_frame_t) + len);

  free(eth_frame);
}

const char *
mac_to_str(mac_address_t mac)
{
  mac = hton_mac(mac);          // convert mac to big endian for processing
  char *tmp = "00:11:22:33:44:55";

  for (int i = 0, j = 0; i < 6; i++, j += 3)
    {
      tmp[j] = INTTOHEXCHAR(mac.address[i] >> 4);
      tmp[j + 1] = INTTOHEXCHAR(mac.address[i] & 0x0F);
    }
  return tmp;
}

bool
mac_address_equal(mac_address_t mac1, mac_address_t mac2)
{
  for (uint8_t i = 0; i < ETH_MAC_ADDRESS_LENGTH; i++)
    {
      if (mac1.address[i] != mac2.address[i])
        return 0;
    }
  return 1;
}
